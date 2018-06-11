#include <net/HAPService.h>

#include <Accessory.h>
#include <AccessorySet.h>
#include <KeyController.h>
#include <net/Message.h>

#include "../Configuration.h"
#include "../Chacha20/chacha20_simple.h"
#include "../Helpers.h"
#include "../srp/srp.h"

#include <algorithm>
#include <chrono>
#include <mutex>
#include <netdb.h>
#include <string>
#include <sys/socket.h>

using namespace hap;
using namespace hap::net;

static const char hapJsonType[] = "application/hap+json";

HAPService& HAPService::getInstance()
{
	static HAPService instance;

	return instance;
}

HAPService::HAPService() : _running(false), _currentConfiguration(1)
{
}

HAPService::~HAPService()
{
	stop();
}

bool HAPService::setupAndListen(deviceType type)
{
	if (_running.load()) {
#ifdef HAP_DEBUG
		printf("HAPService::setupAndListen : hap service already running.\n");
#endif
		return true;
	}

	_type = type;

	int retval = SRP_initialize_library();
	if (!SRP_OK(retval)) {
#ifdef HAP_DEBUG
		printf("HAPService::setupAndListen : error during SRP initalization.\n");
#endif
		return false;
	}

	srand((unsigned int)time(NULL));

	// Socket setup
	_socket = socket(PF_INET, SOCK_STREAM, 0);
	if (_socket < 0) {
#ifdef HAP_DEBUG
		printf("HAPService::setupAndListen : error during socket initialization : %s\n",
			strerror(errno));
#endif
		return false;
	}

	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_family = PF_INET;
	addr.sin_port = htons(0);

	int optval = 1;
	socklen_t optlen = sizeof(optval);
	retval = setsockopt(_socket, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen);
	if (retval < 0) {
#ifdef HAP_DEBUG
		printf("HAPService::setupAndListen : error during socket initialization : %s\n",
			strerror(errno));
#endif
		return false;
	}

	retval = bind(_socket, (const struct sockaddr *)&addr, sizeof(addr));
	if (retval < 0) {
#ifdef HAP_DEBUG
		printf("HAPService::setupAndListen : error during socket initialization : %s\n",
			strerror(errno));
#endif
		return false;
	}

	retval = listen(_socket, (unsigned int)HAPServiceMaximumConnections);
	if (retval < 0) {
#ifdef HAP_DEBUG
		printf("HAPService::setupAndListen : error during socket initialization : %s\n",
			strerror(errno));
#endif
		return false;
	}

	// Get listening port
	sockaddr_in address;
	socklen_t len = sizeof(address);
	getsockname(_socket, (struct sockaddr *)&address, &len);

	uint16_t servicePort = htons(ntohs(address.sin_port));

	// Initialize DNS SD service
	TXTRecordRef txtRecord = buildTXTRecord();

	retval = DNSServiceRegister(&_netService, 0, 0, HAP_DEVICE_NAME, HAPServiceType, "", NULL,
		servicePort, TXTRecordGetLength(&txtRecord),
		TXTRecordGetBytesPtr(&txtRecord), NULL, NULL);

	TXTRecordDeallocate(&txtRecord);

	if (retval != kDNSServiceErr_NoError) {
#ifdef HAP_DEBUG
		printf("HAPService::setupAndListen : error during DNS SD initialization.\n");
#endif
		return false;
	}

	int wakeFD[2];
	retval = pipe(wakeFD);
	if (retval < 0) {
#ifdef HAP_DEBUG
		printf("HAPService::setupAndListen : error during wake pipe allocation.\n");
#endif
		return false;
	}

	_running.store(true);

	_listenerWakeFD = wakeFD[1];
	_listener = new std::thread(&HAPService::listenerLoop, this, wakeFD[0]);

	_keepAlive = new std::thread(&HAPService::keepAliveLoop, this);

	_broadcast = new std::thread(&HAPService::broadcastLoop, this);

#ifdef HAP_DEBUG
	printf("HAPService::setupAndListen : hap service listening on port %u.\n", servicePort);
#endif

	return true;
}

