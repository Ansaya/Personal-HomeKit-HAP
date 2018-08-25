#include <Accessory.h>
#include <../Configuration.h>

#include <BoolCharacteristics.h>
#include <Characteristics.h>
#include <CharType.h>
#include <FloatCharacteristics.h>
#include <IntCharacteristics.h>
#include <Service.h>
#include <ServiceType.h>
#include <StringCharacteristics.h>

#include "../Helpers.h"

#include <algorithm>

using namespace hap;

Accessory::Accessory() : _instancesID(std::make_shared<std::atomic_ushort>(1))
{
}

Accessory::~Accessory()
{
}

uint16_t Accessory::getID() const
{
	return _id;
}

Service_ptr Accessory::addLightBulbService(
	BoolCharacteristics_ptr* onChar,
	IntCharacteristics_ptr* brightnessChar,
	FloatCharacteristics_ptr* hueChar,
	StringCharacteristics_ptr* nameChar,
	FloatCharacteristics_ptr* saturationChar,
	IntCharacteristics_ptr* colorTemperatureChar)
{
	hap::Service_ptr lightBulbService = std::make_shared<Service>(service_lightBulb);
	addService(lightBulbService);

	*onChar = std::make_shared<BoolCharacteristics>(char_on, permission_all);
	lightBulbService->addCharacteristic(*onChar);

	if (brightnessChar != nullptr) {
		*brightnessChar = std::make_shared<IntCharacteristics>(char_brightness, permission_all, 0, 100, 1, unit_percentage);
		lightBulbService->addCharacteristic(*brightnessChar);
	}

	if (hueChar != nullptr) {
		*hueChar = std::make_shared<FloatCharacteristics>(char_hue, permission_all, 0, 360, 1, unit_arcDegree);
		lightBulbService->addCharacteristic(*hueChar);
	}

	if (nameChar != nullptr) {
		*nameChar = std::make_shared<StringCharacteristics>(char_serviceName, permission_read);
		lightBulbService->addCharacteristic(*nameChar);
	}

	if (saturationChar != nullptr) {
		*saturationChar = std::make_shared<FloatCharacteristics>(char_saturation, permission_all, 0, 100, 1, unit_percentage);
		lightBulbService->addCharacteristic(*saturationChar);
	}

	if (colorTemperatureChar != nullptr) {
		*colorTemperatureChar = std::make_shared<IntCharacteristics>(char_colorTemperature, permission_all, 50, 400, 1, unit_percentage);
		lightBulbService->addCharacteristic(*colorTemperatureChar);
	}

	return lightBulbService;
}

Service_ptr Accessory::addSwithService(
	BoolCharacteristics_ptr* onChar,
	StringCharacteristics_ptr* nameChar)
{
	hap::Service_ptr switchService = std::make_shared<Service>(service_switch);
	addService(switchService);

	*onChar = std::make_shared<BoolCharacteristics>(char_on, permission_all);
	switchService->addCharacteristic(*onChar);

	if (nameChar != nullptr) {
		*nameChar = std::make_shared<StringCharacteristics>(char_serviceName, permission_read);
		switchService->addCharacteristic(*nameChar);
	}

	return switchService;
}

Service_ptr Accessory::addOutletService(
	BoolCharacteristics_ptr* onChar,
	BoolCharacteristics_ptr* inUseChar,
	StringCharacteristics_ptr* nameChar)
{
	hap::Service_ptr outletService = std::make_shared<Service>(service_outlet);
	addService(outletService);

	*onChar = std::make_shared<BoolCharacteristics>(char_on, permission_all);
	outletService->addCharacteristic(*onChar);

	*inUseChar = std::make_shared<BoolCharacteristics>(char_outletInUse, permission_read_notify);
	outletService->addCharacteristic(*inUseChar);

	if (nameChar != nullptr) {
		*nameChar = std::make_shared<StringCharacteristics>(char_serviceName, permission_read);
		outletService->addCharacteristic(*nameChar);
	}

	return outletService;
}

