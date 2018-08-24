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

	/**
	 *	@brief Accessory information service for base device attributes, each accessory should have one
	 *
	 *	@param name useless
	 *	@param manufacturer accessory manufacturer name
	 *	@param model accessory model name
	 *	@param serialNumber device serial number
	 *	@param identifyCB function called on device identification request
	 */
	void addInfoService(std::string name, std::string manufactuer, std::string model, std::string serialNumber,
		std::function<void(bool oldValue, bool newValue, void* sender)> identifyCB);

	/**
	 *	@brief Add a light bulb service to this accessory
	 *
	 *	@note Pointer references will be initialized during the function call
	 *
	 *	@param nameChar reference to the service name
	 *	@param onChar reference to the service on/off value
	 *
	 *	@return New light bulb service object
	 */
	Service_ptr addLightBulbService(StringCharacteristics_ptr& nameChar, BoolCharacteristics_ptr& onChar);

	/**
	 *	@brief Add a light bulb service to this accessory with dimmer capabilities
	 *
	 *	@note Pointer references will be initialized during the function call
	 *
	 *	@param nameChar reference to the service name
	 *	@param onChar reference to the service on/off value
	 *	@param brightChar reference to the service brightness value
	 *
	 *	@return New light bulb service object with dimmer control
	 */
	Service_ptr addLightBulbService(StringCharacteristics_ptr& nameChar, BoolCharacteristics_ptr& onChar, IntCharacteristics_ptr& brightChar);

	/**
	 *	@brief Add a switch service to this accessory
	 *
	 *	@note Pointer references will be initialized during the function call
	 *
	 *	@param nameChar reference to the service name
	 *	@param onChar reference to the service on/off value
	 *
	 *	@return New switch service object
	 */
	Service_ptr addSwithService(StringCharacteristics_ptr& nameChar, BoolCharacteristics_ptr& onChar);

	/**
	 *	@brief Add given service to this accessory
	 *
	 *	@param newService service to add to this accessory
	 */
	void addService(Service_ptr newService);

	uint16_t servicesCount();

	Service_ptr getService(int id);

	/**
	 *	@brief Remove given service from this accessory
	 *
	 *	@param service service to be removed from this accessory
	 *
	 *	@return True if service was found and removed, false if service is not present
	 */
	bool removeService(Service_ptr service);

	Characteristics_ptr getCharacteristic(int id);

	bool removeCharacteristics(Characteristics_ptr characteristic);
    
	std::string describe();

private:
	uint16_t _id;
	std::shared_ptr<std::atomic_ushort> _instancesID;
	std::mutex _servicesList;
	std::vector<Service_ptr> _services;

	friend class AccessorySet;
	friend class Service;

};

}

#endif // !HAP_ACCESSORY
