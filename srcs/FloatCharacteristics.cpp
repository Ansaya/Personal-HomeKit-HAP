#include <FloatCharacteristics.h>
#include <../Configuration.h>

#include "../Helpers.h"

using namespace hap;

FloatCharacteristics::FloatCharacteristics(char_type type, permission premission, 
	double minVal, double maxVal, double step, unit charUnit)
	: Characteristics(type, premission), 
	_minVal(minVal), _maxVal(maxVal), _step(step), _unit(charUnit)
{
}

std::string FloatCharacteristics::getValue() const
{
#ifdef HAP_THREAD_SAFE
	std::unique_lock<std::mutex> lock(_valueHandle);
#endif

	return std::to_string(_value);
}

void FloatCharacteristics::setValue(const std::string& newValue, void* sender)
{
	return setValue(std::stod(newValue), sender);
}

void FloatCharacteristics::setValue(double newValue, void* sender)
{
	if (newValue > _maxVal) {
		newValue = _maxVal;
	}
	else if (newValue < _minVal) {
		newValue = _minVal;
	}

	double oldValue;

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

std::string FloatCharacteristics::describe() const
{
	std::string result;

	if (_permission & permission_read) {
		result += wrap("value") + ":" + getValue();
		result += ",";
	}

	if (_minVal != std::numeric_limits<double>::min())
		result += wrap("minValue") + ":" + std::to_string(_minVal) + ",";

	if (_maxVal != std::numeric_limits<double>::max())
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

	result += "\"format\":\"float\"";

	return "{" + result + "}";
}

void FloatCharacteristics::setValueChangeCB(
	std::function<void(double oldValue, double newValue, void* sender)> cb)
{
	_valueChangeCB = cb;
}