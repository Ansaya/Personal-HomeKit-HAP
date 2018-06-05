#ifndef HAP_DEFINES
#define HAP_DEFINES

#include <memory>

namespace hap {

	class Accessory;
	typedef std::shared_ptr<Accessory> Accessory_ptr;

	class AccessorySet;
	typedef std::shared_ptr<AccessorySet> AccessorySet_ptr;

}

#endif