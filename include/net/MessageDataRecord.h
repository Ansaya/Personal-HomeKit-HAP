#ifndef HAP_NET_MESSAGE_DATA_RECORD
#define HAP_NET_MESSAGE_DATA_RECORD

#include <cstdint>
#include <memory>
#include <string>

namespace hap {

namespace net {

class MessageDataRecord {
public:
	MessageDataRecord(uint8_t index, const std::string& data, bool activate = false);

    ~MessageDataRecord();

	uint8_t getIndex() const;

	const std::string& getData() const;

	void pushData(const std::string& data);

	bool isActive() const;

	void setActive(bool state);

private:
	const uint8_t _index;
	std::string _content;
	bool _active;
};

typedef std::shared_ptr<MessageDataRecord> MessageDataRecord_ptr;

typedef std::shared_ptr<const MessageDataRecord> ConstMessageDataRecord_ptr;

}

}

#endif // !HAP_NET_MESSAGE_DATA_RECORD
