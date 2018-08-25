#include <BoolCharacteristics.h>
#include <../Configuration.h>

#include "../Helpers.h"

using namespace hap;

BoolCharacteristics::BoolCharacteristics(char_type _type, permission _premission)
	: Characteristics(_type, _premission)
{
}

std::string BoolCharacteristics::getValue() const
{
#ifdef HAP_THREAD_SAFE
	std::unique_lock<std::mutex> lock(_valueHandle);
#endif

	if (_value)
		return "1";
	return "0";
}

void BoolCharacteristics::setValue(const std::string& newValue, void* sender)
{
	return setValue((bool)("true" == newValue || "1" == newValue), sender);
}

void BoolCharacteristics::setValue(bool newValue, void* sender)
{
	bool oldValue;

	{
#ifdef HAP_THREAD_SAFE
		std::unique_lock<std::mutex> lock(_valueHandle);
#endif
		oldValue = _value;

		_value = newValue;
	}

	if (sender == nullptr) {
		notify();
	}

	if (_valueChangeCB != nullptr && sender != nullptr) {
		_valueChangeCB(oldValue, newValue, sender);
	}
}

std::string BoolCharacteristics::describe() const
{
	std::string result;
	if (_permission & permission_read) {
		result += wrap("value") + ":";
		result += getValue();
		result += ",";
	}

	result += wrap("perms") + ":";
	result += "[";
	if (_permission & permission_read) result += wrap("pr") + ",";
	if (_permission & permission_write) result += wrap("pw") + ",";
	if (_permission & permission_notify) result += wrap("ev") + ",";
	result = result.substr(0, result.size() - 1);
	result += "]";
	result += ",";

	{
		char temp[8];
		snprintf(temp, 8, "%X", _type);
		result += wrap("type") + ":" + wrap(temp);
		result += ",";
	}

	result += wrap("iid") + ":" + std::to_string(getID());
	result += ",";

	result += "\"format\":\"bool\"";

	return "{" + result + "}";
}

void BoolCharacteristics::setValueChangeCB(
	std::function<void(bool oldValue, bool newValue, void* sender)> cb)
{
	_valueChangeCB = cb;
}