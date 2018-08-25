#ifndef HAP_CHARACTERISTICS
#define HAP_CHARACTERISTICS

#include "CharType.h"
#include "hapDefines.h"
#include "Permission.h"

#include <cstdint>
#include <mutex>
#include <string>


namespace hap {

class Characteristics {
public:
	Characteristics(char_type type, permission premission);

	uint16_t getID() const;

	uint16_t getAID() const;
    
	virtual std::string getValue() const = 0;
    
	virtual void setValue(const std::string& newValue, void* sender = nullptr) = 0;
    
	virtual std::string describe() const = 0;
    
	bool writable() const;
    
	bool notifiable() const;
    
	void notify();

protected:
	const char_type _type;
	const permission _permission;
	mutable std::mutex _valueHandle;

private:
	uint16_t _id;
	uint16_t _aid;

	friend class AccessorySet;
	friend class Accessory;
	friend class Service;

};

}

#endif // !HAP_CHARACTERISTICS
