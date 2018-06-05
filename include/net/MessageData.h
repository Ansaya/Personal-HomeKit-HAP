#ifndef HAP_NET_MESSAGE_DATA
#define HAP_NET_MESSAGE_DATA

#include "MessageDataRecord.h"

namespace hap {

namespace net {

	class MessageData {
		MessageDataRecord records[10];
		unsigned char count = 0;

	public:
		MessageData() {}

		MessageData(const char *rawData, unsigned short len);

		MessageData(const MessageData &data);

		MessageData &operator=(const MessageData &);

		void rawData(const char **dataPtr, unsigned short *len);

		void addRecord(MessageDataRecord& record);

		int recordIndex(unsigned char index);

		char *dataPtrForIndex(unsigned char index);

		unsigned int lengthForIndex(unsigned char index);
	};

}

}

#endif // !HAP_NET_MESSAGE_DATA
