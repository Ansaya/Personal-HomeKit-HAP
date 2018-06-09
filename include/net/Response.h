#ifndef HAP_NET_RESPONSE
#define HAP_NET_RESPONSE

#include "MessageData.h"

#include <cstdint>
#include <memory>
#include <string>

namespace hap {

namespace net {

	class Response {
	public:
		Response(uint16_t _responseCode, ConstMessageData_ptr);

		Response(const Response& copy);

		std::string getResponse();

	private:
		uint16_t _responseCode;
		ConstMessageData_ptr _content;
	};

	typedef std::shared_ptr<Response> Response_ptr;

}

}

#endif // !HAP_NET_RESPONSE
