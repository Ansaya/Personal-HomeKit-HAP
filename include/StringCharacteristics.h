#ifndef HAP_STRING_CHARACTERISTICS
#define HAP_STRING_CHARACTERISTICS

#include "Characteristics.h"

#include <cstdint>
#include <functional>
#include <string>

namespace hap {

class StringCharacteristics: public Characteristics {
public:
	StringCharacteristics(char_type type, permission permission, uint16_t _maxLen = UINT16_MAX);

	std::string getValue() override;

	void setValue(const std::string& newValue, void* sender) override;

	std::string describe() override;

	void setValueChangeCB(std::function<void(const std::string& oldValue, const std::string& newValue, void* sender)> cb = nullptr);

private:
	std::string _value;
	const uint16_t _maxLen;
	std::function<void(const std::string& oldValue, const std::string& newValue, void* sender)> _valueChangeCB;
};

}

#endif