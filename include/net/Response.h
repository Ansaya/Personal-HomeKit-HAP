#ifndef HAP_NET_RESPONSE
#define HAP_NET_RESPONSE

#include "MessageData.h"

#include <string>

namespace hap {

namespace net {

	class Response {
		unsigned short responseCode;
		std::string responseType();

	public:
		MessageData data;

		Response(unsigned short _responseCode);

		void getBinaryPtr(char **buffer, int *contentLength);

	};

}

}

#endif // !HAP_NET_RESPONSE
