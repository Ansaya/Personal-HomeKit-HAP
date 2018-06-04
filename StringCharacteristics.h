#ifndef HAP_STRING_CHARACTERISTICS
#define HAP_STRING_CHARACTERISTICS

#include "Characteristics.h"
#include "Helpers.h"
#include "net/ConnectionInfo.h"

#include <functional>
#include <string>

namespace hap {

class StringCharacteristics: public Characteristics {
public:
    std::string _value;
    const unsigned short maxLen;
	std::function<void(std::string oldValue, std::string newValue, net::ConnectionInfo* sender)> valueChangeFunctionCall;

    StringCharacteristics(char_type _type, permission _premission, unsigned short _maxLen)
		: Characteristics(_type, _premission), maxLen(_maxLen) {}

    virtual std::string value(net::ConnectionInfo *sender) {
		if (perUserQuery != nullptr)
			return "\"" + perUserQuery(sender) + "\"";
		return "\"" + _value + "\"";
    }

    virtual void setValue(std::string str, net::ConnectionInfo *sender) {
        if (valueChangeFunctionCall != nullptr && sender != nullptr)
            valueChangeFunctionCall(_value, str, sender);
        _value = str;
    }

	virtual std::string describe(net::ConnectionInfo *sender) {
		return attribute(type, iid, premission, value(sender), maxLen);
	}
};

}

#endif