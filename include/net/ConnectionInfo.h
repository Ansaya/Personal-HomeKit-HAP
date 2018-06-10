#ifndef HAP_NET_CONNECTION_INFO
#define HAP_NET_CONNECTION_INFO

#include "Message.h"
#include "Response.h"

#include <atomic>
#include <cstdint>
#include <list>
#include <mutex>
#include <poll.h>
#include <string>
#include <thread>

namespace hap {

namespace net {

class ConnectionInfo {
public:
    bool relay = false;

    char identity[37];
    
    uint8_t controllerToAccessoryKey[32];
    uint8_t accessoryToControllerKey[32];
    unsigned long long numberOfMsgRec = 0;
    unsigned long long numberOfMsgSend = 0;

	int _socketFD;
	std::mutex _socketWrite;

	ConnectionInfo(int socketFD, const std::string& socketName);

	~ConnectionInfo();

    void Poly1305_GenKey(const unsigned char * key, uint8_t * buf, uint16_t len, bool dataWithLength, char* verify);

	void addNotify(void *target, int aid, int iid);

	bool notify(void *target);

	void removeNotify(void *target);

	void clearNotify();

private:
	int _wakeFD;

	const std::string _socketName;
	char _buffer[4096];
	std::thread _clientSocket;
	
	std::atomic_bool _connected;
	std::atomic_bool _verified;

	std::list<void*> _notificationList;

	void _clientSocketLoop(int wakeFD);

	Response_ptr _handlePairSetup(Message& request);

	Response_ptr _handlePairVerify(Message& request);

	std::string _handleAccessory(const std::string& request);
};

}

}

#endif // !HAP_NET_CONNECTION_INFO
