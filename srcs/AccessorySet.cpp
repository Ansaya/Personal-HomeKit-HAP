#include <AccessorySet.h>
#include <../Configuration.h>

#include <Accessory.h>
#include <Service.h>
#include <Characteristics.h>
#include "../Helpers.h"

#include <algorithm>

using namespace hap;

AccessorySet::AccessorySet() : _accessoryID(1)
{
}

AccessorySet& AccessorySet::getInstance() {
	static AccessorySet instance;

	return instance;
}

uint16_t AccessorySet::accessoriesCount() {
	return _accessories.size();
}

Accessory_ptr AccessorySet::getAccessory(uint16_t id)
{
#ifdef HAP_THREAD_SAFE
	std::unique_lock<std::mutex> lock(_accessoriesList);
#endif

	auto a = std::find_if(_accessories.begin(), _accessories.end(), 
		[id](const Accessory_ptr ap) { return ap->getID() == id; });

	if (a != _accessories.end())
		return *a;

	return nullptr;
}

void AccessorySet::addAccessory(Accessory_ptr accessory)
{
	accessory->_id = _accessoryID.fetch_add(1);

#ifdef HAP_THREAD_SAFE
	std::unique_lock<std::mutex> servicesLock(accessory->_servicesList);
#endif

	for (auto& s : accessory->_services) {
		s->_aid = accessory->_id;

#ifdef HAP_THREAD_SAFE
		std::unique_lock<std::mutex> charsLock(s->_characteristicsList);
#endif

		for (auto& c : s->_characteristics)
			c->_aid = accessory->_id;
	}

#ifdef HAP_THREAD_SAFE
	std::unique_lock<std::mutex> lock(_accessoriesList);
#endif

	_accessories.push_back(accessory);
}

bool AccessorySet::removeAccessory(Accessory_ptr acc)
{
#ifdef HAP_THREAD_SAFE
	std::unique_lock<std::mutex> lock(_accessoriesList);
#endif

	for (auto it = _accessories.begin(); it != _accessories.end(); ++it) {
		if (*it == acc) {
			_accessories.erase(it);
			return true;
		}
	}

	return false;
}

std::string AccessorySet::describe()
{
	//std::unique_lock<std::mutex> lock(_accessoriesList);

	int numberOfAcc = _accessories.size();
	std::string *desc = new std::string[numberOfAcc];
	for (int i = 0; i < numberOfAcc; i++) {
		desc[i] = _accessories[i]->describe();
	}

	std::string result = arrayWrap(desc, numberOfAcc);
	delete[] desc;
	std::string key = "accessories";

	result = dictionaryWrap(&key, &result, 1);
	return result;
}