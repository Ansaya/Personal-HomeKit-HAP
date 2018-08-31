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
	 *	@param name Service name
	 *	@param manufacturer name of the company whose brand will appear on the accessory
	 *	@param model manufacturer-specific model of the accessory
	 *	@param serialNumber manufacturer-specific serial number of the accessory (length > 1)
	 *	@param firmwareRevision device firmware revision as x[.y[.z]] string
	 *	@param identifyCB accessory identification routine
	 *	@param hardwareRevision device hardware revision as x[.y[.z]] string
	 *	@param accessoryFalgs when set indicates accessory requires additional setup
	 */
	void addInfoService(
		const std::string& name, 
		const std::string& manufactuer, 
		const std::string& model, 
		const std::string& serialNumber, 
		const std::string& firmwareRevision,
		std::function<void(bool oldValue, bool newValue, void* sender)> identifyCB,
		const std::string& hardwareRevision = "",
		const bool accessoryFlags = false);

	/**
	 *	@brief Add a light bulb service to this accessory
	 *
	 *	@param onChar on/off value
	 *	@param brightnessChar perceived brightness value
	 *	@param hueChar hue or color value (0-360)
	 *	@param nameChar service name
	 *	@param saturationChar color saturation value
	 *	@param colorTemperatureChar color temperature represented in mirek scale
	 *
	 *	@return New light bulb service object
	 */
	Service_ptr addLightBulbService(
		BoolCharacteristics_ptr* onChar,
		IntCharacteristics_ptr* brightnessChar = nullptr,
		FloatCharacteristics_ptr* hueChar = nullptr,
		StringCharacteristics_ptr* nameChar = nullptr,
		FloatCharacteristics_ptr* saturationChar = nullptr,
		IntCharacteristics_ptr* colorTemperatureChar = nullptr);

	/**
	 *	@brief Add a binary switch service to this accessory
	 *
	 *	@param onChar on/off value
	 *	@param nameChar service name
	 *
	 *	@return New switch service object
	 */
	Service_ptr addSwitchService(
		BoolCharacteristics_ptr* onChar, 
		StringCharacteristics_ptr* nameChar = nullptr);

	/**
	 *	@brief Add an outlet service to this accessory
	 *
	 *	@param onChar on/off value
	 *	@param inUseChar outlet in use value
	 *	@param nameChar service name
	 *
	 *	@return New outlet service object
	 */
	Service_ptr addOutletService(
		BoolCharacteristics_ptr* onChar,
		BoolCharacteristics_ptr* inUseChar,
		StringCharacteristics_ptr* nameChar = nullptr);


	/**
	 *	@brief Add a stateless programmable switch service
	 *
	 *	@param eventChar event type (0-single press, 1-double press, 2-long press)
	 *	@param nameChar service name
	 *	@param labelIndexChar service label index value 
	 */
	Service_ptr addStatelessSwitchService(
		IntCharacteristics_ptr* eventChar,
		StringCharacteristics_ptr* nameChar = nullptr,
		IntCharacteristics_ptr* labelIndexChar = nullptr);

	/**
	 *	@brief Add a speaker service
	 *
	 *	@param muteChar mute on/off value
	 *	@param volumeChar volume percentage value
	 *	@param nameChar service name
	 *
	 *	@return New speaker service object
	 */
	Service_ptr addSpeakerService(
		BoolCharacteristics_ptr* muteChar, 
		IntCharacteristics_ptr* volumeChar = nullptr, 
		StringCharacteristics_ptr* nameChar = nullptr);

	/**
	 *	@brief Add a temperature sensor service
	 *
	 *	@param temperatureChar temperature value
	 *	@param nameChar serviceName
	 *	@param statusActiveChar activity status value (true means correctly working)
	 *	@param statusFaultChar fault status (1 means general fault)
	 *	@param statusLowBatteryChar battery status (1 means low battery)
	 *	@param statusTamperedChar tampered status (1 means accessory has been tampered with)
	 *
	 *	@return New temperature service
	 */
	Service_ptr addTemperatureSensorService(
		IntCharacteristics_ptr* temperatureChar,
		StringCharacteristics_ptr* nameChar = nullptr,
		BoolCharacteristics_ptr* statusActiveChar = nullptr,
		IntCharacteristics_ptr* statusFalutChar = nullptr,
		IntCharacteristics_ptr* statusLowBatteryChar = nullptr,
		IntCharacteristics_ptr* statusTamperedChar = nullptr);

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
