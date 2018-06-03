#ifndef HAP_ACCESSORY
#define HAP_ACCESSORY

#include "Characteristics.h"
#include "net/ConnectionInfo.h"
#include "Service.h"

#include <functional>
#include <string>
#include <vector>


namespace hap {

class Accessory {
public:
    int numberOfInstance = 0;
    int aid;
    std::vector<Service *>_services;

	void addService(Service *ser);

	void addInfoService(std::string accName, std::string manufactuerName, std::string modelName, std::string serialNumber, 
		std::function<void(bool oldValue, bool newValue, net::ConnectionInfo *sender)> identifyCallback);

	void addCharacteristics(Service *ser, Characteristics *cha);

	bool removeService(Service *ser);

	bool removeCharacteristics(Characteristics *cha);

	Accessory();

	short numberOfService();

	Service *serviceAtIndex(int index);

	Characteristics *characteristicsAtIndex(int index);
    
	std::string describe(net::ConnectionInfo *sender);
};

}

#endif // !HAP_ACCESSORY
