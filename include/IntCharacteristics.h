#ifndef HAP_INT_CHARACTERISTICS
#define HAP_INT_CHARACTERISTICS

#include "Characteristics.h"
#include "Unit.h"

#include <functional>
#include <string>

namespace hap {

class IntCharacteristics: public Characteristics {
public:
	IntCharacteristics(char_type _type, permission _premission, int minVal, int maxVal, int step, unit charUnit);

	std::string getValue() override;

	void setValue(const std::string& newValue, void* sender) override;

	std::string describe() override;

	void setValueChangeCB(std::function<void(int oldValue, int newValue, void* sender)> cb = nullptr);

private:
	int _value;
	const int _minVal, _maxVal, _step;
	const unit _unit;
	std::function<void(int oldValue, int newValue, void* sender)> _valueChangeCB;
};

}

#endif