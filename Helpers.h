#ifndef HAP_HELPERS
#define HAP_HELPERS

#include "Unit.h"

#include <string>

namespace hap {

	int is_big_endian();

	//Wrap to JSON
	std::string wrap(const char *str);

	std::string arrayWrap(std::string *s, unsigned short len);

	std::string dictionaryWrap(std::string *key, std::string *value, unsigned short len);

	std::string attribute(unsigned int type, unsigned short acclaim, int p, std::string value);

	std::string attribute(unsigned int type, unsigned short acclaim, int p, std::string value, int minVal, int maxVal, int step, unit valueUnit);

	std::string attribute(unsigned int type, unsigned short acclaim, int p, std::string value, float minVal, float maxVal, float step, unit valueUnit);

	std::string attribute(unsigned int type, unsigned short acclaim, int p, bool value);

	std::string attribute(unsigned int type, unsigned short acclaim, int p, int value, int minVal, int maxVal, int step, unit valueUnit);

	std::string attribute(unsigned int type, unsigned short acclaim, int p, float value, float minVal, float maxVal, float step, unit valueUnit);

	std::string attribute(unsigned int type, unsigned short acclaim, int p, std::string value, unsigned short len);

}

#endif // !HAP_HELPERS