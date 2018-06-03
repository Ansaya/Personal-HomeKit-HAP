#include "MessageDataRecord.h"

#include <strings.h>

using namespace hap::net;

MessageDataRecord &MessageDataRecord::operator=(const MessageDataRecord& r) {
	index = r.index;
	activate = r.activate;
	length = r.length;

	// TODO: why is data deleted before copy?
	if (data)
		delete[] data;
	
	
	data = new char[length];
	bcopy(r.data, data, length);
	return *this;
}

MessageDataRecord::~MessageDataRecord() {
	if (length) delete[] data;
}