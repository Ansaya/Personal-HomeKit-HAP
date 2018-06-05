#include <IntCharacteristics.h>

#include "../Helpers.h"

using namespace hap;

IntCharacteristics::IntCharacteristics(char_type _type, permission _premission, int minVal, int maxVal, int step, unit charUnit)
	: Characteristics(_type, _premission), _minVal(minVal), _maxVal(maxVal), _step(step), _unit(charUnit)
{
	_value = minVal;
}

std::string IntCharacteristics::value(net::ConnectionInfo *sender)
{
	if (perUserQuery != nullptr && sender != nullptr)
		return perUserQuery(sender);

	return std::to_string((int)_value);
}

void IntCharacteristics::setValue(std::string str, net::ConnectionInfo *sender)
{
	float temp = atoi(str.c_str());
	if (valueChangeFunctionCall != nullptr && sender != nullptr)
		valueChangeFunctionCall(_value, temp, sender);
	_value = temp;
}

std::string IntCharacteristics::describe(net::ConnectionInfo *sender)
{
	std::string result;
	char tempStr[16];

	if (premission & permission_read) {
		result += wrap("value") + ":" + value(sender);
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
	if (premission & permission_read) result += wrap("pr") + ",";
	if (premission & permission_write) result += wrap("pw") + ",";
	if (premission & permission_notify) result += wrap("ev") + ",";
	result = result.substr(0, result.size() - 1);
	result += "]";
	result += ",";

	snprintf(tempStr, 16, "%X", type);
	result += wrap("type") + ":" + wrap(tempStr);
	result += ",";

	snprintf(tempStr, 16, "%hd", iid);
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