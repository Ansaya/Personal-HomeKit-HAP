#include "Characteristics.h"

#include "Accessory.h"
#include "net/HAPService.h"

#include <thread>

using namespace hap;

void Characteristics::notify()
{
	char *broadcastTemp = new char[1024];
	snprintf(broadcastTemp, 1024, "{\"characteristics\":[{\"aid\": %d, \"iid\": %d, \"value\": %s}]}", accessory->aid, iid, value(NULL).c_str());

	net::BroadcastInfo * info = new net::BroadcastInfo;
	info->sender = this;
	info->desc = broadcastTemp;

	std::thread t(&net::HAPService::announce, net::HAPService::getInstance(), info);
	t.detach();
}