#ifndef HAP_FLOAT_CHARACTERISTICS
#define HAP_FLOAT_CHARACTERISTICS

#include "Characteristics.h"
#include "Unit.h"

#include <functional>
#include <string>

namespace hap {

class FloatCharacteristics: public Characteristics {
public:
	FloatCharacteristics(char_type type, permission permission, 
		float minVal, float maxVal, float step, unit charUnit);

	std::string getValue() override;

	void setValue(const std::string& newValue, void* sender) override;

	std::string describe() override;

	void setValueChangeCB(std::function<void(float oldValue, float newValue, void* sender)> cb = nullptr);

private:
	float _value;
	const float _minVal, _maxVal, _step;
	const unit _unit;
	std::function<void(float oldValue, float newValue, void* sender)> _valueChangeCB;
};

}

#endif // !HAP_FLOAT_CHARACTERISTICS
