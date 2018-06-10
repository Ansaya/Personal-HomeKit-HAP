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
		static HAPService &getInstance();

		~HAPService();

		void setup(hap::deviceType type);

		void handleConnection();

		void announce(BroadcastInfo* info);

	private:
		std::atomic_bool _setup;
		hap::deviceType _type;
		int _socket;
		DNSServiceRef _netService;

		std::atomic_int _currentConfiguration;

		std::list<ConnectionInfo*> _connections;
		std::thread* _keepAlive;

		HAPService();
		HAPService(const HAPService&) = delete;
		void operator=(const HAPService&) = delete;

		void broadcastMessage(void *sender, char *resultData, size_t resultLen);

		void keepAliveLoop();

		TXTRecordRef buildTXTRecord();

		friend class ConnectionInfo;

		void updatePairable();
	};

}

}

#endif // !HAP_NET_HAP_SERVICE
