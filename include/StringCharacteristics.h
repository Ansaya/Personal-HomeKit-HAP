#ifndef HAP_STRING_CHARACTERISTICS
#define HAP_STRING_CHARACTERISTICS

#include "Characteristics.h"

#include <functional>
#include <string>

namespace hap {

class StringCharacteristics: public Characteristics {
public:
    std::string _value;
    const unsigned short maxLen;
	std::function<void(std::string oldValue, std::string newValue, net::ConnectionInfo* sender)> valueChangeFunctionCall;

	StringCharacteristics(char_type _type, permission _premission, unsigned short _maxLen);

	virtual std::string value(net::ConnectionInfo *sender);

	virtual void setValue(std::string str, net::ConnectionInfo *sender);

	virtual std::string describe(net::ConnectionInfo *sender);
};

}

#endif