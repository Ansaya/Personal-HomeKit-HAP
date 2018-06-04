#ifndef HAP_CHARACTERISTICS
#define HAP_CHARACTERISTICS

#include "CharType.h"
#include "net/ConnectionInfo.h"
#include "hapDefines.h"
#include "Permission.h"

#include <functional>
#include <string>


namespace hap {

class Characteristics {
public:
    Accessory *accessory;
    const unsigned int type;
    const int premission;
    int iid;
	std::function<std::string(net::ConnectionInfo* sender)> perUserQuery;

    Characteristics(char_type _type, permission _premission): type(_type), premission(_premission) {}
    
	virtual std::string value(net::ConnectionInfo *sender) = 0;
    
	void setValue(std::string str) {
        setValue(str, nullptr);
    }
    
	virtual void setValue(std::string str, net::ConnectionInfo *sender) = 0;
    
	virtual std::string describe(net::ConnectionInfo *sender) = 0;
    
	bool writable() { return premission & permission_write; }
    
	bool notifiable() { return premission & permission_notify; }
    
	void notify();
};

}

#endif // !HAP_CHARACTERISTICS
