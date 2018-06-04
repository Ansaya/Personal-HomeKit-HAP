#include "Helpers.h"

#include "Permission.h"

using namespace hap;

std::string hap::wrap(const char *str) { 
	return (std::string)"\"" + str + "\""; 
}

std::string hap::arrayWrap(std::string *s, unsigned short len)
{
	std::string result;

	result += "[";

	for (int i = 0; i < len; i++) {
		result += s[i] + ",";
	}
	result = result.substr(0, result.size() - 1);

	result += "]";

	return result;
}

std::string hap::dictionaryWrap(std::string *key, std::string *value, unsigned short len)
{
	std::string result;

	result += "{";

	for (int i = 0; i < len; i++) {
		result += wrap(key[i].c_str()) + ":" + value[i] + ",";
	}
	result = result.substr(0, result.size() - 1);

	result += "}";

	return result;
}

//Value String
std::string hap::attribute(unsigned int type, unsigned short acclaim, int p, std::string value)
{
	std::string result;
	if (p & permission_read) {
		result += wrap("value") + ":";
		result += value;
		result += ",";
	}

	result += wrap("perms") + ":";
	result += "[";
	if (p & permission_read) result += wrap("pr") + ",";
	if (p & permission_write) result += wrap("pw") + ",";
	if (p & permission_notify) result += wrap("ev") + ",";
	result = result.substr(0, result.size() - 1);
	result += "]";
	result += ",";

	char tempStr[4];
	snprintf(tempStr, 4, "%X", type);
	result += wrap("type") + ":" + wrap(tempStr);
	result += ",";

	snprintf(tempStr, 4, "%hd", acclaim);
	result += wrap("iid") + ":" + tempStr;
	result += ",";

	result += "\"format\":\"bool\"";

	return "{" + result + "}";
}

