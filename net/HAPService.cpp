#include "HAPService.h"

#include "../Accessory.h"
#include "../AccessorySet.h"
#include "../Configuration.h"
#include "../Helpers.h"
#include "../KeyController.h"
#include "Message.h"

#include "../Chacha20/chacha20_simple.h"
#include "../srp/srp.h"

#include <algorithm>
#include <byteswap.h>
#include <dns_sd.h>
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
	if (_instance == nullptr)
		_instance = new HAPService();

	return *_instance;
}

HAPService::HAPService() {
	_setup = false;
}

HAPService::~HAPService() {
	if(_setup)
		DNSServiceRefDeallocate(_netService);
}

void HAPService::setup(deviceType type)
{
	_type = type;

	setupSocket();

	_setup = true;
}

void HAPService::setupSocket()
{
	SRP_initialize_library();
	srand((unsigned int)time(NULL));

	_keepAlive = std::thread(&HAPService::keepAliveLoop, this);

	TXTRecordRef txtRecord = buildTXTRecord();

	// Socket setup
	_socket = socket(PF_INET, SOCK_STREAM, 0);
	sockaddr_in addr;   
	bzero(&addr, sizeof(addr));
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_family = PF_INET;
	addr.sin_port = htons(0);

	int optval = 1;	socklen_t optlen = sizeof(optval);
	setsockopt(_socket, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen);

	bind(_socket, (const struct sockaddr *)&addr, sizeof(addr));
	listen(_socket, HAPServiceMaximumConnections);

	// Get listening port
	sockaddr_in address;
	socklen_t len = sizeof(address);
	getsockname(_socket, (struct sockaddr *)&address, &len);
	unsigned short portNumber = ntohs(address.sin_port);

	DNSServiceRegister(&_netService, 0, 0, deviceName, HAPServiceType, "", NULL,
		htons(portNumber), TXTRecordGetLength(&txtRecord), TXTRecordGetBytesPtr(&txtRecord), NULL, NULL);

	TXTRecordDeallocate(&txtRecord);
}

void HAPService::handleConnection() {
	struct sockaddr_in client_addr; 
	socklen_t clen;

	int subSocket = accept(_socket, (struct sockaddr *)&client_addr, &clen);

	//Before anything start, get sniff the host name of the client
	std::string socketName = "";
	if (clen == sizeof(struct sockaddr_in)) {
		char buffer[1024];
		int res = getnameinfo((struct sockaddr *)&client_addr, clen, buffer, 1024, NULL, 0, NI_NOFQDN);
		socketName = std::string(buffer);
	}

	ConnectionInfo* newConn = new ConnectionInfo();
	newConn->subSocket = subSocket;
	newConn->hostname = socketName;

	_connections.push_back(newConn);

	newConn->thread = std::thread(&HAPService::connectionLoop, this, newConn);
	newConn->thread.detach();

}

void HAPService::announce(BroadcastInfo *info) {

	void *sender = info->sender;
	char *desc = info->desc;

	char reply[1024];
	int len = snprintf(reply, 1024, "EVENT/1.0 200 OK\r\nContent-Type: application/hap+json\r\nContent-Length: %lu\r\n\r\n%s", strlen(desc), desc);

#if HomeKitLog == 1 && HomeKitReplyHeaderLog==1
	printf("%s\n", reply);
#endif

	broadcastMessage(sender, reply, len);

	delete[] desc;
	delete info;
}

