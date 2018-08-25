#include <Characteristics.h>

#include <Accessory.h>
#include <net/HAPService.h>

using namespace hap;

Characteristics::Characteristics(char_type type, permission permission)
	: _type(type), _permission(permission)
{
}

uint16_t Characteristics::getID() const
{
	return _id;
}

uint16_t Characteristics::getAID() const
{
	return _aid;
}

bool Characteristics::writable() const
{ 
	return _permission & permission_write;
}

bool Characteristics::notifiable() const
{ 
	return _permission & permission_notify;
}

void Characteristics::notify()
{
	// If char isn't notifiable skip all
	if (!notifiable()) return;

	// Build notification broadcast message
	net::BroadcastInfo_ptr info = std::make_shared<net::BroadcastInfo>();
	info->sender = this;
	info->desc += "{\"characteristics\":[{\"aid\": ";
	info->desc += std::to_string(_aid);
	info->desc += ", \"iid\": ";
	info->desc += std::to_string(_id);
	info->desc += ", \"value\": ";
	info->desc += getValue();
	info->desc += "}]}";

	net::HAPService::getInstance().announce(info);
}