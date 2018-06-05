#include "Helpers.h"

#include <Permission.h>

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