#ifndef HAP_FLOAT_CHARACTERISTICS
#define HAP_FLOAT_CHARACTERISTICS

#include "Characteristics.h"
#include "Helpers.h"
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

    FloatCharacteristics(char_type _type, permission _premission, float minVal, float maxVal, float step, unit charUnit)
		: Characteristics(_type, _premission), _minVal(minVal), _maxVal(maxVal), _step(step), _unit(charUnit) {}

    virtual std::string value(net::ConnectionInfo *sender) {
        if (perUserQuery != nullptr)
            return perUserQuery(sender);

        return std::to_string(_value);
    }

    virtual void setValue(std::string str, net::ConnectionInfo *sender) {
        float temp = atof(str.c_str());
		if (valueChangeFunctionCall != nullptr && sender != nullptr)
			valueChangeFunctionCall(_value, temp, sender);
		_value = temp;
    }
	virtual std::string describe(net::ConnectionInfo *sender) {
		return attribute(type, iid, premission, value(sender), _minVal, _maxVal, _step, _unit);
	}
};

}

#endif // !HAP_FLOAT_CHARACTERISTICS
