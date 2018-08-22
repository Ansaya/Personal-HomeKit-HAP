#include <StringCharacteristics.h>

#include "../Helpers.h"

using namespace hap;

StringCharacteristics::StringCharacteristics(
	char_type type, permission permission, uint16_t maxLen)
	: Characteristics(type, permission), _maxLen(maxLen)
{
}

std::string StringCharacteristics::getValue()
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

#ifdef HAP_THREAD_SAFE
	std::unique_lock<std::mutex> lock(_valueHandle);
#endif

	if (_valueChangeCB != nullptr && sender != nullptr)
		_valueChangeCB(_value, newValue, sender);
	_value = newValue;
}

std::string StringCharacteristics::describe()
{
	std::string result;
	char tempStr[4];

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

	snprintf(tempStr, 4, "%X", _type);
	result += wrap("type") + ":" + wrap(tempStr);
	result += ",";

	snprintf(tempStr, 4, "%hd", getID());
	result += wrap("iid") + ":" + tempStr;
	result += ",";

	if (_maxLen > 0) {
		snprintf(tempStr, 4, "%hd", _maxLen);
		result += wrap("maxLen") + ":" + tempStr;
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