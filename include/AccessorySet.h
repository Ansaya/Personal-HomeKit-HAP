#ifndef HAP_ACCESSORY_SET
#define HAP_ACCESSORY_SET

#include "hapDefines.h"
#include "net/ConnectionInfo.h"

#include <atomic>
#include <cstdint>
#include <mutex>
#include <string>
#include <vector>

namespace hap {

class AccessorySet {
public:
	static AccessorySet& getInstance();

	uint16_t accessoriesCount();

	Accessory_ptr getAccessory(uint16_t id);

	void addAccessory(Accessory_ptr acc);

	bool removeAccessory(Accessory_ptr acc);

    std::string describe();

private:
	std::atomic_ushort _accessoryID;
	std::mutex _accessoriesList;
	std::vector<Accessory_ptr> _accessories;

	AccessorySet();
	AccessorySet(AccessorySet const&) = delete;
	void operator=(AccessorySet const&) = delete;
};

}

#endif // !HAP_ACCESSORY_SET
