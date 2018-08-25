#include <Service.h>
#include <../Configuration.h>

#include <Accessory.h>
#include <Characteristics.h>
#include "../Helpers.h"

#include <algorithm>

using namespace hap;

Service::Service(service_type type) : _type(type)
{
}

int Service::getID() const
{
	return _id;
}

uint16_t Service::characteristicsCount() const
{ 
	return _characteristics.size();
}

void Service::addCharacteristic(Characteristics_ptr newCharacteristic)
{
	if (_parentInstancesID != nullptr) {
		newCharacteristic->_id = _parentInstancesID->fetch_add(1);
		newCharacteristic->_aid = _aid;
	}

#ifdef HAP_THREAD_SAFE
	std::unique_lock<std::mutex> lock(_characteristicsList);
#endif // HAP_THREAD_SAFE

	_characteristics.push_back(newCharacteristic);
}

bool Service::removeCharacteristic(Characteristics_ptr characteristic)
{
#ifdef HAP_THREAD_SAFE
	std::unique_lock<std::mutex> lock(_characteristicsList);
#endif

	for (auto it = _characteristics.begin(); it != _characteristics.end(); ++it) {
		if (*it == characteristic) {
			_characteristics.erase(it);
			return true;
		}
	}

	return false;
}

Characteristics_ptr Service::getCharacteristic(int id) const
{ 
#ifdef HAP_THREAD_SAFE
	std::unique_lock<std::mutex> lock(_characteristicsList);
#endif

	auto c = std::find_if(_characteristics.begin(), _characteristics.end(), 
		[id](const Characteristics_ptr ch) { return ch->_id == id; });

	if (c != _characteristics.end())
		return *c;

	return nullptr;
}

std::string Service::describe() const
{
	std::string keys[3] = { "iid", "type", "characteristics" };
	std::string values[3];

	values[0] = std::to_string(_id);
	{
		char temp[8];
		snprintf(temp, 8, "\"%X\"", _type);
		values[1] = temp;
	}

	{
#ifdef HAP_THREAD_SAFE
		std::unique_lock<std::mutex> lock(_characteristicsList);
#endif

		int no = _characteristics.size();
		std::string *chars = new std::string[no];
		for (int i = 0; i < no; i++) {
			chars[i] = _characteristics[i]->describe();
		}
		values[2] = arrayWrap(chars, no);
		delete[] chars;
	}
	return dictionaryWrap(keys, values, 3);
}