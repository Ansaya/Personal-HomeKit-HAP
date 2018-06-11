#ifndef HAP_NET_HAP_SERVICE
#define HAP_NET_HAP_SERVICE

#define HAPServiceType "_hap._tcp"
#define HAPServiceMaximumConnections 5

#include "../DeviceType.h"
#include "ConnectionInfo.h"

#include <atomic>
#include <condition_variable>
#include <dns_sd.h>
#include <list>
#include <memory>
#include <mutex>
#include <thread>

namespace hap {

namespace net {

	struct BroadcastInfo {
		void *sender;
		std::string desc;
	};

	typedef std::shared_ptr<BroadcastInfo> BroadcastInfo_ptr;

	class HAPService {

	public:
		static HAPService &getInstance();

		~HAPService();

		bool setupAndListen(hap::deviceType type);

		void stop();

		void announce(BroadcastInfo_ptr info);

	private:
		std::atomic_bool _running;
		hap::deviceType _type;
		int _socket;
		DNSServiceRef _netService;

		std::atomic_int _currentConfiguration;

		// Connection handling
		int _listenerWakeFD;
		std::thread* _listener;
		std::list<ConnectionInfo*> _connections;

		void listenerLoop(int wakeFD);

		std::condition_variable _keepAliveStop;
		std::thread* _keepAlive;

		void keepAliveLoop();

		std::condition_variable _broadcastCV;
		std::mutex _broadcastInfoMutex;
		std::list<BroadcastInfo_ptr> _broadcastInfo;
		std::thread* _broadcast;

		void broadcastLoop();

		void broadcastMessage(BroadcastInfo_ptr& info);

		HAPService();
		HAPService(const HAPService&) = delete;
		void operator=(const HAPService&) = delete;

		TXTRecordRef buildTXTRecord();

		friend class ConnectionInfo;

		void updatePairable();
	};

}

}

#endif // !HAP_NET_HAP_SERVICE
