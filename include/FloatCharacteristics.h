#ifndef HAP_FLOAT_CHARACTERISTICS
#define HAP_FLOAT_CHARACTERISTICS

#include "Characteristics.h"
#include "Unit.h"
#include "net/ConnectionInfo.h"

#include <functional>
#include <string>

namespace hap {

class FloatCharacteristics: public Characteristics {
public:
    float _value;
    const float _minVal, _maxVal, _step;
    const unit _unit;
	std::function<void(float oldValue, float newValue, net::ConnectionInfo *sender)> valueChangeFunctionCall;

	FloatCharacteristics(char_type _type, permission _premission, float minVal, float maxVal, float step, unit charUnit);

	virtual std::string value(net::ConnectionInfo *sender);

	virtual void setValue(std::string str, net::ConnectionInfo *sender);

	virtual std::string describe(net::ConnectionInfo *sender);
};

}

#endif // !HAP_FLOAT_CHARACTERISTICS