void HAPService::stop()
{
	if (!_running.load()) return;

	_running.store(false);

	// Dispose all clients connections
	for (auto it : _connections) {
		delete it;
	}

	// Dispose connection listener thread
	char c = 1;
	write(_listenerWakeFD, &c, 1);
	_listener->join();
	close(_listenerWakeFD);
	_listenerWakeFD = -1;

	// Dispose keep alive thread
	_keepAliveStop.notify_all();
	_keepAlive->join();

	// Dispose broadcast thread
	BroadcastInfo_ptr fake = nullptr;
	_broadcastInfo.push_back(fake);
	_broadcastCV.notify_all();
	_broadcast->join();

	// Dispose dns and srp stuff
	DNSServiceRefDeallocate(_netService);
	SRP_finalize_library();

#ifdef HAP_DEBUG
	printf("HAPService::stop : hap service stopped.\n");
#endif
}

void HAPService::announce(BroadcastInfo_ptr info)
{
	if (!_running.load()) return;

	std::unique_lock<std::mutex> lock(_broadcastInfoMutex);

	_broadcastInfo.push_back(info);
	_broadcastCV.notify_all();
}

void HAPService::listenerLoop(int wakeFD)
{
	struct pollfd fds[2];
	fds[0].fd = _socket;
	fds[0].events = POLLIN | POLLHUP | POLLERR;
	fds[1].fd = wakeFD;
	fds[1].events = POLLIN;

#ifdef HAP_DEBUG
	printf("HAPService::listenerLoop : listener ready for incoming connections...\n");
#endif

	do {
		// Poll for ten seconds to avoid deadlock in case of some error
		poll(fds, 2, 10000);

		if (fds[0].revents & (POLLHUP | POLLERR)) {
#ifdef HAP_DEBUG
			printf("HAPService::listenerLoop : error on listener socket.\n");
#endif
			break;
		}

		if (fds[0].revents & POLLIN) {

			struct sockaddr_in client_addr;
			socklen_t clen;

			int subSocket = accept(_socket, (struct sockaddr *)&client_addr, &clen);

			//Before anything start, get sniff the host name of the client
			std::string socketName = "";
			if (clen == sizeof(struct sockaddr_in)) {
				char buffer[128];
				int retval = getnameinfo((struct sockaddr *)&client_addr, clen, buffer,
					sizeof(buffer), NULL, 0, NI_NOFQDN);
				if (retval) {
#ifdef HAP_DEBUG
					printf("HAPService::listenerLoop : error : could not read socket "
						"name : %s\n", gai_strerror(retval));
#endif
				}
				else
					socketName = std::string(buffer);
			}

			ConnectionInfo* newConn = new ConnectionInfo(subSocket, socketName);

			_connections.push_back(newConn);

#ifdef HAP_DEBUG
			printf("HAPService::listenerLoop : new client connection: \"%d\" %s\n",
				subSocket, socketName.c_str());
#endif
		}
	} while (_running.load());

	if (shutdown(_socket, SHUT_RDWR) != 0) {
#ifdef HAP_DEBUG
		printf("HAPService::stop : error on socket shutdown : %s\n", strerror(errno));
#endif
	}

	// Dispose wake pipe
	close(wakeFD);

#ifdef HAP_DEBUG
	printf("HAPService::listenerLoop : listener shutdown.\n");
#endif
}

