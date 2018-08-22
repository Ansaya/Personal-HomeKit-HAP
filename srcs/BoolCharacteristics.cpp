#include <BoolCharacteristics.h>
#include <../Configuration.h>

#include "../Helpers.h"

using namespace hap;

BoolCharacteristics::BoolCharacteristics(char_type _type, permission _premission)
	: Characteristics(_type, _premission)
{
}

std::string BoolCharacteristics::getValue()
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
	bool _newValue = ("true" == newValue || "1" == newValue);

#ifdef HAP_THREAD_SAFE
	std::unique_lock<std::mutex> lock(_valueHandle);
#endif

	if (_valueChangeCB != nullptr && sender != nullptr)
		_valueChangeCB(_value, _newValue, sender);

	_value = _newValue;
}

std::string BoolCharacteristics::describe()
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

	char tempStr[4];
	snprintf(tempStr, 4, "%X", _type);
	result += wrap("type") + ":" + wrap(tempStr);
	result += ",";

	snprintf(tempStr, 4, "%hd", getID());
	result += wrap("iid") + ":" + tempStr;
	result += ",";

	result += "\"format\":\"bool\"";

	return "{" + result + "}";
}

void BoolCharacteristics::setValueChangeCB(
	std::function<void(bool oldValue, bool newValue, void* sender)> cb)
{
	_valueChangeCB = cb;
}