#ifndef HAP_NET_MESSAGE
#define HAP_NET_MESSAGE

#include "MessageData.h"

namespace hap {

namespace net {

	class Message {
		char method[5];
		char type[40];

	public:
		char directory[20];
		MessageData data;

		Message(const char *rawData);

		void getBinaryPtr(char **buffer, int *contentLength);
	};

}

}

#endif // !HAP_NET_MESSAGE
