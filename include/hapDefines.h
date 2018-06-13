#ifndef HAP_DEFINES
#define HAP_DEFINES

#include <memory>

namespace hap {

	class AccessorySet;

	class Accessory;
	typedef std::shared_ptr<Accessory> Accessory_ptr;

	class Service;
	typedef std::shared_ptr<Service> Service_ptr;

	class Characteristics;
	typedef std::shared_ptr<Characteristics> Characteristics_ptr;

}

#endif