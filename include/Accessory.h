#ifndef HAP_ACCESSORY
#define HAP_ACCESSORY

#include "Characteristics.h"
#include "Service.h"

#include <atomic>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <vector>


namespace hap {

class Accessory {
public:
	Accessory();

	virtual ~Accessory();

	uint16_t getID() const;

	void addService(Service_ptr newService);

	void addInfoService(std::string name, std::string manufactuer, std::string model, std::string serialNumber,
		std::function<void(bool oldValue, bool newValue, void* sender)> identifyCB);

	bool removeService(Service_ptr service);

	bool removeCharacteristics(Characteristics_ptr characteristic);

	uint16_t servicesCount();

	Service_ptr getService(int id);

	Characteristics_ptr getCharacteristic(int id);
    
	std::string describe();

private:
	uint16_t _id;
	std::shared_ptr<std::atomic_ushort> _instancesID;
	std::mutex _servicesList;
	std::vector<Service_ptr>_services;

	friend class AccessorySet;
	friend class Service;

};

}

#endif // !HAP_ACCESSORY
