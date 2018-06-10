#include <net/MessageDataRecord.h>

using namespace hap::net;

MessageDataRecord::MessageDataRecord(uint8_t index, const std::string& data, bool activate)
	: _index(index), _content(data), _active(activate)
{
}

MessageDataRecord::~MessageDataRecord()
{
}

uint8_t MessageDataRecord::getIndex() const
{
	return _index;
}

const std::string& MessageDataRecord::getData() const
{
	return _content;
}

void MessageDataRecord::pushData(const std::string& data)
{
	_content.append(data);
}

bool MessageDataRecord::isActive() const
{
	return _active;
}

void MessageDataRecord::setActive(bool state)
{
	_active = state;
}