//Broadcast a event message, use for notifcation or keep alive
void HAPService::broadcastMessage(void *sender, char *resultData, size_t resultLen) {

	for (auto& conn : _connections) {

		int socketNumber = conn->subSocket;

		if (socketNumber >= 0 && conn->connected && conn->numberOfMsgSend >= 0 && (conn->notify(sender) || sender == NULL)) {

#if HomeKitLog == 1
			printf("Broadcast with sender a %d message to client %d\n", resultLen, socketNumber);
#endif
			std::unique_lock<std::mutex> lock(conn->mutex);

			chacha20_ctx chacha20;    bzero(&chacha20, sizeof(chacha20));

			char temp[64];  bzero(temp, 64); char temp2[64];  bzero(temp2, 64);

			unsigned char *reply = new unsigned char[resultLen + 18];
			reply[0] = resultLen % 256;
			reply[1] = (resultLen - (uint8_t)reply[0]) / 256;

			unsigned long long numberOfMsgSend = conn->numberOfMsgSend;
			if (!is_big_endian()) numberOfMsgSend = bswap_64(conn->numberOfMsgSend);
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
	//For every 10 seconds, this thread will wake, and send a keep alive message
	while (true) {
		printf("Keep Alive");
		BroadcastInfo *alivePackage = new BroadcastInfo;
		alivePackage->sender = nullptr;
		char *aliveMsg = new char[32];
		strncpy(aliveMsg, "{\"characteristics\": []}", 32);
		alivePackage->desc = aliveMsg;
		announce(alivePackage);
		sleep(keepAlivePeriod);
	}
}

void HAPService::connectionLoop(ConnectionInfo* info)
{
	int subSocket = info->subSocket;    
	ssize_t len;
	
	if (subSocket >= 0) {
#if HomeKitLog == 1
		printf("Start Connect: %d\n", subSocket);
#endif

		do {
			len = read(subSocket, info->buffer, 4096);
#if HomeKitLog == 1
			printf("Return len %d for socket %d\n", len, subSocket);
#endif

#if HomeKitReplyHeaderLog == 1
			printf("Message: %s\n", info->buffer);
#endif

			Message msg(info->buffer);
			if (len > 0) {
				if (!strcmp(msg.directory, "pair-setup")) {

					/*
					* The process of pair-setup
					*/

					info->handlePairSetup();

					// Update configuration
					_currentConfiguration++;
					TXTRecordRef txtRecord = buildTXTRecord();
					DNSServiceUpdateRecord(_netService, NULL, 0, TXTRecordGetLength(&txtRecord), TXTRecordGetBytesPtr(&txtRecord), 0);
					TXTRecordDeallocate(&txtRecord);
				}
				else if (!strcmp(msg.directory, "pair-verify")) {
					info->handlePairVerify();
					//When pair-verify done, we handle Accessory Request
					info->handleAccessoryRequest();
				}
				else if (!strcmp(msg.directory, "identify")) {
					close(subSocket);
				}
			}

		} while (len > 0);

		close(subSocket);
#if HomeKitLog == 1
		printf("Stop Connect: %d\n", subSocket);
#endif

		info->subSocket = -1;

		std::remove_if(_connections.begin(), _connections.end(), [info](const ConnectionInfo* &connInfo) {
			return connInfo == info;
		});

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
	TXTRecordSetValue(&txtRecord, "id", 17, deviceIdentity);    //Device id
	char buf[3];
	sprintf(buf, "%d", _currentConfiguration);
	TXTRecordSetValue(&txtRecord, "c#", 1, buf);    //Configuration Number
	TXTRecordSetValue(&txtRecord, "s#", 1, "4");    //Number of service
	if (KeyController::getInstance().hasController()) buf[0] = '0';
	else buf[0] = '1';
	TXTRecordSetValue(&txtRecord, "sf", 1, buf);    //Discoverable: 0 if has been paired
	TXTRecordSetValue(&txtRecord, "ff", 1, "0");    //1 for MFI product
	TXTRecordSetValue(&txtRecord, "md", strlen(deviceName), deviceName);    //Model Name
	int len = sprintf(buf, "%d", _type);
	TXTRecordSetValue(&txtRecord, "ci", len, buf);    //1 for MFI product
	return txtRecord;
}

void HAPService::handleAccessory(const char *request, unsigned int requestLen, char **reply, unsigned int *replyLen, ConnectionInfo *sender) {
#if HomeKitLog == 1
	printf("Receive request: %s\n", request);
#endif
	int index = 5;
	char method[5];
	{
		//Read method
		method[4] = 0;
		bcopy(request, method, 4);
		if (method[3] == ' ') {
			method[3] = 0;
			index = 4;
		}
	}

	char path[1024];
	int i;
	for (i = 0; i < 1024 && request[index] != ' '; i++, index++) {
		path[i] = request[index];
	}
	path[i] = 0;
#if HomeKitLog == 1
	printf("Path: %s\n", path);
#endif

	const char *dataPtr = request;
	while (true) {
		dataPtr = &dataPtr[1];
		if (dataPtr[0] == '\r' && dataPtr[1] == '\n' && dataPtr[2] == '\r' && dataPtr[3] == '\n') break;
	}

	dataPtr += 4;

	char *replyData = NULL;  unsigned short replyDataLen = 0;

	int statusCode = 0;

	const char *protocol = "HTTP/1.1";
	const char *returnType = hapJsonType;

	if (strcmp(path, "/accessories") == 0) {
		//Publish the characterists of the accessories
#if HomeKitLog == 1
		printf("Ask for accessories info\n");
#endif
		statusCode = 200;
		std::string desc = AccessorySet::getInstance().describe(sender);
		replyDataLen = desc.length();
		replyData = new char[replyDataLen + 1];
		bcopy(desc.c_str(), replyData, replyDataLen);
		replyData[replyDataLen] = 0;
	}
	else if (strcmp(path, "/pairings") == 0) {
		Message msg(request);
		statusCode = 200;
#if HomeKitLog == 1
		printf("%d\n", *msg.data.dataPtrForIndex(0));
#endif
		if (*msg.data.dataPtrForIndex(0) == 3) {
			//Pairing with new user

			printf("Add new user\n");

			KeyRecord controllerRec;
			bcopy(msg.data.dataPtrForIndex(3), controllerRec.publicKey, 32);
			bcopy(msg.data.dataPtrForIndex(1), controllerRec.controllerID, 36);
			KeyController::getInstance().addControllerKey(controllerRec);

			MessageDataRecord drec;
			drec.activate = true; drec.data = new char[1]; *drec.data = 2;
			drec.index = 6; drec.length = 1;

			MessageData data;
			data.addRecord(drec);
			data.rawData((const char **)&replyData, &replyDataLen);
			returnType = pairingTlv8Type;
			statusCode = 200;
		}
		else {

			printf("Delete user");

			KeyRecord controllerRec;
			bcopy(msg.data.dataPtrForIndex(1), controllerRec.controllerID, 36);
			KeyController::getInstance().removeControllerKey(controllerRec);

			MessageDataRecord drec;
			drec.activate = true; drec.data = new char[1]; *drec.data = 2;
			drec.index = 6; drec.length = 1;

			MessageData data;
			data.addRecord(drec);
			data.rawData((const char **)&replyData, &replyDataLen);
			returnType = pairingTlv8Type;
			statusCode = 200;
		}
		//Pairing status change, so update
		updatePairable();
	}
	else if (strncmp(path, "/characteristics", 16) == 0) {
		std::unique_lock<std::mutex> lock(AccessorySet::getInstance().accessoryMutex);

		printf("Characteristics\n");
		if (strncmp(method, "GET", 3) == 0) {
			//Read characteristics
			int aid = 0;    int iid = 0;

			char indexBuffer[1000];
			sscanf(path, "/characteristics?id=%[^\n]", indexBuffer);


			printf("Get characteristics %s with len %d\n", indexBuffer, strlen(indexBuffer));

			statusCode = 404;

			std::string result = "[";

			while (strlen(indexBuffer) > 0) {

				printf("Get characteristics %s with len %d\n", indexBuffer, strlen(indexBuffer));

				char temp[1000];
				//Initial the temp
				temp[0] = 0;
				sscanf(indexBuffer, "%d.%d%[^\n]", &aid, &iid, temp);
				printf("Get temp %s with len %d\n", temp, strlen(temp));
				strncpy(indexBuffer, temp, 1000);
				printf("Get characteristics %s with len %d\n", indexBuffer, strlen(indexBuffer));
				//Trim comma
				if (indexBuffer[0] == ',') {
					indexBuffer[0] = '0';
				}

				Accessory *a = AccessorySet::getInstance().accessoryAtIndex(aid);
				if (a != NULL) {
					Characteristics *c = a->characteristicsAtIndex(iid);
					if (c != NULL) {
#if HomeKitLog == 1
						printf("Ask for one characteristics: %d . %d\n", aid, iid);
#endif

						std::string s[3] = { std::to_string(aid), std::to_string(iid), c->value(sender) };
						std::string k[3] = { "aid", "iid", "value" };
						if (result.length() != 1) {
							result += ",";
						}

						std::string _result = dictionaryWrap(k, s, 3);
						result += _result;

					}

				}
			}

			result += "]";

			std::string d = "characteristics";
			result = dictionaryWrap(&d, &result, 1);

			replyDataLen = result.length();
			replyData = new char[replyDataLen + 1];
			replyData[replyDataLen] = 0;
			bcopy(result.c_str(), replyData, replyDataLen);
			statusCode = 200;

		}
		else if (strncmp(method, "PUT", 3) == 0) {
			//Change characteristics
			printf("PUT characteristics: \n");

			char characteristicsBuffer[1000];
			sscanf(dataPtr, "{\"characteristics\":[{%[^]]s}", characteristicsBuffer);

			char *buffer2 = characteristicsBuffer;
			while (strlen(buffer2) && statusCode != 400) {
				bool reachLast = false; bool updateNotify = false;
				char *buffer1;
				buffer1 = strtok_r(buffer2, "}", &buffer2);
				if (*buffer2 != 0) buffer2 += 2;

				int aid = 0;    int iid = 0; char value[16];
				int result = sscanf(buffer1, "\"aid\":%d,\"iid\":%d,\"value\":%s", &aid, &iid, value);
				if (result == 2) {
					sscanf(buffer1, "\"aid\":%d,\"iid\":%d,\"ev\":%s", &aid, &iid, value);
					updateNotify = true;
				}
				else if (result == 0) {
					sscanf(buffer1, "\"remote\":true,\"value\":%[^,],\"aid\":%d,\"iid\":%d", value, &aid, &iid);
					if (result == 2) {
						sscanf(buffer1, "\"remote\":true,\"aid\":%d,\"iid\":%d,\"ev\":%s", &aid, &iid, value);
						updateNotify = true;
					}
					sender->relay = true;
				}
				printf("%d . %d\n", aid, iid);

				Accessory *a = AccessorySet::getInstance().accessoryAtIndex(aid);
				if (a == NULL) {
					statusCode = 400;
				}
				else {
					Characteristics *c = a->characteristicsAtIndex(iid);

					if (updateNotify) {
#if HomeKitLog == 1
						printf("Ask to notify one characteristics: %d . %d -> %s\n", aid, iid, value);
#endif
						if (c == NULL) {
							statusCode = 400;
						}
						else {
							if (c->notifiable()) {
								if (strncmp(value, "1", 1) == 0 || strncmp(value, "true", 4) == 0)
									sender->addNotify(c, aid, iid);
								else
									sender->removeNotify(c);

								statusCode = 204;
							}
							else {
								statusCode = 400;
							}
						}
					}
					else {
#if HomeKitLog == 1
						printf("Ask to change one characteristics: %d . %d -> %s\n", aid, iid, value);
#endif
						if (c == NULL) {
							statusCode = 400;
						}
						else {
							if (c->writable()) {
								c->setValue(value, sender);

								char *broadcastTemp = new char[1024];
								snprintf(broadcastTemp, 1024, "{\"characteristics\":[{%s}]}", buffer1);
								BroadcastInfo * info = new BroadcastInfo;
								info->sender = c;
								info->desc = broadcastTemp;

								std::thread t = std::thread(&HAPService::announce, this, info);
								t.detach();

								statusCode = 204;
							}
							else {
								statusCode = 400;
							}
						}
					}

				}

			}

		}
		else {
			return;
		}

	}
	else {
		//Error
#if HomeKitLog == 1
		printf("Ask for something I don't know\n");
#endif
		printf("%s\n", request);
		printf("%s", path);
		statusCode = 404;
	}

	//Calculate the length of header
	char * tmp = new char[256];
	bzero(tmp, 256);
	int len = snprintf(tmp, 256, "%s %d OK\r\nContent-Type: %s\r\nContent-Length: %u\r\n\r\n", protocol, statusCode, returnType, replyDataLen);
	delete[] tmp;

	//replyLen should omit the '\0'.
	(*replyLen) = len + replyDataLen;
	//reply should add '\0', or the printf is incorrect
	*reply = new char[*replyLen + 1];
	bzero(*reply, *replyLen + 1);
	snprintf(*reply, len + 1, "%s %d OK\r\nContent-Type: %s\r\nContent-Length: %u\r\n\r\n", protocol, statusCode, returnType, replyDataLen);

	if (replyData) {
		bcopy(replyData, &(*reply)[len], replyDataLen);
		delete[] replyData;
	}

#if HomeKitLog == 1 && HomeKitReplyHeaderLog==1
	printf("Reply: %s\n", *reply);
#endif

}