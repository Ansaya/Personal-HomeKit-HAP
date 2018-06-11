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
	ConnectionInfo(int socketFD, const std::string& socketName);

	~ConnectionInfo();

	void send(const std::string& data);

	void addNotify(void *target, int aid, int iid);

	bool isNotified(void *target);

	void removeNotify(void *target);

	void clearNotify();

private:
	int _wakeFD;
	int _socketFD;
	const std::string _socketName;
	char _buffer[4096];
	std::thread _clientSocket;
	std::mutex _socketWrite;
	std::atomic_bool _connected;
	std::atomic_bool _verified;
	uint64_t _nonceReceive = 0;
	uint64_t _nonceSend = 0;

	char identity[37];
	uint8_t controllerToAccessoryKey[32];
	uint8_t accessoryToControllerKey[32];

	std::list<void*> _notificationList;

	void _clientSocketLoop(int wakeFD);

	Response_ptr _handlePairSetup(Message& request);

	Response_ptr _handlePairVerify(Message& request);

	std::string _handleAccessory(const std::string& request);

	std::string encrypt(const std::string& data, uint8_t nonce[8], const uint8_t* key, uint8_t keyLength, bool dataOnly) const;

	std::string decrypt(const char* encryptedData, uint16_t encryptedDataLength, uint8_t nonce[8], const uint8_t* key, uint8_t keyLength, bool dataOnly) const;

	void Poly1305_GenKey(const uint8_t * key, uint8_t * buf, uint16_t len, bool dataWithLength, uint8_t* verify) const;
};

}

}

#endif // !HAP_NET_CONNECTION_INFO
