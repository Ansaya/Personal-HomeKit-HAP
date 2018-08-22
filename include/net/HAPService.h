#ifndef HAP_NET_HAP_SERVICE
#define HAP_NET_HAP_SERVICE

#define HAPServiceType "_hap._tcp"
#define HAPServiceMaximumConnections 5

#include "../DeviceType.h"
#include "../hapDefines.h"
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
		const void *sender;
		std::string desc;
	};

	typedef std::shared_ptr<BroadcastInfo> BroadcastInfo_ptr;

	class HAPService {

	public:
		static HAPService &getInstance();

		~HAPService();

		/**
		 *	@brief Initialize and start the Apple HomeKit bridge service
		 *
		 *	@note Client keys are stored in ./hap_client_keys by default, to change key file
		 *		  path call KeyController::setKeyRecordPath method
		 *
		 *	@param name service name to show
		 *	@param password pin code to require on client connection (format needed is XXX-XX-XXX, where X is an integer number)
		 */
		bool setupAndListen(const std::string& name, const std::string& password);

		void stop();

		bool isRunning() const;

		void announce(BroadcastInfo_ptr info);

		/**
		 *	@brief Check if given string is a valid password for the HAP service
		 *
		 *	@note HAP password should be formatted as XXX-XX-XXX where X is an integer number 0-9
		 *
		 *	@param password password string to be validated
		 *
		 *	@return True if the password is valid, false else
		 */
		bool validatePassword(const std::string& password);

	private:
		std::string _name;
		std::string _password;
		std::atomic_bool _running;
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
