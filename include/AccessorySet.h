#ifndef HAP_ACCESSORY_SET
#define HAP_ACCESSORY_SET

#include "Accessory.h"
#include "net/ConnectionInfo.h"

#include <mutex>
#include <string>
#include <vector>

namespace hap {

class AccessorySet {
private:
    std::vector<Accessory_ptr> _accessories;
    int _aid = 0;

	AccessorySet();
    AccessorySet(AccessorySet const&) = delete;
    void operator=(AccessorySet const&) = delete;

public:
	static AccessorySet& getInstance();

    std::mutex accessoryMutex;

	short numberOfAccessory();

	Accessory_ptr accessoryAtIndex(int index);

	void addAccessory(Accessory_ptr acc);

	bool removeAccessory(Accessory_ptr acc);

    std::string describe(net::ConnectionInfo *sender);
};

}

#endif // !HAP_ACCESSORY_SET