std::string hap::attribute(unsigned int type, unsigned short acclaim, int p, std::string value, int minVal, int maxVal, int step, unit valueUnit)
{
	std::string result;
	char tempStr[16];

	if (p & permission_read) {
		result += wrap("value") + ":" + value;
		result += ",";
	}

	snprintf(tempStr, 16, "%d", minVal);
	if (minVal != INT32_MIN)
		result += wrap("minValue") + ":" + tempStr + ",";

	snprintf(tempStr, 16, "%d", maxVal);
	if (maxVal != INT32_MAX)
		result += wrap("maxValue") + ":" + tempStr + ",";

	snprintf(tempStr, 16, "%d", step);
	if (step > 0)
		result += wrap("minStep") + ":" + tempStr + ",";

	result += wrap("perms") + ":";
	result += "[";
	if (p & permission_read) result += wrap("pr") + ",";
	if (p & permission_write) result += wrap("pw") + ",";
	if (p & permission_notify) result += wrap("ev") + ",";
	result = result.substr(0, result.size() - 1);
	result += "]";
	result += ",";

	snprintf(tempStr, 16, "%X", type);
	result += wrap("type") + ":" + wrap(tempStr);
	result += ",";

	snprintf(tempStr, 16, "%hd", acclaim);
	result += wrap("iid") + ":" + tempStr;
	result += ",";

	switch (valueUnit) {
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

std::string hap::attribute(unsigned int type, unsigned short acclaim, int p, std::string value, float minVal, float maxVal, float step, unit valueUnit) {
	std::string result;
	char tempStr[16];

	if (p & permission_read) {
		result += wrap("value") + ":" + value;
		result += ",";
	}

	snprintf(tempStr, 16, "%f", minVal);
	if (minVal != INT32_MIN)
		result += wrap("minValue") + ":" + tempStr + ",";

	snprintf(tempStr, 16, "%f", maxVal);
	if (maxVal != INT32_MAX)
		result += wrap("maxValue") + ":" + tempStr + ",";

	snprintf(tempStr, 16, "%f", step);
	if (step > 0)
		result += wrap("minStep") + ":" + tempStr + ",";

	result += wrap("perms") + ":";
	result += "[";
	if (p & permission_read) result += wrap("pr") + ",";
	if (p & permission_write) result += wrap("pw") + ",";
	if (p & permission_notify) result += wrap("ev") + ",";
	result = result.substr(0, result.size() - 1);
	result += "]";
	result += ",";

	snprintf(tempStr, 16, "%X", type);
	result += wrap("type") + ":" + wrap(tempStr);
	result += ",";

	snprintf(tempStr, 16, "%hd", acclaim);
	result += wrap("iid") + ":" + tempStr;
	result += ",";

	switch (valueUnit) {
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

//Raw value
std::string hap::attribute(unsigned int type, unsigned short acclaim, int p, bool value)
{
	std::string result;
	if (p & permission_read) {
		result += wrap("value") + ":";
		if (value) result += "true";
		else result += "false";
		result += ",";
	}

	result += wrap("perms") + ":";
	result += "[";
	if (p & permission_read) result += wrap("pr") + ",";
	if (p & permission_write) result += wrap("pw") + ",";
	if (p & permission_notify) result += wrap("ev") + ",";
	result = result.substr(0, result.size() - 1);
	result += "]";
	result += ",";

	char tempStr[4];
	snprintf(tempStr, 4, "%X", type);
	result += wrap("type") + ":" + wrap(tempStr);
	result += ",";

	snprintf(tempStr, 4, "%hd", acclaim);
	result += wrap("iid") + ":" + tempStr;
	result += ",";

	result += "\"format\":\"bool\"";

	return "{" + result + "}";
}

std::string hap::attribute(unsigned int type, unsigned short acclaim, int p, int value, int minVal, int maxVal, int step, unit valueUnit)
{
	std::string result;
	char tempStr[16];

	snprintf(tempStr, 16, "%d", value);

	if (p & permission_read) {
		result += wrap("value") + ":" + tempStr;
		result += ",";
	}

	snprintf(tempStr, 16, "%d", minVal);
	if (minVal != INT32_MIN)
		result += wrap("minValue") + ":" + tempStr + ",";

	snprintf(tempStr, 16, "%d", maxVal);
	if (maxVal != INT32_MAX)
		result += wrap("maxValue") + ":" + tempStr + ",";

	snprintf(tempStr, 16, "%d", step);
	if (step > 0)
		result += wrap("minStep") + ":" + tempStr + ",";

	result += wrap("perms") + ":";
	result += "[";
	if (p & permission_read) result += wrap("pr") + ",";
	if (p & permission_write) result += wrap("pw") + ",";
	if (p & permission_notify) result += wrap("ev") + ",";
	result = result.substr(0, result.size() - 1);
	result += "]";
	result += ",";

	snprintf(tempStr, 16, "%X", type);
	result += wrap("type") + ":" + wrap(tempStr);
	result += ",";

	snprintf(tempStr, 16, "%hd", acclaim);
	result += wrap("iid") + ":" + tempStr;
	result += ",";

	switch (valueUnit) {
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

std::string hap::attribute(unsigned int type, unsigned short acclaim, int p, float value, float minVal, float maxVal, float step, unit valueUnit)
{
	std::string result;
	char tempStr[16];

	snprintf(tempStr, 16, "%f", value);

	if (p & permission_read) {
		result += wrap("value") + ":" + tempStr;
		result += ",";
	}

	snprintf(tempStr, 16, "%f", minVal);
	if (minVal != INT32_MIN)
		result += wrap("minValue") + ":" + tempStr + ",";

	snprintf(tempStr, 16, "%f", maxVal);
	if (maxVal != INT32_MAX)
		result += wrap("maxValue") + ":" + tempStr + ",";

	snprintf(tempStr, 16, "%f", step);
	if (step > 0)
		result += wrap("minStep") + ":" + tempStr + ",";

	result += wrap("perms") + ":";
	result += "[";
	if (p & permission_read) result += wrap("pr") + ",";
	if (p & permission_write) result += wrap("pw") + ",";
	if (p & permission_notify) result += wrap("ev") + ",";
	result = result.substr(0, result.size() - 1);
	result += "]";
	result += ",";

	snprintf(tempStr, 16, "%X", type);
	result += wrap("type") + ":" + wrap(tempStr);
	result += ",";

	snprintf(tempStr, 16, "%hd", acclaim);
	result += wrap("iid") + ":" + tempStr;
	result += ",";

	switch (valueUnit) {
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

std::string hap::attribute(unsigned int type, unsigned short acclaim, int p, std::string value, unsigned short len)
{
	std::string result;
	char tempStr[4];

	if (p & permission_read) {
		result += wrap("value") + ":" + value.c_str();
		result += ",";
	}

	result += wrap("perms") + ":";
	result += "[";
	if (p & permission_read) result += wrap("pr") + ",";
	if (p & permission_write) result += wrap("pw") + ",";
	if (p & permission_notify) result += wrap("ev") + ",";
	result = result.substr(0, result.size() - 1);
	result += "]";
	result += ",";

	snprintf(tempStr, 4, "%X", type);
	result += wrap("type") + ":" + wrap(tempStr);
	result += ",";

	snprintf(tempStr, 4, "%hd", acclaim);
	result += wrap("iid") + ":" + tempStr;
	result += ",";

	if (len > 0) {
		snprintf(tempStr, 4, "%hd", len);
		result += wrap("maxLen") + ":" + tempStr;
		result += ",";
	}

	result += "\"format\":\"string\"";

	return "{" + result + "}";
}