#include <IntCharacteristics.h>
#include <../Configuration.h>

#include "../Helpers.h"

using namespace hap;

IntCharacteristics::IntCharacteristics(char_type type, permission premission, 
	int minVal, int maxVal, int step, unit charUnit)
	: Characteristics(type, premission), 
	_value(minVal), _minVal(minVal), _maxVal(maxVal), _step(step), _unit(charUnit)
{
}

std::string IntCharacteristics::getValue()
{
#ifdef HAP_THREAD_SAFE
	std::unique_lock<std::mutex> lock(_valueHandle);
#endif

	return std::to_string(_value);
}

void IntCharacteristics::setValue(const std::string& newValue, void* sender)
{
	float temp = std::stof(newValue);
	if (temp > _maxVal)
		temp = _maxVal;
	if (temp < _minVal)
		temp = _minVal;

	int oldValue;

	{
#ifdef HAP_THREAD_SAFE
		std::unique_lock<std::mutex> lock(_valueHandle);
#endif
		oldValue = _value;

		_value = temp;
	}

	if (_valueChangeCB != nullptr && sender != nullptr) {
		_valueChangeCB(oldValue, temp, sender);
	}
}

std::string IntCharacteristics::describe()
{
	std::string result;
	char tempStr[16];

	if (_permission & permission_read) {
		result += wrap("value") + ":" + getValue();
		result += ",";
	}

	snprintf(tempStr, 16, "%d", _minVal);
	if (_minVal != INT32_MIN)
		result += wrap("minValue") + ":" + tempStr + ",";

	snprintf(tempStr, 16, "%d", _maxVal);
	if (_maxVal != INT32_MAX)
		result += wrap("maxValue") + ":" + tempStr + ",";

	snprintf(tempStr, 16, "%d", _step);
	if (_step > 0)
		result += wrap("minStep") + ":" + tempStr + ",";

	result += wrap("perms") + ":";
	result += "[";
	if (_permission & permission_read) result += wrap("pr") + ",";
	if (_permission & permission_write) result += wrap("pw") + ",";
	if (_permission & permission_notify) result += wrap("ev") + ",";
	result = result.substr(0, result.size() - 1);
	result += "]";
	result += ",";

	snprintf(tempStr, 16, "%X", _type);
	result += wrap("type") + ":" + wrap(tempStr);
	result += ",";

	snprintf(tempStr, 16, "%hd", getID());
	result += wrap("iid") + ":" + tempStr;
	result += ",";

	switch (_unit) {
	case unit_arcDegree:
		result += wrap("unit") + ":" + wrap("arcdegrees") + ",";
		break;
	case unit_celsius:
		result += wrap("unit") + ":" + wrap("celsius") + ",";
		break;
	case unit_percentage:
		result += wrap("unit") + ":" + wrap("percentage") + ",";
		break;
	}

	result += "\"format\":\"int\"";

	return "{" + result + "}";
}

void IntCharacteristics::setValueChangeCB(
	std::function<void(int oldValue, int newValue, void* sender)> cb)
{
	_valueChangeCB = cb;
}