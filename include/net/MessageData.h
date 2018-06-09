#ifndef HAP_NET_MESSAGE_DATA
#define HAP_NET_MESSAGE_DATA

#include "MessageDataRecord.h"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace hap {

namespace net {

	class MessageData {
	public:
		MessageData();

		MessageData(const std::string& rawData);

		MessageData(const MessageData& copy);

		MessageData &operator=(const MessageData &);

		std::string rawData() const;

		void addRecord(MessageDataRecord_ptr record);

		ConstMessageDataRecord_ptr getRecordForIndex(uint8_t index) const;

	private:
		std::vector<MessageDataRecord_ptr> _records;

		MessageDataRecord_ptr _findRecord(uint8_t recordIndex) const;
	};

	typedef std::shared_ptr<MessageData> MessageData_ptr;

	typedef std::shared_ptr<const MessageData> ConstMessageData_ptr;

}

}

#endif // !HAP_NET_MESSAGE_DATA
