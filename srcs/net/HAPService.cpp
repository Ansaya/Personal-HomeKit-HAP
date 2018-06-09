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
static const char pairingTlv8Type[] = "application/pairing+tlv8";

HAPService& HAPService::getInstance()
{
	static HAPService instance;

	return instance;
}

HAPService::HAPService() : _setup(false), _currentConfiguration(1) {
}

HAPService::~HAPService() {
	if (_setup.load()) {
		DNSServiceRefDeallocate(_netService);
		SRP_finalize_library();
	}
}

void HAPService::setup(deviceType type)
{
	_type = type;

	SRP_initialize_library();
	srand((unsigned int)time(NULL));

	//_keepAlive = new std::thread(&HAPService::keepAliveLoop, this);

	TXTRecordRef txtRecord = buildTXTRecord();

	// Socket setup
	_socket = socket(PF_INET, SOCK_STREAM, 0);
	sockaddr_in addr;
	bzero(&addr, sizeof(addr));
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_family = PF_INET;
	addr.sin_port = htons(0);

	int optval = 1;	
	socklen_t optlen = sizeof(optval);
	setsockopt(_socket, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen);

	bind(_socket, (const struct sockaddr *)&addr, sizeof(addr));
	listen(_socket, (unsigned int)HAPServiceMaximumConnections);

	// Get listening port
	sockaddr_in address;
	socklen_t len = sizeof(address);
	getsockname(_socket, (struct sockaddr *)&address, &len);

	DNSServiceRegister(&_netService, 0, 0, HAP_DEVICE_NAME, HAPServiceType, "", NULL,
		htons(ntohs(address.sin_port)), TXTRecordGetLength(&txtRecord), 
		TXTRecordGetBytesPtr(&txtRecord), NULL, NULL);

	TXTRecordDeallocate(&txtRecord);

	_setup.store(true);

#ifdef HAP_DEBUG
	printf("HAPService::setup : hap service setup completed.\n");
#endif // HAP_DEBUG
}

void HAPService::handleConnection() {
	if (!_setup.load()) {
		printf("HAPService::handleConnection : please call HAPService::setup before!\n");
		return;
	}

	struct sockaddr_in client_addr; 
	socklen_t clen;

	int subSocket = accept(_socket, (struct sockaddr *)&client_addr, &clen);

	//Before anything start, get sniff the host name of the client
	std::string socketName = "";
	if (clen == sizeof(struct sockaddr_in)) {
		char buffer[128];
		int res = getnameinfo((struct sockaddr *)&client_addr, clen, buffer, sizeof(buffer), NULL, 0, NI_NOFQDN);
		if(res == 0)
			socketName = std::string(buffer);
	}

	ConnectionInfo* newConn = new ConnectionInfo(subSocket, socketName);

	_connections.push_back(newConn);

#ifdef HAP_DEBUG
	printf("HAPService::handleConnection : New client connected: \"%d\"  %s\n", subSocket, socketName.c_str());
#endif // HAP_DEBUG
}

void HAPService::announce(BroadcastInfo *info) {

	void *sender = info->sender;
	char *desc = info->desc;

	char reply[1024];
	int len = snprintf(reply, 1024, 
		"EVENT/1.0 200 OK\r\n"
		"Content-Type: %s\r\n"
		"Content-Length: %lu\r\n"
		"\r\n"
		"%s",
		hapJsonType,
		strlen(desc), 
		desc);

#ifdef HAP_NET_DEBUG
	printf("Announce message:\n%s\n", reply);
#endif

	broadcastMessage(sender, reply, len);

	delete[] desc;
	delete info;
}

//Broadcast a event message, use for notifcation or keep alive
void HAPService::broadcastMessage(void *sender, char *resultData, size_t resultLen) {

	for (auto& conn : _connections) {

		int socketNumber = conn->_socketFD;

		if (socketNumber >= 0 && conn->numberOfMsgSend >= 0 && (conn->notify(sender) || sender == nullptr)) {

#ifdef HAP_DEBUG
			printf("HAPService::broadcastMessage : sending %d byte message to client %d\n", resultLen, socketNumber);
#endif
			std::unique_lock<std::mutex> lock(conn->_socketWrite);

			chacha20_ctx chacha20;
			bzero(&chacha20, sizeof(chacha20));

			char temp[64];
			bzero(temp, 64);
			char temp2[64];
			bzero(temp2, 64);

			unsigned char *reply = new unsigned char[resultLen + 18];
			reply[0] = resultLen % 256;
			reply[1] = (resultLen - (uint8_t)reply[0]) / 256;

			unsigned long long numberOfMsgSend = conn->numberOfMsgSend;

			chacha20_setup(&chacha20, (const uint8_t *)conn->accessoryToControllerKey, 32, (uint8_t *)&numberOfMsgSend);
			conn->numberOfMsgSend++;

			chacha20_encrypt(&chacha20, (const uint8_t*)temp, (uint8_t *)temp2, 64);
			chacha20_encrypt(&chacha20, (const uint8_t*)resultData, (uint8_t*)&reply[2], resultLen);

			char verify[16];
			memset(verify, 0, 16);
			conn->Poly1305_GenKey((const unsigned char *)temp2, (uint8_t *)reply, resultLen, true, verify);
			memcpy((unsigned char*)&reply[resultLen + 2], verify, 16);

			write(socketNumber, reply, resultLen + 18);
			delete[] reply;
		}
	}
}

void HAPService::keepAliveLoop() 
{
	while (true) {
#ifdef HAP_DEBUG
		printf("HAPService::keepAliveLoop : keep alive.\n");
#endif // HAP_DEBUG

		BroadcastInfo *alivePackage = new BroadcastInfo;
		alivePackage->sender = nullptr;
		char *aliveMsg = new char[32];
		strncpy(aliveMsg, "{\"characteristics\": []}", 32);
		alivePackage->desc = aliveMsg;
		announce(alivePackage);
		std::this_thread::sleep_for(std::chrono::seconds(HAP_NET_HAP_SERVICE_KEEP_ALIVE_PERIOD));
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