#ifndef HAP_NET_CONNECTION_INFO
#define HAP_NET_CONNECTION_INFO

#include <cstdint>
#include <list>
#include <mutex>
#include <string>
#include <thread>

namespace hap {

namespace net {

class ConnectionInfo {
public:
    std::thread thread;
    std::mutex mutex;

    bool connected = false;
    bool relay = false;

    char identity[37];
    std::string hostname;
    
    uint8_t controllerToAccessoryKey[32];
    uint8_t accessoryToControllerKey[32];
    unsigned long long numberOfMsgRec = 0;
    unsigned long long numberOfMsgSend = 0;
    int subSocket = -1;
    char buffer[4096];

	std::list<void*> _notificationList;

    void handlePairSetup();
    void handlePairVerify();
    void handleAccessoryRequest();

    void Poly1305_GenKey(const unsigned char * key, uint8_t * buf, uint16_t len, bool dataWithLength, char* verify);

	void addNotify(void *target, int aid, int iid);

	bool notify(void *target);

	void removeNotify(void *target);

	void clearNotify();
};

}

}

#endif // !HAP_NET_CONNECTION_INFO
