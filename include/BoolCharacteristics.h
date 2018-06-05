#ifndef HAP_BOOL_CHARACTERISTICS
#define HAP_BOOL_CHARACTERISTICS

#include "Characteristics.h"

#include <functional>
#include <string>

namespace hap {

class BoolCharacteristics: public Characteristics {
public:
    bool _value;
	std::function<void(bool oldValue, bool newValue, net::ConnectionInfo* sender)> valueChangeFunctionCall;

	BoolCharacteristics(char_type _type, permission _premission);

	virtual std::string value(net::ConnectionInfo *sender);

	virtual void setValue(std::string str, net::ConnectionInfo *sender);

	virtual std::string describe(net::ConnectionInfo *sender);
};

}

#endif // !HAP_BOOL_CHARACTERISTICS
