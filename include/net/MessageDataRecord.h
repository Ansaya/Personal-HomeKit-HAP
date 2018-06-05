#ifndef HAP_NET_MESSAGE_DATA_RECORD
#define HAP_NET_MESSAGE_DATA_RECORD


namespace hap {

namespace net {

class MessageDataRecord {
public:
    unsigned char index = 0;
    char *data = 0;
    unsigned int length = 0;
    bool activate = false;

    ~MessageDataRecord();

    MessageDataRecord &operator=(const MessageDataRecord&);
};

}

}

#endif // !HAP_NET_MESSAGE_DATA_RECORD
