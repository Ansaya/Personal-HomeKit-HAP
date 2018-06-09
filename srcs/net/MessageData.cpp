#include <net/MessageData.h>

#include <algorithm>
#include <cstring>
#include <string>

using namespace hap::net;

MessageData::MessageData()
{
}

MessageData::MessageData(const std::string& rawData)
{
	// Decode message data structure into single data records
	// [rawData] = [index (1 byte) | length (1 byte) | data record (255 bytes max) ]*
	size_t delta = 0;
	while (delta < rawData.length()) {
		uint8_t currentIndex = rawData[delta + 0];
		uint8_t dataLength = rawData[delta + 1];
		std::string recordData = rawData.substr(delta + 2, dataLength);

		MessageDataRecord_ptr currentRecord = _findRecord(currentIndex);
		if (currentRecord == nullptr) {
			currentRecord = std::make_shared<MessageDataRecord>(
				currentIndex, 
				recordData, 
				true);

			_records.push_back(currentRecord);
		}
		else {
			currentRecord->pushData(recordData);
		}
		delta += dataLength + 2;
	}
}

MessageData::MessageData(const MessageData &copy)
{
	*this = copy;
}

MessageData & MessageData::operator=(const MessageData &data)
{
	// Copy each record to the new object
	for (auto& it : data._records)
		_records.push_back(std::make_shared<MessageDataRecord>(*it));

	return *this;
}

std::string MessageData::rawData() const
{
	std::string buffer;

	// Write each record back to a signle buffer structure as 
	// [index (1 byte) | length (1 byte) | data record (255 bytes max) ]*
	for (auto& it : _records) {
		if (it->isActive()) {
			// Restore 255 bytes data chunks
			size_t i = 0;
			for (; i < it->getData().length() / (uint8_t)UINT8_MAX; i++) {
				buffer += (char)it->getIndex();
				buffer += (char)UINT8_MAX;
				buffer += it->getData().substr(i * UINT8_MAX, UINT8_MAX);
			}
			uint8_t tailLength = it->getData().length() % (uint8_t)UINT8_MAX;
			buffer += (char)it->getIndex();
			buffer += (char)tailLength;
			buffer += it->getData().substr(i * 255, tailLength);
		}
	}

	return buffer;
}

void MessageData::addRecord(MessageDataRecord_ptr record)
{
	_records.push_back(record);
}

ConstMessageDataRecord_ptr MessageData::getRecordForIndex(uint8_t index) const
{
	return _findRecord(index);
}

MessageDataRecord_ptr MessageData::_findRecord(uint8_t index) const
{
	auto mdr = std::find_if(_records.begin(), _records.end(),
		[index](const MessageDataRecord_ptr& mdr_p)
	{
		return mdr_p->getIndex() == index;
	});

	if (mdr != _records.end())
		return *mdr;

	return nullptr;
}