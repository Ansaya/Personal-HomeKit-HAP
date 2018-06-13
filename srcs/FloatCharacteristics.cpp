#include <FloatCharacteristics.h>

#include "../Helpers.h"

using namespace hap;

FloatCharacteristics::FloatCharacteristics(char_type type, permission premission, 
	float minVal, float maxVal, float step, unit charUnit)
	: Characteristics(type, premission), 
	_minVal(minVal), _maxVal(maxVal), _step(step), _unit(charUnit)
{
}

std::string FloatCharacteristics::getValue()
{
	std::unique_lock<std::mutex> lock(_valueHandle);

	return std::to_string(_value);
}

void FloatCharacteristics::setValue(const std::string& newValue, void* sender)
{
	float temp = std::stof(newValue);
	if (temp > _maxVal)
		temp = _maxVal;
	if (temp < _minVal)
		temp = _minVal;

	std::unique_lock<std::mutex> lock(_valueHandle);

	if (_valueChangeCB != nullptr && sender != nullptr)
		_valueChangeCB(_value, temp, sender);

	_value = temp;
}

std::string FloatCharacteristics::describe()
{
	std::string result;
	char tempStr[16];

	if (_permission & permission_read) {
		result += wrap("value") + ":" + getValue();
		result += ",";
	}

	snprintf(tempStr, 16, "%f", _minVal);
	if (_minVal != INT32_MIN)
		result += wrap("minValue") + ":" + tempStr + ",";

	snprintf(tempStr, 16, "%f", _maxVal);
	if (_maxVal != INT32_MAX)
		result += wrap("maxValue") + ":" + tempStr + ",";

	snprintf(tempStr, 16, "%f", _step);
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

	result += "\"format\":\"float\"";

	return "{" + result + "}";
}

void FloatCharacteristics::setValueChangeCB(
	std::function<void(float oldValue, float newValue, void* sender)> cb)
{
	_valueChangeCB = cb;
}