void HAPService::keepAliveLoop()
{
#ifdef HAP_DEBUG
	printf("HAPService::keepAliveLoop : keep alive running.\n");
#endif
	std::mutex m;
	BroadcastInfo_ptr alivePackage = std::make_shared<BroadcastInfo>();
	alivePackage->sender = nullptr;
	alivePackage->desc = "{\"characteristics\": []}";

	do {
		// Wait for given keep alive timeout or for shutdown notification
		std::unique_lock<std::mutex> lock(m);
		std::cv_status state = _keepAliveStop
			.wait_for(lock, std::chrono::seconds(HAP_NET_HAP_SERVICE_KEEP_ALIVE_PERIOD));

		// If notified before timeout it's time to quit
		if (state == std::cv_status::no_timeout) break;

#ifdef HAP_DEBUG
		printf("HAPService::keepAliveLoop : keep alive message sent.\n");
#endif
		broadcastMessage(alivePackage);

	} while (_running.load());

#ifdef HAP_DEBUG
	printf("HAPService::keepAliveLoop : keep alive shutdown.\n");
#endif
}

void HAPService::broadcastLoop()
{
#ifdef HAP_DEBUG
	printf("HAPService::broadcastLoop : broadcast loop running.\n");
#endif
	BroadcastInfo_ptr bi;

	do {
		{
			// Wait for something to send
			std::unique_lock<std::mutex> lock(_broadcastInfoMutex);
			_broadcastCV.wait(lock, [&]() { return !_broadcastInfo.empty(); });

#ifdef HAP_DEBUG
			printf("HAPService::broadcastLoop : wake up from wait.\n");
#endif

			// Close if not running any more
			if (!_running.load()) break;

			// Get first element, broadcast it and pop it from the list
			bi = _broadcastInfo.front();
			_broadcastInfo.pop_front();
		}

		if (bi != nullptr)
			broadcastMessage(bi);

	} while (_running.load());

#ifdef HAP_DEBUG
	printf("HAPService::broadcastLoop : broadcast loop shutdown.\n");
#endif
}

//Broadcast a event message, use for notifcation or keep alive
void HAPService::broadcastMessage(BroadcastInfo_ptr& info)
{
	std::string data;
	data += "EVENT/1.0 200 OK\r\n";
	data += "Content-Type: ";
	data += hapJsonType;
	data += "\r\n";
	data += "Content-Length: ";
	data += std::to_string(info->desc.size());
	data += "\r\n\r\n";
	data += info->desc;

#ifdef HAP_NET_DEBUG
	printf("Broadcasting message:\n%s\n\n", data.c_str());
#endif

	for (auto& conn : _connections) {
		if (info->sender == nullptr || conn->isNotified(info->sender))
			conn->send(data);
	}
}

void HAPService::updatePairable() {
	TXTRecordRef txtRecord = buildTXTRecord();
	DNSServiceUpdateRecord(_netService, NULL, 0, TXTRecordGetLength(&txtRecord), TXTRecordGetBytesPtr(&txtRecord), 0);
	TXTRecordDeallocate(&txtRecord);
}

TXTRecordRef HAPService::buildTXTRecord() {
	TXTRecordRef txtRecord;
	TXTRecordCreate(&txtRecord, 0, NULL);
	TXTRecordSetValue(&txtRecord, "pv", 3, "1.0");  //Version
	TXTRecordSetValue(&txtRecord, "id", 17, HAP_DEVICE_MAC);    //Device id
	char buf[3];
	sprintf(buf, "%d", _currentConfiguration.load());
	TXTRecordSetValue(&txtRecord, "c#", 1, buf);    //Configuration Number
	TXTRecordSetValue(&txtRecord, "s#", 1, "4");    //Number of service
	if (KeyController::getInstance().hasController())
#ifdef HAP_MULTIPLE_PAIRING
		buf[0] = '1';
#else
		buf[0] = '0';
#endif
	else
		buf[0] = '1';
	TXTRecordSetValue(&txtRecord, "sf", 1, buf);    //Discoverable: 0 if has been paired
	TXTRecordSetValue(&txtRecord, "ff", 1, "0");    //1 for MFI product
	TXTRecordSetValue(&txtRecord, "md", strlen(HAP_DEVICE_NAME), HAP_DEVICE_NAME);    //Model Name
	int len = sprintf(buf, "%d", _type);
	TXTRecordSetValue(&txtRecord, "ci", len, buf);    //1 for MFI product
	return txtRecord;
}