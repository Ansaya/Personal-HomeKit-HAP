#include <Accessory.h>

#include <BoolCharacteristics.h>
#include <Characteristics.h>
#include <CharType.h>
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

void Accessory::addService(Service_ptr newService)
{
	newService->_id = _instancesID->fetch_add(1);
	newService->_aid = _id;
	
	std::unique_lock<std::mutex> serviceLock(newService->_characteristicsList);

	for (auto& it : newService->_characteristics) {
		it->_id = _instancesID->fetch_add(1);
		it->_aid = _id;
	}

	newService->_parentInstancesID = _instancesID;

	std::unique_lock<std::mutex> lock(_servicesList);

	_services.push_back(newService);
}

void Accessory::addInfoService(std::string name, std::string manufactuer, std::string model, std::string serialNumber,
	std::function<void(bool oldValue, bool newValue, void* sender)> identifyCB)
{
	Service_ptr infoService = std::make_shared<Service>(service_accessoryInfo);
	addService(infoService);

	StringCharacteristics *serviceNameChar = new StringCharacteristics(char_serviceName, permission_read);
	serviceNameChar->Characteristics::setValue(name);

	StringCharacteristics *manufacturerNameChar = new StringCharacteristics(char_manufactuer, permission_read);
	manufacturerNameChar->Characteristics::setValue(manufactuer);

	StringCharacteristics *modelNameChar = new StringCharacteristics(char_modelName, permission_read);
	modelNameChar->Characteristics::setValue(model);

	StringCharacteristics *serialNumberChar = new StringCharacteristics(char_serialNumber, permission_read);
	serialNumberChar->Characteristics::setValue(serialNumber);

	BoolCharacteristics *identifyChar = new BoolCharacteristics(char_identify, permission_write);
	identifyChar->Characteristics::setValue("false");
	identifyChar->setValueChangeCB(identifyCB);

	infoService->addCharacteristic(std::shared_ptr<Characteristics>(serviceNameChar));
	infoService->addCharacteristic(std::shared_ptr<Characteristics>(manufacturerNameChar));
	infoService->addCharacteristic(std::shared_ptr<Characteristics>(modelNameChar));
	infoService->addCharacteristic(std::shared_ptr<Characteristics>(serialNumberChar));
	infoService->addCharacteristic(std::shared_ptr<Characteristics>(identifyChar));
}

bool Accessory::removeService(Service_ptr ser)
{
	std::unique_lock<std::mutex> lock(_servicesList);

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
	std::unique_lock<std::mutex> lock(_servicesList);

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
	std::unique_lock<std::mutex> lock(_servicesList);

	auto s = std::find_if(_services.begin(), _services.end(), 
		[id](const Service_ptr sp) { return sp->getID() == id; });

	if (s != _services.end())
		return *s;

	return nullptr;
}

Characteristics_ptr Accessory::getCharacteristic(int id)
{
	std::unique_lock<std::mutex> lock(_servicesList);

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

	{
		keys[0] = "aid";
		char temp[8];
		sprintf(temp, "%d", _id);
		values[0] = temp;
	}

	{
		std::unique_lock<std::mutex> lock(_servicesList);

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