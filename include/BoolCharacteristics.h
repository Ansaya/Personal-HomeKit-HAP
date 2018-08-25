#ifndef HAP_BOOL_CHARACTERISTICS
#define HAP_BOOL_CHARACTERISTICS

#include "Characteristics.h"

#include <functional>
#include <string>

namespace hap {

class BoolCharacteristics: public Characteristics {
public:
	BoolCharacteristics(char_type _type, permission _premission);

	std::string getValue() const override;

	void setValue(const std::string& newValue, void* sender) override;

	void setValue(bool newValue, void* sender = nullptr);

	std::string describe() const override;

	void setValueChangeCB(std::function<void(bool oldValue, bool newValue, void* sender)> cb = nullptr);

private:
	bool _value;
	std::function<void(bool oldValue, bool newValue, void* sender)> _valueChangeCB;
};

}

#endif // !HAP_BOOL_CHARACTERISTICS
