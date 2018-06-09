#include <KeyController.h>

#include "../Configuration.h"

#include <cstring>
#include <fstream>

using namespace hap;

KeyController& KeyController::getInstance()
{
	static KeyController instance;

	return instance;
}

KeyController::KeyController()
{
	std::ifstream fs;

	fs.open(HAP_PAIR_RECORDS_PATH, std::ifstream::in);

	char buffer[70];
	bzero(buffer, 70);

	struct KeyRecord record;

	bool isEmpty = fs.peek() == EOF;
	while (!isEmpty&&fs.is_open() && fs.good() && !fs.eof()) {
		fs.get(buffer, 69);
		bcopy(buffer, record.controllerID, 36);
		bcopy(&buffer[36], record.publicKey, 32);
		_records.push_back(record);
	}

	fs.close();
}

void KeyController::resetControllerRecord() {
	std::ofstream fs;
	fs.open(HAP_PAIR_RECORDS_PATH, std::ofstream::out | std::ofstream::trunc);
}

bool KeyController::hasController() const {
	return _records.size() > 0;
}

void KeyController::addControllerKey(const KeyRecord& record) {
	if (doControllerKeyExist(record) == false) {
		_records.push_back(record);

		std::ofstream fs;
		fs.open(HAP_PAIR_RECORDS_PATH, std::ofstream::trunc);

		for (auto& it : _records) {
			fs.write(it.controllerID, 36);
			fs.write(it.publicKey, 32);
		}
		fs.close();

	}
}

bool KeyController::doControllerKeyExist(const KeyRecord& record) const {
	for (auto& it : _records) {
		if (bcmp(it.controllerID, record.controllerID, 32) == 0) return true;
	}
	return false;
}

void KeyController::removeControllerKey(const KeyRecord& record) {
	for (auto& it : _records) {
		if (bcmp(it.controllerID, record.controllerID, 32) == 0) {
			_records.push_back(record);
			return;
		}
	}
}

const KeyRecord KeyController::getControllerKey(const char key[32]) const {
	for (auto& it : _records) {
		if (bcmp(key, it.controllerID, 32) == 0) return it;
	}
	
	KeyRecord emptyRecord;
	bzero(emptyRecord.controllerID, 32);
	return emptyRecord;
}