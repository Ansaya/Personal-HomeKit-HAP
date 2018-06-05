#include <Accessory.h>

#include <BoolCharacteristics.h>
#include <CharType.h>
#include <ServiceType.h>
#include <StringCharacteristics.h>

#include "../Helpers.h"

using namespace hap;

void Accessory::addService(Service *ser) {
	ser->serviceID = ++numberOfInstance;
	_services.push_back(ser);
}

void Accessory::addInfoService(std::string accName, std::string manufactuerName, std::string modelName, std::string serialNumber,
	std::function<void(bool oldValue, bool newValue, net::ConnectionInfo *sender)> identifyCallback)
{
	Service *infoService = new Service(service_accessoryInfo);
	addService(infoService);

	StringCharacteristics *accNameCha = new StringCharacteristics(char_serviceName, permission_read, 0);
	accNameCha->Characteristics::setValue(accName);
	addCharacteristics(infoService, accNameCha);

	StringCharacteristics *manNameCha = new StringCharacteristics(char_manufactuer, permission_read, 0);
	manNameCha->Characteristics::setValue(manufactuerName);
	addCharacteristics(infoService, manNameCha);

	StringCharacteristics *modelNameCha = new StringCharacteristics(char_modelName, permission_read, 0);
	modelNameCha->Characteristics::setValue(modelName);
	addCharacteristics(infoService, modelNameCha);

	StringCharacteristics *serialNameCha = new StringCharacteristics(char_serialNumber, permission_read, 0);
	serialNameCha->Characteristics::setValue(serialNumber);
	addCharacteristics(infoService, serialNameCha);

	BoolCharacteristics *identify = new BoolCharacteristics(char_identify, permission_write);
	identify->Characteristics::setValue("false");
	identify->valueChangeFunctionCall = identifyCallback;
	addCharacteristics(infoService, identify);
}

void Accessory::addCharacteristics(Service *ser, Characteristics *cha) {
	cha->iid = ++numberOfInstance;
	cha->accessory = this;
	ser->_characteristics.push_back(cha);
}

bool Accessory::removeService(Service *ser) {
	bool exist = false;
	for (auto it = _services.begin(); it != _services.end(); it++) {
		if (*it == ser) {
			_services.erase(it);
			exist = true;
		}
	}
	return exist;
}

bool Accessory::removeCharacteristics(Characteristics *cha) {
	bool exist = false;
	for (auto it = _services.begin(); it != _services.end(); it++) {
		for (auto jt = (*it)->_characteristics.begin(); jt != (*it)->_characteristics.end(); jt++) {
			if (*jt == cha) {
				(*it)->_characteristics.erase(jt);
				exist = true;
			}
		}
	}
	return exist;
}

Accessory::Accessory()
{

}

short Accessory::numberOfService() { return _services.size(); }

Service *Accessory::serviceAtIndex(int index) {
	for (auto it = _services.begin(); it != _services.end(); it++) {
		if ((*it)->serviceID == index) {
			return *it;
		}
	}
	return nullptr;
}

Characteristics *Accessory::characteristicsAtIndex(int index) {
	for (auto it = _services.begin(); it != _services.end(); it++) {
		for (auto jt = (*it)->_characteristics.begin(); jt != (*it)->_characteristics.end(); jt++) {
			if ((*jt)->iid == index) {
				return *jt;
			}
		}
	}
	return nullptr;
}

std::string Accessory::describe(net::ConnectionInfo *sender) {
	std::string keys[2];
	std::string values[2];

	{
		keys[0] = "aid";
		char temp[8];
		sprintf(temp, "%d", aid);
		values[0] = temp;
	}

	{
		//Form services list
		int noOfService = numberOfService();
		std::string *services = new std::string[noOfService];
		for (int i = 0; i < noOfService; i++) {
			services[i] = _services[i]->describe(sender);
		}
		keys[1] = "services";
		values[1] = arrayWrap(services, noOfService);
		delete[] services;
	}

	std::string result = dictionaryWrap(keys, values, 2);

	return result;
}