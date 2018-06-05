#include <net/MessageData.h>

#include <cstring>
#include <string>

using namespace hap::net;

MessageData::MessageData(const char *rawData, unsigned short len) {
	unsigned short delta = 0;
	while (delta < len) {
		int index = recordIndex(rawData[delta + 0]);
		if (index < 0) {
			records[count].index = (rawData)[delta + 0];
			records[count].length = (unsigned char)(rawData)[delta + 1];
			records[count].data = new char[records[count].length];
			records[count].activate = true;
			bcopy(&rawData[delta + 2], records[count].data, records[count].length);
			delta += (records[count].length + 2);
			count++;
		}
		else {
			int newLen = ((unsigned char*)(rawData))[delta + 1];
			newLen += records[index].length;
			char *ptr = new char[newLen];
			bcopy(records[index].data, ptr, records[index].length);
			bcopy(&rawData[delta + 2], &ptr[records[index].length], newLen - records[index].length);
			delete[] records[index].data;
			records[index].data = ptr;

			delta += (newLen - records[index].length + 2);
			records[index].length = newLen;

		}
	}
}

MessageData & MessageData::operator=(const MessageData &data) {
	count = data.count;
	for (int i = 0; i < 10; i++) {
		if (data.records[i].length) {
			records[i] = data.records[i];
			records[i].data = new char[records[i].length];
			bcopy(data.records[i].data, records[i].data, data.records[i].length);
		}
	}
	return *this;
}

void MessageData::rawData(const char **dataPtr, unsigned short *len) {
	std::string buffer = "";
	for (int i = 0; i < 10; i++) {
		if (records[i].activate) {
			for (int j = 0; j != records[i].length;) {
				unsigned char len = records[i].length - j>255 ? 255 : records[i].length - j;
				std::string temp(&records[i].data[j], len);
				temp = (char)records[i].index + ((char)len + temp);
				buffer += temp;
				j += (unsigned int)len;
			}
		}
	}
	*len = buffer.length();
	*dataPtr = new char[*len];
	bcopy(buffer.c_str(), (void *)*dataPtr, *len);
}

void MessageData::addRecord(MessageDataRecord& record) {
	records[count] = record;
	count++;
}

int MessageData::recordIndex(unsigned char index) {
	for (int i = 0; i < count; i++) {
		if (records[i].activate&&records[i].index == index) return i;
	}
	return -1;
}

char *MessageData::dataPtrForIndex(unsigned char index) {
	int _index = recordIndex(index);
	if (_index >= 0)
		return records[_index].data;
	return 0;
}

unsigned int MessageData::lengthForIndex(unsigned char index) {
	int _index = recordIndex(index);
	if (_index >= 0)
		return records[_index].length;
	return 0;
}