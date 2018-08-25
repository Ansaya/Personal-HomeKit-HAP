#ifndef HAP_INT_CHARACTERISTICS
#define HAP_INT_CHARACTERISTICS

#include "Characteristics.h"
#include "Unit.h"

#include <functional>
#include <string>

namespace hap {

class IntCharacteristics: public Characteristics {
public:
	IntCharacteristics(
		char_type _type, 
		permission _premission, 
		int minVal = std::numeric_limits<int>::min(),
		int maxVal = std::numeric_limits<int>::max(),
		int step = 0, 
		unit charUnit = unit_none);

	std::string getValue() const override;

	void setValue(const std::string& newValue, void* sender) override;

	void setValue(int newValue, void* sender = nullptr);

	std::string describe() const override;

	void setValueChangeCB(std::function<void(int oldValue, int newValue, void* sender)> cb = nullptr);

private:
	int _value;
	const int _minVal, _maxVal, _step;
	const unit _unit;
	std::function<void(int oldValue, int newValue, void* sender)> _valueChangeCB;
};

}

#endif