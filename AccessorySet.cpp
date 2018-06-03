#include "AccessorySet.h"

#include "Helpers.h"

using namespace hap;

AccessorySet& AccessorySet::getInstance() {
	static AccessorySet instance;

	return instance;
}

short AccessorySet::numberOfAccessory() {
	return _accessories.size();
}

Accessory *AccessorySet::accessoryAtIndex(int index) {
	for (auto it = _accessories.begin(); it != _accessories.end(); it++) {
		if ((*it)->aid == index) {
			return *it;
		}
	}
	return nullptr;
}

void AccessorySet::addAccessory(Accessory *acc) {
	acc->aid = ++_aid;
	_accessories.push_back(acc);
}

bool AccessorySet::removeAccessory(Accessory *acc) {
	bool exist = false;
	for (auto it = _accessories.begin(); it != _accessories.end(); it++) {
		if (*it == acc) {
			_accessories.erase(it);
			exist = true;
		}
	}
	return exist;
}

std::string AccessorySet::describe(net::ConnectionInfo *sender) {
	int numberOfAcc = numberOfAccessory();
	std::string *desc = new std::string[numberOfAcc];
	for (int i = 0; i < numberOfAcc; i++) {
		desc[i] = _accessories[i]->describe(sender);
	}

	std::string result = arrayWrap(desc, numberOfAcc);
	delete[] desc;
	std::string key = "accessories";

	result = dictionaryWrap(&key, &result, 1);
	return result;
}