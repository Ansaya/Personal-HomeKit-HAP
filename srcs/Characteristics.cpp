#include <Characteristics.h>

#include <Accessory.h>
#include <net/HAPService.h>

#include <thread>

using namespace hap;

Characteristics::Characteristics(char_type _type, permission _premission)
	: type(_type), premission(_premission)
{

}

void Characteristics::setValue(std::string str)
{
	setValue(str, nullptr);
	notify();
}

void Characteristics::notify()
{
	// If char isn't notifiable skip all
	if (!notifiable()) return;

	// Build notification broadcast message
	net::BroadcastInfo_ptr info = std::make_shared<net::BroadcastInfo>();
	info->sender = this;
	info->desc += "{\"characteristics\":[{\"aid\": ";
	info->desc += std::to_string(accessory->aid);
	info->desc += ", \"iid\": ";
	info->desc += std::to_string(iid);
	info->desc += ", \"value\": ";
	info->desc += value(nullptr);
	info->desc += "}]}";

	net::HAPService::getInstance().announce(info);
}