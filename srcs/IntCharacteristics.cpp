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

std::string IntCharacteristics::getValue() const
{
#ifdef HAP_THREAD_SAFE
	std::unique_lock<std::mutex> lock(_valueHandle);
#endif

	return std::to_string(_value);
}

void IntCharacteristics::setValue(const std::string& newValue, void* sender)
{
	return setValue(std::stoi(newValue), sender);
}

void IntCharacteristics::setValue(int newValue, void* sender)
{
	if (newValue > _maxVal) {
		newValue = _maxVal;
	}
	else if (newValue < _minVal) {
		newValue = _minVal;
	}

	int oldValue;

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

std::string IntCharacteristics::describe() const
{
	std::string result;

	if (_permission & permission_read) {
		result += wrap("value") + ":" + getValue();
		result += ",";
	}

	if (_minVal != std::numeric_limits<int>::min())
		result += wrap("minValue") + ":" + std::to_string(_minVal) + ",";


	if (_maxVal != std::numeric_limits<int>::max())
		result += wrap("maxValue") + ":" + std::to_string(_maxVal) + ",";

	if (_step > 0)
		result += wrap("minStep") + ":" + std::to_string(_step) + ",";

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
	case unit_none:
	default:
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