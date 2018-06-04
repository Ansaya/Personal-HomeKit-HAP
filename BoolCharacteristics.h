#ifndef HAP_BOOL_CHARACTERISTICS
#define HAP_BOOL_CHARACTERISTICS

#include "Characteristics.h"
#include "Helpers.h"

#include <functional>
#include <string>

namespace hap {

class BoolCharacteristics: public Characteristics {
public:
    bool _value;
	std::function<void(bool oldValue, bool newValue, net::ConnectionInfo* sender)> valueChangeFunctionCall;

    BoolCharacteristics(char_type _type, permission _premission) : Characteristics(_type, _premission) {}

    virtual std::string value(net::ConnectionInfo *sender) {
        if (perUserQuery != nullptr)
            return perUserQuery(sender);
        if (_value)
            return "1";
        return "0";
    }

    virtual void setValue(std::string str, net::ConnectionInfo *sender) {
        bool newValue = ("true" == str || "1" == str);
        if (valueChangeFunctionCall != nullptr && sender != nullptr)
            valueChangeFunctionCall(_value, newValue, sender);
        _value = newValue;
    }

	virtual std::string describe(net::ConnectionInfo *sender) {
		return attribute(type, iid, premission, value(sender));
	}
};

}

#endif // !HAP_BOOL_CHARACTERISTICS
