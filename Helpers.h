#ifndef HAP_HELPERS
#define HAP_HELPERS

#include <Unit.h>

#include <string>

namespace hap {

	//Wrap to JSON
	std::string wrap(const char *str);

	std::string arrayWrap(std::string *s, unsigned short len);

	std::string dictionaryWrap(std::string *key, std::string *value, unsigned short len);

}

#endif // !HAP_HELPERS