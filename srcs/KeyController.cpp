#include <KeyController.h>

#include "../Configuration.h"

#include <cstring>
#include <fstream>
#include <cstdio>

using namespace hap;

KeyController& KeyController::getInstance()
{
	static KeyController instance;

	return instance;
}

KeyController::KeyController() : _filePath("./hap_client_keys")
{
	std::unique_lock<std::mutex> lock(_file);

	std::ifstream fs;

	fs.open(_filePath, std::ifstream::in);

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

bool KeyController::setKeyRecordPath(const std::string& filePath)
{
	std::ofstream fs;
	fs.open(filePath);

	if (fs.fail()) {
		return false;
	}
	else {
		std::unique_lock<std::mutex> lock(_file);

		std::ifstream oldfs;
		oldfs.open(_filePath, std::ifstream::in);

		fs << oldfs.rdbuf();

		oldfs.close();
		std::remove(_filePath.c_str());

		_filePath = filePath;
		return true;
	}
}

void KeyController::resetControllerRecord() {
	std::ofstream fs;
	fs.open(_filePath, std::ofstream::out | std::ofstream::trunc);
}

bool KeyController::hasController() const {
	return _records.size() > 0;
}

void KeyController::addControllerKey(const KeyRecord& record) {
	if (doControllerKeyExist(record) == false) {
		std::unique_lock<std::mutex> lock(_file);

		_records.push_back(record);

		_writeBack();
	}
}

bool KeyController::doControllerKeyExist(const KeyRecord& record) const 
{
	for (auto& it : _records) {
		if (bcmp(it.controllerID, record.controllerID, 32) == 0) return true;
	}
	return false;
}

void KeyController::removeControllerKey(const KeyRecord& record)
{
	std::unique_lock<std::mutex> lock(_file);

	for (auto& it : _records) {
		if (bcmp(it.controllerID, record.controllerID, 32) == 0) {
			_records.push_back(record);

			_writeBack();
			return;
		}
	}
}

const KeyRecord KeyController::getControllerKey(const char key[32]) const 
{
	for (auto& it : _records) {
		if (bcmp(key, it.controllerID, 32) == 0) return it;
	}
	
	KeyRecord emptyRecord;
	bzero(emptyRecord.controllerID, 32);
	return emptyRecord;
}

void KeyController::_writeBack()
{
	std::ofstream fs;
	fs.open(_filePath, std::ofstream::trunc);

	for (auto& it : _records) {
		fs.write(it.controllerID, 36);
		fs.write(it.publicKey, 32);
	}
	fs.close();
}