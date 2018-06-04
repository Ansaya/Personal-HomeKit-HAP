#ifndef HAP_INT_CHARACTERISTICS
#define HAP_INT_CHARACTERISTICS

#include "Characteristics.h"
#include "Helpers.h"
#include "net/ConnectionInfo.h"
#include "Unit.h"

#include <functional>
#include <string>

namespace hap {

class IntCharacteristics: public Characteristics {
public:
    int _value;
    const int _minVal, _maxVal, _step;
    const unit _unit;
	std::function<void(int oldValue, int newValue, net::ConnectionInfo *sender)> valueChangeFunctionCall;

    IntCharacteristics(char_type _type, permission _premission, int minVal, int maxVal, int step, unit charUnit)
		: Characteristics(_type, _premission), _minVal(minVal), _maxVal(maxVal), _step(step), _unit(charUnit) 
	{
        _value = minVal;
    }

    virtual std::string value(net::ConnectionInfo *sender) {
        if (perUserQuery != nullptr)
            return perUserQuery(sender);

        return std::to_string((int)_value);
    }

    virtual void setValue(std::string str, net::ConnectionInfo *sender) {
        float temp = atoi(str.c_str());
		if (valueChangeFunctionCall != nullptr && sender != nullptr)
			valueChangeFunctionCall(_value, temp, sender);
		_value = temp;
    }

	virtual std::string describe(net::ConnectionInfo *sender) {
		return attribute(type, iid, premission, value(sender), _minVal, _maxVal, _step, _unit);
	}
};

}

#endif