#include <BoolCharacteristics.h>

#include "../Helpers.h"

using namespace hap;

BoolCharacteristics::BoolCharacteristics(char_type _type, permission _premission)
	: Characteristics(_type, _premission)
{
}

std::string BoolCharacteristics::value(net::ConnectionInfo *sender) 
{
	if (perUserQuery != nullptr && sender != nullptr)
		return perUserQuery(sender);
	if (_value)
		return "1";
	return "0";
}

void BoolCharacteristics::setValue(std::string str, net::ConnectionInfo *sender) 
{
	bool newValue = ("true" == str || "1" == str);
	if (valueChangeFunctionCall != nullptr && sender != nullptr)
		valueChangeFunctionCall(_value, newValue, sender);
	_value = newValue;
}

std::string BoolCharacteristics::describe(net::ConnectionInfo *sender)
{
	std::string result;
	if (premission & permission_read) {
		result += wrap("value") + ":";
		result += value(sender);
		result += ",";
	}

	result += wrap("perms") + ":";
	result += "[";
	if (premission & permission_read) result += wrap("pr") + ",";
	if (premission & permission_write) result += wrap("pw") + ",";
	if (premission & permission_notify) result += wrap("ev") + ",";
	result = result.substr(0, result.size() - 1);
	result += "]";
	result += ",";

	char tempStr[4];
	snprintf(tempStr, 4, "%X", type);
	result += wrap("type") + ":" + wrap(tempStr);
	result += ",";

	snprintf(tempStr, 4, "%hd", iid);
	result += wrap("iid") + ":" + tempStr;
	result += ",";

	result += "\"format\":\"bool\"";

	return "{" + result + "}";
}