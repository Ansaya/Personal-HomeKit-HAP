#ifndef HAP_INT_CHARACTERISTICS
#define HAP_INT_CHARACTERISTICS

#include "Characteristics.h"
#include "Unit.h"
#include "net/ConnectionInfo.h"

#include <functional>
#include <string>

namespace hap {

class IntCharacteristics: public Characteristics {
public:
    int _value;
    const int _minVal, _maxVal, _step;
    const unit _unit;
	std::function<void(int oldValue, int newValue, net::ConnectionInfo *sender)> valueChangeFunctionCall;

	IntCharacteristics(char_type _type, permission _premission, int minVal, int maxVal, int step, unit charUnit);

	virtual std::string value(net::ConnectionInfo *sender);

	virtual void setValue(std::string str, net::ConnectionInfo *sender);

	virtual std::string describe(net::ConnectionInfo *sender);
};

}

#endif