Service_ptr Accessory::addStatelessSwithService(
	IntCharacteristics_ptr* eventChar,
	StringCharacteristics_ptr* nameChar,
	IntCharacteristics_ptr* labelIndexChar)
{
	hap::Service_ptr switchService = std::make_shared<Service>(service_programmableSwitch_stateless);
	addService(switchService);

	*eventChar = std::make_shared<IntCharacteristics>(char_programmableSwitchEvent, permission_read_notify, 0, 2, 1, unit_none);
	switchService->addCharacteristic(*eventChar);

	if (nameChar != nullptr) {
		*nameChar = std::make_shared<StringCharacteristics>(char_serviceName, permission_read);
		switchService->addCharacteristic(*nameChar);
	}

	if (labelIndexChar != nullptr) {
		*labelIndexChar = std::make_shared<IntCharacteristics>(char_serviceLabelIndex, permission_read, 0, std::numeric_limits<uint8_t>::max(), 1, unit_none);
		switchService->addCharacteristic(*labelIndexChar);
	}

	return switchService;
}

Service_ptr Accessory::addSpeakerService(
	BoolCharacteristics_ptr* muteChar,
	IntCharacteristics_ptr* volumeChar,
	StringCharacteristics_ptr* nameChar)
{
	hap::Service_ptr speakerService = std::make_shared<Service>(service_speaker);
	addService(speakerService);

	*muteChar = std::make_shared<BoolCharacteristics>(char_mute, permission_all);
	speakerService->addCharacteristic(*muteChar);

	if (volumeChar != nullptr) {
		*volumeChar = std::make_shared<IntCharacteristics>(char_volume, permission_all, 0, 100, 1, unit_percentage);
		speakerService->addCharacteristic(*volumeChar);
	}

	if (nameChar != nullptr) {
		*nameChar = std::make_shared<StringCharacteristics>(char_serviceName, permission_read);
		speakerService->addCharacteristic(*nameChar);
	}

	return speakerService;
}

Service_ptr Accessory::addTemperatureSensorService(
	IntCharacteristics_ptr* temperatureChar,
	StringCharacteristics_ptr* nameChar,
	BoolCharacteristics_ptr* statusActiveChar,
	IntCharacteristics_ptr* statusFalutChar,
	IntCharacteristics_ptr* statusLowBatteryChar,
	IntCharacteristics_ptr* statusTamperedChar)
{
	hap::Service_ptr temperatureService = std::make_shared<Service>(service_temperatureSensor);
	addService(temperatureService);

	*temperatureChar = std::make_shared<IntCharacteristics>(char_currentTemperature, permission_read_notify, 0, 100, 1, unit_celsius);
	temperatureService->addCharacteristic(*temperatureChar);

	if (nameChar != nullptr) {
		*nameChar = std::make_shared<StringCharacteristics>(char_serviceName, permission_read);
		temperatureService->addCharacteristic(*nameChar);
	}

	if (statusActiveChar != nullptr) {
		*statusActiveChar = std::make_shared<BoolCharacteristics>(char_sensorActive, permission_read_notify);
		temperatureService->addCharacteristic(*statusActiveChar);
	}

	if (statusFalutChar != nullptr) {
		*statusFalutChar = std::make_shared<IntCharacteristics>(char_sensorFault, permission_read_notify, 0, 1, 1, unit_none);
		temperatureService->addCharacteristic(*statusFalutChar);
	}

	if (statusLowBatteryChar != nullptr) {
		*statusLowBatteryChar = std::make_shared<IntCharacteristics>(char_sensorLowBattery, permission_read_notify, 0, 1, 1, unit_none);
		temperatureService->addCharacteristic(*statusLowBatteryChar);
	}

	if (statusTamperedChar != nullptr) {
		*statusTamperedChar = std::make_shared<IntCharacteristics>(char_sensorTampered, permission_read_notify, 0, 1, 1, unit_none);
		temperatureService->addCharacteristic(*statusTamperedChar);
	}

	return temperatureService;
}

void Accessory::addService(Service_ptr newService)
{
	newService->_id = _instancesID->fetch_add(1);
	newService->_aid = _id;
	
#ifdef HAP_THREAD_SAFE
	std::unique_lock<std::mutex> serviceLock(newService->_characteristicsList);
#endif

	for (auto& it : newService->_characteristics) {
		it->_id = _instancesID->fetch_add(1);
		it->_aid = _id;
	}

	newService->_parentInstancesID = _instancesID;

#ifdef HAP_THREAD_SAFE
	std::unique_lock<std::mutex> lock(_servicesList);
#endif

	_services.push_back(newService);
}

