#ifndef HAP_SERVICE
#define HAP_SERVICE

#include "Characteristics.h"
#include "ServiceType.h"
#include "net/ConnectionInfo.h"

#include <string>
#include <vector>

namespace hap {

class Service {
public:
    int serviceID, uuid;
    std::vector<Characteristics *> _characteristics;

	Service(service_type _uuid);

	virtual short numberOfCharacteristics();

	virtual Characteristics *characteristicsAtIndex(int index);

    std::string describe(net::ConnectionInfo *sender);
};

}

#endif // !HAP_SERVICE