#ifndef HAP_FLOAT_CHARACTERISTICS
#define HAP_FLOAT_CHARACTERISTICS

#include "Characteristics.h"
#include "Unit.h"

#include <functional>
#include <string>

namespace hap {

class FloatCharacteristics: public Characteristics {
public:
	FloatCharacteristics(
		char_type type, 
		permission permission, 
		double minVal = std::numeric_limits<double>::min(),
		double maxVal = std::numeric_limits<double>::max(),
		double step = 0.0, 
		unit charUnit = unit_none);

	std::string getValue() const override;

	void setValue(const std::string& newValue, void* sender = nullptr) override;

	void setValue(double newValue, void* sender = nullptr);

	std::string describe() const override;

	void setValueChangeCB(std::function<void(double oldValue, double newValue, void* sender)> cb = nullptr);

private:
	double _value;
	const double _minVal, _maxVal, _step;
	const unit _unit;
	std::function<void(double oldValue, double newValue, void* sender)> _valueChangeCB;
};

}

#endif // !HAP_FLOAT_CHARACTERISTICS
