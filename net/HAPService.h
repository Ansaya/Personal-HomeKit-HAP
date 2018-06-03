#ifndef HAP_NET_HAP_SERVICE
#define HAP_NET_HAP_SERVICE

#define HAPServiceType "_hap._tcp"
#define HAPServiceMaximumConnections 5

#include "../DeviceType.h"
#include "ConnectionInfo.h"

#include <atomic>
#include <dns_sd.h>
#include <list>
#include <thread>

namespace hap {

namespace net {

	struct BroadcastInfo {
		void *sender;
		char *desc;
	};


	class HAPService {

	public:
		static HAPService & getInstance();

		~HAPService();

		void setup(hap::deviceType type);

		void handleConnection();

		void announce(BroadcastInfo* info);

	private:
		static HAPService* _instance;

		std::atomic<bool> _setup;
		hap::deviceType _type;
		int _socket;
		DNSServiceRef _netService;

		int _currentConfiguration = 1;

		std::list<ConnectionInfo*> _connections;
		std::thread _keepAlive;

		HAPService();

		void setupSocket();

		//void handlePairSetup(int subSocket, char *buffer) const;

		//void handlePairVerify(int subSocket, char *buffer) const;

		void broadcastMessage(void *sender, char *resultData, size_t resultLen);

		void keepAliveLoop();

		void connectionLoop(ConnectionInfo* info);

		void updatePairable();

		TXTRecordRef buildTXTRecord();

		friend void ConnectionInfo::handleAccessoryRequest();

		void handleAccessory(const char *request, unsigned int requestLen, char **reply, unsigned int *replyLen, ConnectionInfo *sender);

	};

}

}

#endif // !HAP_NET_HAP_SERVICE
