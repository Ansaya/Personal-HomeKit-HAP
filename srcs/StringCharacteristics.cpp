#include <StringCharacteristics.h>
#include <../Configuration.h>

#include "../Helpers.h"

using namespace hap;

StringCharacteristics::StringCharacteristics(
	char_type type, permission permission, uint8_t maxLen)
	: Characteristics(type, permission), _maxLen(maxLen)
{
}

std::string StringCharacteristics::getValue() const
{
#ifdef HAP_THREAD_SAFE
	std::unique_lock<std::mutex> lock(_valueHandle);
#endif

	return "\"" + _value + "\"";
}

void StringCharacteristics::setValue(const std::string& newValue, void* sender)
{
	std::string _newValue = newValue.length() > _maxLen ? newValue.substr(0, _maxLen) 
		: newValue;

	std::string oldValue;

	{
#ifdef HAP_THREAD_SAFE
		std::unique_lock<std::mutex> lock(_valueHandle);
#endif
		oldValue = _value;

		_value = _newValue;
	}

	if (sender == nullptr) {
		notify();
	}

	if (_valueChangeCB != nullptr && sender != nullptr) {
		_valueChangeCB(oldValue, _newValue, sender);
	}
}

std::string StringCharacteristics::describe() const
{
	std::string result;

	if (_permission & permission_read) {
		result += wrap("value") + ":" + getValue();
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

	if (_maxLen != 64) {
		result += wrap("maxLen") + ":" + std::to_string(_maxLen);
		result += ",";
	}

	result += "\"format\":\"string\"";

	return "{" + result + "}";
}

void StringCharacteristics::setValueChangeCB(
	std::function<void(const std::string& oldValue, const std::string& newValue, void* sender)> cb)
{
	_valueChangeCB = cb;
}