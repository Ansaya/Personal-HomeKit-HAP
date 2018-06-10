#ifndef HAP_NET_MESSAGE
#define HAP_NET_MESSAGE

#include "MessageData.h"

#include <memory>
#include <string>

namespace hap {

namespace net {

	class Message {
	public:
		Message(const char *rawData, size_t length);

		Message(const Message& copy);

		~Message();

		const std::string &getMethod() const;
		const std::string &getPath() const;
		const std::string &getContentType() const;
		ConstMessageData_ptr getContent() const;

		std::string getOriginalRequest();

	private:
		std::string _method;
		std::string _path;
		std::string _contentType;
		MessageData_ptr _content;
	};

	typedef std::shared_ptr<Message> Message_ptr;

}

}

#endif // !HAP_NET_MESSAGE
