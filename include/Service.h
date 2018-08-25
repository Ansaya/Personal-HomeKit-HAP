#ifndef HAP_SERVICE
#define HAP_SERVICE

#include "hapDefines.h"
#include "ServiceType.h"
#include "net/ConnectionInfo.h"

#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace hap {

class Service {
public:
	Service(service_type type);

	int getID() const;

	uint16_t characteristicsCount() const;

	void addCharacteristic(Characteristics_ptr newCharacteristic);

	bool removeCharacteristic(Characteristics_ptr characteristic);

	Characteristics_ptr getCharacteristic(int id) const;

    std::string describe() const;

private:
	int _id;
	int _aid;
	const service_type _type;
	std::shared_ptr<std::atomic_ushort> _parentInstancesID;

	mutable std::mutex _characteristicsList;
	std::vector<Characteristics_ptr> _characteristics;

	Service(Service const&) = delete;

	friend class AccessorySet;
	friend class Accessory;
};

}

#endif // !HAP_SERVICE