void Accessory::addInfoService(
	const std::string& name,
	const std::string& manufactuer,
	const std::string& model,
	const std::string& serialNumber,
	const std::string& firmwareRevision,
	std::function<void(bool oldValue, bool newValue, void* sender)> identifyCB,
	const std::string& hardwareRevision,
	const bool accessoryFlags)
{
	Service_ptr infoService = std::make_shared<Service>(service_accessoryInfo);
	addService(infoService);

	BoolCharacteristics_ptr identifyChar = std::make_shared<BoolCharacteristics>(char_identify, permission_write);
	if (identifyCB != nullptr) {
		identifyChar->setValueChangeCB(identifyCB);
	}
	infoService->addCharacteristic(identifyChar);

	StringCharacteristics_ptr manufacturerNameChar = std::make_shared<StringCharacteristics>(char_manufactuer, permission_read);
	manufacturerNameChar->setValue(manufactuer);
	infoService->addCharacteristic(manufacturerNameChar);

	StringCharacteristics_ptr modelNameChar = std::make_shared<StringCharacteristics>(char_modelName, permission_read);
	modelNameChar->setValue(model);
	infoService->addCharacteristic(modelNameChar);

	StringCharacteristics_ptr serviceNameChar = std::make_shared<StringCharacteristics>(char_serviceName, permission_read);
	serviceNameChar->setValue(name);
	infoService->addCharacteristic(serviceNameChar);

	StringCharacteristics_ptr serialNumberChar = std::make_shared<StringCharacteristics>(char_serialNumber, permission_read);
	serialNumberChar->setValue(serialNumber);
	infoService->addCharacteristic(serialNumberChar);

	StringCharacteristics_ptr firmwareRevisionChar = std::make_shared<StringCharacteristics>(char_firmwareRevision, permission_read);
	firmwareRevisionChar->setValue(firmwareRevision);
	infoService->addCharacteristic(firmwareRevisionChar);

	if (!hardwareRevision.empty()) {
		StringCharacteristics_ptr hardwareRevisionChar = std::make_shared<StringCharacteristics>(char_hardwareRevision, permission_read);
		hardwareRevisionChar->setValue(hardwareRevision);
		infoService->addCharacteristic(hardwareRevisionChar);
	}

	if (accessoryFlags) {
		IntCharacteristics_ptr accessoryFlagsChar = std::make_shared<IntCharacteristics>(char_accessoryFlags, permission_read_notify);
		accessoryFlagsChar->setValue(1);
		infoService->addCharacteristic(accessoryFlagsChar);
	}
}

bool Accessory::removeService(Service_ptr ser)
{
#ifdef HAP_THREAD_SAFE
	std::unique_lock<std::mutex> lock(_servicesList);
#endif

	for (auto it = _services.begin(); it != _services.end(); ++it) {
		if (*it == ser) {
			_services.erase(it);
			return true;
		}
	}

	return false;
}

bool Accessory::removeCharacteristics(Characteristics_ptr characteristic)
{
#ifdef HAP_THREAD_SAFE
	std::unique_lock<std::mutex> lock(_servicesList);
#endif

	for (auto& it : _services) {
		if (it->removeCharacteristic(characteristic))
			return true;
	}

	return false;
}

uint16_t Accessory::servicesCount()
{ 
	return _services.size();
}

Service_ptr Accessory::getService(int id)
{
#ifdef HAP_THREAD_SAFE
	std::unique_lock<std::mutex> lock(_servicesList);
#endif

	auto s = std::find_if(_services.begin(), _services.end(), 
		[id](const Service_ptr sp) { return sp->getID() == id; });

	if (s != _services.end())
		return *s;

	return nullptr;
}

Characteristics_ptr Accessory::getCharacteristic(int id)
{
#ifdef HAP_THREAD_SAFE
	std::unique_lock<std::mutex> lock(_servicesList);
#endif

	for (auto& it : _services) {
		Characteristics_ptr c = it->getCharacteristic(id);
		if (c != nullptr)
			return c;
	}

	return nullptr;
}

std::string Accessory::describe()
{
	std::string keys[2];
	std::string values[2];

	keys[0] = "aid";
	values[0] = std::to_string(_id);

	{
#ifdef HAP_THREAD_SAFE
		std::unique_lock<std::mutex> lock(_servicesList);
#endif

		//Form services list
		int noOfService = servicesCount();
		std::string *services = new std::string[noOfService];
		for (int i = 0; i < noOfService; i++) {
			services[i] = _services[i]->describe();
		}
		keys[1] = "services";
		values[1] = arrayWrap(services, noOfService);
		delete[] services;
	}

	std::string result = dictionaryWrap(keys, values, 2);

	return result;
}