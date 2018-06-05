#include <StringCharacteristics.h>

#include "../Helpers.h"

using namespace hap;

StringCharacteristics::StringCharacteristics(char_type _type, permission _premission, unsigned short _maxLen)
	: Characteristics(_type, _premission), maxLen(_maxLen)
{
}

std::string StringCharacteristics::value(net::ConnectionInfo *sender)
{
	if (perUserQuery != nullptr && sender != nullptr)
		return "\"" + perUserQuery(sender) + "\"";
	return "\"" + _value + "\"";
}

void StringCharacteristics::setValue(std::string str, net::ConnectionInfo *sender)
{
	if (valueChangeFunctionCall != nullptr && sender != nullptr)
		valueChangeFunctionCall(_value, str, sender);
	_value = str;
}

std::string StringCharacteristics::describe(net::ConnectionInfo *sender)
{
	std::string result;
	char tempStr[4];

	if (premission & permission_read) {
		result += wrap("value") + ":" + value(sender).c_str();
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

	snprintf(tempStr, 4, "%X", type);
	result += wrap("type") + ":" + wrap(tempStr);
	result += ",";

	snprintf(tempStr, 4, "%hd", iid);
	result += wrap("iid") + ":" + tempStr;
	result += ",";

	if (maxLen > 0) {
		snprintf(tempStr, 4, "%hd", maxLen);
		result += wrap("maxLen") + ":" + tempStr;
		result += ",";
	}

	result += "\"format\":\"string\"";

	return "{" + result + "}";
}