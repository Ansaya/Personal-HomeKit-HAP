#include <net/ConnectionInfo.h>

#include <AccessorySet.h>
#include <Accessory.h>
#include <Characteristics.h>
#include <hapDefines.h>
#include <KeyController.h>
#include <net/HAPService.h>
#include <net/Message.h>
#include <net/MessageData.h>
#include <net/MessageDataRecord.h>
#include <net/PairStates.h>
#include <net/Response.h>

#include "../Chacha20/chacha20_simple.h"
#include "../curve25519/curve25519-donna.h"
#include "../ed25519-donna/ed25519.h"
#include "../Helpers.h"
#include "../poly1305-opt-master/poly1305.h"
#include "../rfc6234-master/hkdf.h"
#include "../srp/srp.h"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>

#define SOCKET_POLL_COND (POLLIN | POLLHUP | POLLERR)
#define WAKE_FD_POLL_COND (POLLIN | POLLERR)

using namespace hap::net;

static const char hapJsonType[] = "application/hap+json";
static const char pairingTlv8Type[] = "application/pairing+tlv8";

static const uint8_t modulusStr[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC9, 
	0x0F, 0xDA, 0xA2, 0x21, 0x68, 0xC2, 0x34, 0xC4, 0xC6, 0x62, 0x8B, 0x80, 0xDC, 0x1C, 0xD1, 0x29, 
	0x02, 0x4E, 0x08, 0x8A, 0x67, 0xCC, 0x74, 0x02, 0x0B, 0xBE, 0xA6, 0x3B, 0x13, 0x9B, 0x22, 0x51, 
	0x4A, 0x08, 0x79, 0x8E, 0x34, 0x04, 0xDD, 0xEF, 0x95, 0x19, 0xB3, 0xCD, 0x3A, 0x43, 0x1B, 0x30, 
	0x2B, 0x0A, 0x6D, 0xF2, 0x5F, 0x14, 0x37, 0x4F, 0xE1, 0x35, 0x6D, 0x6D, 0x51, 0xC2, 0x45, 0xE4, 
	0x85, 0xB5, 0x76, 0x62, 0x5E, 0x7E, 0xC6, 0xF4, 0x4C, 0x42, 0xE9, 0xA6, 0x37, 0xED, 0x6B, 0x0B, 
	0xFF, 0x5C, 0xB6, 0xF4, 0x06, 0xB7, 0xED, 0xEE, 0x38, 0x6B, 0xFB, 0x5A, 0x89, 0x9F, 0xA5, 0xAE, 
	0x9F, 0x24, 0x11, 0x7C, 0x4B, 0x1F, 0xE6, 0x49, 0x28, 0x66, 0x51, 0xEC, 0xE4, 0x5B, 0x3D, 0xC2, 
	0x00, 0x7C, 0xB8, 0xA1, 0x63, 0xBF, 0x05, 0x98, 0xDA, 0x48, 0x36, 0x1C, 0x55, 0xD3, 0x9A, 0x69, 
	0x16, 0x3F, 0xA8, 0xFD, 0x24, 0xCF, 0x5F, 0x83, 0x65, 0x5D, 0x23, 0xDC, 0xA3, 0xAD, 0x96, 0x1C, 
	0x62, 0xF3, 0x56, 0x20, 0x85, 0x52, 0xBB, 0x9E, 0xD5, 0x29, 0x07, 0x70, 0x96, 0x96, 0x6D, 0x67, 
	0x0C, 0x35, 0x4E, 0x4A, 0xBC, 0x98, 0x04, 0xF1, 0x74, 0x6C, 0x08, 0xCA, 0x18, 0x21, 0x7C, 0x32, 
	0x90, 0x5E, 0x46, 0x2E, 0x36, 0xCE, 0x3B, 0xE3, 0x9E, 0x77, 0x2C, 0x18, 0x0E, 0x86, 0x03, 0x9B, 
	0x27, 0x83, 0xA2, 0xEC, 0x07, 0xA2, 0x8F, 0xB5, 0xC5, 0x5D, 0xF0, 0x6F, 0x4C, 0x52, 0xC9, 0xDE, 
	0x2B, 0xCB, 0xF6, 0x95, 0x58, 0x17, 0x18, 0x39, 0x95, 0x49, 0x7C, 0xEA, 0x95, 0x6A, 0xE5, 0x15, 
	0xD2, 0x26, 0x18, 0x98, 0xFA, 0x05, 0x10, 0x15, 0x72, 0x8E, 0x5A, 0x8A, 0xAA, 0xC4, 0x2D, 0xAD, 
	0x33, 0x17, 0x0D, 0x04, 0x50, 0x7A, 0x33, 0xA8, 0x55, 0x21, 0xAB, 0xDF, 0x1C, 0xBA, 0x64, 0xEC, 
	0xFB, 0x85, 0x04, 0x58, 0xDB, 0xEF, 0x0A, 0x8A, 0xEA, 0x71, 0x57, 0x5D, 0x06, 0x0C, 0x7D, 0xB3, 
	0x97, 0x0F, 0x85, 0xA6, 0xE1, 0xE4, 0xC7, 0xAB, 0xF5, 0xAE, 0x8C, 0xDB, 0x09, 0x33, 0xD7, 0x1E, 
	0x8C, 0x94, 0xE0, 0x4A, 0x25, 0x61, 0x9D, 0xCE, 0xE3, 0xD2, 0x26, 0x1A, 0xD2, 0xEE, 0x6B, 0xF1, 
	0x2F, 0xFA, 0x06, 0xD9, 0x8A, 0x08, 0x64, 0xD8, 0x76, 0x02, 0x73, 0x3E, 0xC8, 0x6A, 0x64, 0x52, 
	0x1F, 0x2B, 0x18, 0x17, 0x7B, 0x20, 0x0C, 0xBB, 0xE1, 0x17, 0x57, 0x7A, 0x61, 0x5D, 0x6C, 0x77, 
	0x09, 0x88, 0xC0, 0xBA, 0xD9, 0x46, 0xE2, 0x08, 0xE2, 0x4F, 0xA0, 0x74, 0xE5, 0xAB, 0x31, 0x43, 
	0xDB, 0x5B, 0xFC, 0xE0, 0xFD, 0x10, 0x8E, 0x4B, 0x82, 0xD1, 0x20, 0xA9, 0x3A, 0xD2, 0xCA, 0xFF, 
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

static const uint8_t curveBasePoint[] = { 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

static const uint8_t generator[1] = { 0x05 };

static const uint8_t accessorySecretKey[32] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC9, 
	0x0F, 0xDA, 0xA2, 0x21, 0x68, 0xC2, 0x34, 0xC4, 0xC6, 0x62, 0x8B, 0x80, 0xDC, 0x1C, 0xD1, 0x29, 
	0x02, 0x4E, 0x08, 0x8A, 0x67, 0xCC, 0x74 };


ConnectionInfo::ConnectionInfo(int socketFD, const std::string& socketName, const std::string& pinCode) 
	: _socketFD(socketFD), _socketName(socketName), _pinCode(pinCode), _connected(true), _verified(false)
{
	// Will be unlocked once the new client has been verified
	_socketWrite.lock();

	int wakePipe[2];
	pipe(wakePipe);

	_wakeFD = wakePipe[1];

	_clientSocket = std::thread(&ConnectionInfo::_clientSocketLoop, this, wakePipe[0]);
}

ConnectionInfo::~ConnectionInfo()
{
	if (_connected.load()) {
		_connected.store(false);
		const char c = 'c';
		int retval = write(_wakeFD, &c, 1);
		if (retval == -1) {
#ifdef HAP_DEBUG
			printf("ConnectionInfo::~ConnectionInfo : error cancelling client "
				"socket thread for client %s.\n", _socketName.c_str());
#endif // HAP_DEBUG
			return;
		}
	}

	_clientSocket.join();
}

const std::string &ConnectionInfo::getSocketName() const
{
	return _socketName;
}

void ConnectionInfo::send(const std::string& data)
{
	if (!_connected.load())
		return;

#ifdef HAP_NET_THREAD_SAFE
	std::unique_lock<std::mutex> writeLock(_socketWrite);
#endif

	std::string encryptedData = encrypt(data, (uint8_t*)&_nonceSend, 
		accessoryToControllerKey, sizeof(accessoryToControllerKey), false);
	_nonceSend++;

	write(_socketFD, &encryptedData.front(), encryptedData.length());
}

void ConnectionInfo::addNotify(const void *target, int aid, int iid)
{
#ifdef HAP_NET_THREAD_SAFE
	std::unique_lock<std::mutex> lock(_notificationsList);
#endif

	_notifications.push_back(target);

#ifdef HAP_DEBUG
	printf("ConnectionInfo::addNotify : add notify %s to %d.%d.\n", identity, aid, iid);
#endif
}

bool ConnectionInfo::isNotified(const void *target)
{
#ifdef HAP_NET_THREAD_SAFE
	std::unique_lock<std::mutex> lock(_notificationsList);
#endif

	for (auto& it : _notifications)
		if (it == target)
			return true;

	return false;
}

void ConnectionInfo::removeNotify(const void *target)
{
#ifdef HAP_NET_THREAD_SAFE
	std::unique_lock<std::mutex> lock(_notificationsList);
#endif

	std::remove_if(_notifications.begin(), _notifications.end(),
		[target](const void * v) { return v == target; });
}

void ConnectionInfo::clearNotify()
{
	_notifications.clear();
}

void ConnectionInfo::_clientSocketLoop(int wakeFD)
{
#ifdef HAP_DEBUG
	printf("ConnectionInfo::_clientSocketLoop : client %s connected.\n", _socketName.c_str());
#endif

	pollfd fds[2];
	memset(fds, 0, sizeof(fds));
	fds[0].fd = _socketFD;
	fds[0].events = SOCKET_POLL_COND;
	fds[1].fd = wakeFD;
	fds[1].events = WAKE_FD_POLL_COND;

	do {
		// Timeout has been reached, go ahead waiting
		if (!poll(fds, 2, 10000))
			continue;

		if (fds[1].revents & WAKE_FD_POLL_COND
			|| fds[0].revents & (POLLHUP | POLLERR)) {
			break;
		}

		ssize_t dataLength = read(_socketFD, _buffer, sizeof(_buffer));

		// If empty data is recived then client is disconnected
		if (dataLength <= 0) {
#ifdef HAP_DEBUG
			printf("ConnectionInfo::_clientSocketLoop : client %s sent empty request.",
				_socketName.c_str());
			if (dataLength < 0)
				printf(" (%s)\n", strerror(errno));
			else
				printf("\n");
#endif
			break;
		}

#ifdef HAP_DEBUG
		printf("ConnectionInfo::_clientSocketLoop : received %zd bytes from client %s.\n", 
			dataLength, _socketName.c_str());
#endif

		std::string responseData;

		if (!_verified) {
#ifdef HAP_NET_DEBUG
			fwrite(_buffer, 1, dataLength, stdout);
			printf("\n");
#endif
			// Request not encrypted
			Message request(_buffer, dataLength);

			Response_ptr response;
			if (request.getPath() == "pair-setup") {
				response = _handlePairSetup(request);
			}
			else if (request.getPath() == "pair-verify") {
				response = _handlePairVerify(request);
			}
			else if (request.getPath() == "identity") {
				break;
			}

			if (response != nullptr) {
				responseData = response->getResponse();
#ifdef HAP_NET_DEBUG
				printf("ConnectionInfo::_clientSocketLoop : crafted response for client %s :\n", 
					_socketName.c_str());
				fwrite(&responseData.front(), 1, responseData.length(), stdout);
				printf("\n");
#endif
			}
		}
		else {
			// Request encrypted

			// Decrypt request
			std::string data = decrypt(_buffer, dataLength, (uint8_t*)&_nonceReceive, 
				controllerToAccessoryKey, sizeof(controllerToAccessoryKey), false);
			_nonceReceive++;

			if (!data.empty()) {
#ifdef HAP_DEBUG
				printf("ConnectionInfo::_clientSocketLoop : data verified succesfully.\n");
#ifdef HAP_NET_DEBUG
				fwrite(&data.front(), 1, data.size(), stdout);
				printf("\n");
#endif
#endif
			}
			else {

#ifdef HAP_DEBUG
				printf("ConnectionInfo::_clientSocketLoop : passed-in data is not verified!\n");
#endif
				memset(_buffer, 0, sizeof(_buffer));
				continue;
			}

			//Output return
			std::string responseContent = _handleAccessory(data);

#ifdef HAP_NET_DEBUG
			printf("ConnectionInfo::_clientSocketLoop : crafted response for client %s :\n",
				_socketName.c_str());
			fwrite(&responseContent.front(), 1, responseContent.length(), stdout);
			printf("\n");
#endif

			_socketWrite.lock();

			responseData = encrypt(responseContent, (uint8_t*)&_nonceSend, 
				accessoryToControllerKey, sizeof(accessoryToControllerKey), false);
			_nonceSend++;
		}

		if (responseData.empty()) {
#ifdef HAP_DEBUG
			printf("ConnectionInfo::_clientSocketLoop : error : empty response for "
				"client %s.\n", _socketName.c_str());
#endif
			memset(_buffer, 0, sizeof(_buffer));
			continue;
		}

		ssize_t writtenData = write(_socketFD, &responseData.front(), responseData.length());
#ifdef HAP_DEBUG
		printf("ConnectionInfo::_clientSocketLoop : sent %d bytes "
			"to client %s\n", writtenData, _socketName.c_str());
#endif

		if (_verified.load())
			_socketWrite.unlock();

		memset(_buffer, 0, sizeof(_buffer));
	} while (_connected.load());

	_connected.store(false);

	shutdown(_socketFD, SHUT_RDWR);
	close(wakeFD);

#ifdef HAP_DEBUG
	printf("ConnectionInfo::_clientSocketLoop : client %s disconnected.\n", _socketName.c_str());
#endif
}

Response_ptr ConnectionInfo::_handlePairSetup(Message& request)
{
	static thread_local SRP *srp = SRP_new(SRP6a_server_method());
	static thread_local cstr* secretKey = NULL;
	static thread_local uint8_t sessionKey[64];

	PairSetupState state = (PairSetupState)
		request.getContent()->getRecordForIndex(6)->getData().front();

	MessageDataRecord_ptr stateRecord =
		std::make_shared<MessageDataRecord>(6, std::string(1, (char)(state + 1)), true);

	Response_ptr response;

	MessageData_ptr content = std::make_shared<MessageData>();
	content->addRecord(stateRecord);

	switch (state)
	{
	case State_M1_SRPStartRequest:
	{
#ifdef HAP_DEBUG
		printf("ConnectionInfo::handlePairSetup : State_M1_SRPStartRequest\n");
#endif
		std::string salt(16, 0);
		for (size_t i = 0; i < salt.length(); i++) {
			salt[i] = rand();
		}

		SRP_set_username(srp, "Pair-Setup");
		int modulusSize = sizeof(modulusStr) / sizeof(modulusStr[0]);
		SRP_set_params(srp, modulusStr, modulusSize,
			generator, sizeof(generator), (uint8_t*)&salt.front(), salt.length());
		SRP_set_auth_password(srp, _pinCode.c_str());
		cstr* publicKey = NULL;
		SRP_gen_pub(srp, &publicKey);

		MessageDataRecord_ptr saltRec = std::make_shared<MessageDataRecord>(2, salt, true);
		MessageDataRecord_ptr publicKeyRec = std::make_shared<MessageDataRecord>(3,
			std::string(publicKey->data, publicKey->length), true);
		cstr_free(publicKey);

		content->addRecord(saltRec);
		content->addRecord(publicKeyRec);

		response = std::make_shared<Response>(200, content);
	}
	break;

	case State_M3_SRPVerifyRequest:
	{
#ifdef HAP_DEBUG
		printf("ConnectionInfo::handlePairSetup : State_M3_SRPVerifyRequest\n");
#endif
		std::string key;
		ConstMessageDataRecord_ptr record3 = request.getContent()->getRecordForIndex(3);
		if (record3 != nullptr) {
			key = record3->getData();
		}
		std::string proof;
		ConstMessageDataRecord_ptr record4 = request.getContent()->getRecordForIndex(4);
		if (record4 != nullptr) {
			proof = record4->getData();
		}

		SRP_RESULT result = SRP_compute_key(srp, &secretKey, (uint8_t*)&key.front(), key.length());
		result = SRP_verify(srp, (uint8_t*)&proof.front(), proof.length());

		if (!SRP_OK(result)) {
			MessageDataRecord_ptr error =
				std::make_shared<MessageDataRecord>(7, std::string(1, (char)2), true);

			content->addRecord(error);
#ifdef HAP_DEBUG
			printf("ConnectionInfo::handlePairSetup : error at M3.\n");
#endif
		}
		else {
			cstr* response = NULL;
			SRP_respond(srp, &response);
			MessageDataRecord_ptr responseRecord =
				std::make_shared<MessageDataRecord>(4, std::string(response->data, response->length), true);
			cstr_free(response);

			content->addRecord(responseRecord);
#ifdef HAP_DEBUG
			printf("ConnectionInfo::handlePairSetup : password correct.\n");
#endif
		}
		SRP_free(srp);

		const char salt[] = "Pair-Setup-Encrypt-Salt";
		const char info[] = "Pair-Setup-Encrypt-Info";
		int i = hkdf((uint8_t*)salt, strlen(salt),
			(uint8_t*)secretKey->data, secretKey->length,
			(uint8_t*)info, strlen(info), (uint8_t*)sessionKey, 32);
		if (i != 0)
			break;

		response = std::make_shared<Response>(200, content);
	}
	break;

	case State_M5_ExchangeRequest:
	{
#ifdef HAP_DEBUG
		printf("ConnectionInfo::handlePairSetup : State_M5_ExchangeRequest\n");
#endif
		std::string encryptedRequest =
			request.getContent()->getRecordForIndex(5)->getData();

		std::string decryptedData = decrypt(&encryptedRequest.front(), 
			encryptedRequest.length(), (uint8_t*)"PS-Msg05", sessionKey, 32, true);

		if (decryptedData.empty()) {
			MessageDataRecord_ptr responseRecord =
				std::make_shared<MessageDataRecord>(7, std::string(1, (char)1), true);

			content->addRecord(responseRecord);

#ifdef HAP_DEBUG
			printf("ConnectionInfo::handlePairSetup : corrupt TLv8 at M5\n");
#endif
		}
		else {
			/*
			* HAK Pair Setup M6
			*/
			MessageData subTLV8(decryptedData);
			std::string controllerIdentifier = subTLV8.getRecordForIndex(1)->getData();
			std::string controllerPublicKey = subTLV8.getRecordForIndex(3)->getData();
			std::string controllerSignature = subTLV8.getRecordForIndex(10)->getData();

			char controllerHash[100];

			KeyRecord newRecord;
			memset(&newRecord, 0, sizeof(newRecord));
			bcopy(&controllerIdentifier.front(), newRecord.controllerID, 36);
			bcopy(&controllerPublicKey.front(), newRecord.publicKey, 32);
			KeyController::getInstance().addControllerKey(newRecord);

			const char salt[] = "Pair-Setup-Controller-Sign-Salt";
			const char info[] = "Pair-Setup-Controller-Sign-Info";
			int i = hkdf((uint8_t*)salt, strlen(salt),
				(uint8_t*)secretKey->data, secretKey->length,
				(uint8_t*)info, strlen(info), (uint8_t*)controllerHash, 32);
			if (i != 0) {
#ifdef HAP_DEBUG
				printf("ConnectionInfo::_handlePairSetup : hkdf verification falied.\n");
#endif
				break;
			}

			bcopy(&controllerIdentifier.front(), &controllerHash[32], 36);
			bcopy(&controllerPublicKey.front(), &controllerHash[68], 32);

			int ed25519_err = ed25519_sign_open((uint8_t*)controllerHash, 100,
				(uint8_t*)&controllerPublicKey.front(), (uint8_t*)&controllerSignature.front());

			if (ed25519_err) {
#ifdef HAP_DEBUG
				printf("ConnectionInfo::_handlePairSetup : ed25519 verification falied.\n");
#endif
				break;
			}
			else {
				MessageData returnTLV8;

				/*
				* Generate Signature
				*/
				const char salt[] = "Pair-Setup-Accessory-Sign-Salt";
				const char info[] = "Pair-Setup-Accessory-Sign-Info";
				uint8_t output[150];
				hkdf((uint8_t*)salt, strlen(salt),
					(uint8_t*)secretKey->data, secretKey->length,
					(uint8_t*)info, strlen(info), output, 32);
				cstr_free(secretKey);
				bcopy(HAP_DEVICE_MAC, &output[32], strlen(HAP_DEVICE_MAC));

				char *signature = new char[64];
				ed25519_secret_key edSecret;
				bcopy(accessorySecretKey, edSecret, sizeof(edSecret));
				ed25519_public_key edPubKey;
				ed25519_publickey(edSecret, edPubKey);

				bcopy(edPubKey, &output[32 + strlen(HAP_DEVICE_MAC)], 32);
				ed25519_sign(output, 64 + strlen(HAP_DEVICE_MAC), (uint8_t*)edSecret, (uint8_t*)edPubKey, (uint8_t*)signature);

				MessageDataRecord_ptr signatureRecord =
					std::make_shared<MessageDataRecord>(10, std::string(signature, 64), true);

				MessageDataRecord_ptr publicKeyRecord =
					std::make_shared<MessageDataRecord>(3, std::string((char*)edPubKey, 32), true);

				MessageDataRecord_ptr usernameRecord =
					std::make_shared<MessageDataRecord>(1, HAP_DEVICE_MAC, true);

				returnTLV8.addRecord(signatureRecord);
				returnTLV8.addRecord(publicKeyRecord);
				returnTLV8.addRecord(usernameRecord);

				std::string tlv8Data = returnTLV8.rawData();
				std::string tlv8RecordData = encrypt(tlv8Data, (uint8_t *)"PS-Msg06", sessionKey, 32, true);

				MessageDataRecord_ptr tlv8Record =
					std::make_shared<MessageDataRecord>(5, tlv8RecordData, true);

				content->addRecord(tlv8Record);
			}

		}

		response = std::make_shared<Response>(200, content);

		// Update configuration for new connected client
		HAPService::getInstance()._currentConfiguration++;
		HAPService::getInstance().updatePairable();
	}
	break;

	default:
		break;
	}

	return response;
}

Response_ptr ConnectionInfo::_handlePairVerify(Message& request)
{
#ifdef HAP_DEBUG
	printf("ConnectionInfo::handlePairVerify : start pair verify.\n");
#endif

	static thread_local curved25519_key publicKey;
	static thread_local curved25519_key controllerPublicKey;
	static thread_local curved25519_key sharedKey;

	static thread_local uint8_t enKey[32];

	PairVerifyState state = (PairVerifyState)
		request.getContent()->getRecordForIndex(6)->getData().front();

	MessageDataRecord_ptr stateRecord =
		std::make_shared<MessageDataRecord>(6, std::string(1, (char)(state + 1)), true);

	Response_ptr response = nullptr;

	MessageData_ptr content = std::make_shared<MessageData>();
	content->addRecord(stateRecord);

	switch (state) {
	case State_Pair_Verify_M1:
	{
#ifdef HAP_DEBUG
		printf("ConnectionInfo::handlePairVerify : M1\n");
#endif

		bcopy(&request.getContent()->getRecordForIndex(3)->getData().front(),
			controllerPublicKey, 32);

		curved25519_key secretKey;
		for (size_t i = 0; i < sizeof(secretKey); i++) {
			secretKey[i] = rand();
		}
		curve25519_donna((uint8_t*)publicKey, (uint8_t*)secretKey,
			(uint8_t*)curveBasePoint);

		curve25519_donna(sharedKey, secretKey, controllerPublicKey);

		char *temp = new char[100];
		bcopy(publicKey, temp, 32);
		bcopy(HAP_DEVICE_MAC, &temp[32], strlen(HAP_DEVICE_MAC));
		bcopy(controllerPublicKey, &temp[49], 32);

		ed25519_secret_key edSecret;
		bcopy(accessorySecretKey, edSecret, sizeof(edSecret));
		ed25519_public_key edPubKey;
		ed25519_publickey(edSecret, edPubKey);

		std::string signRecordData(64, 0);

		ed25519_sign((uint8_t*)temp, 64 + strlen(HAP_DEVICE_MAC), edSecret, edPubKey,
			(uint8_t*)&signRecordData.front());
		delete[] temp;

		MessageDataRecord_ptr pubKeyRecord =
			std::make_shared<MessageDataRecord>(3, std::string((char*)publicKey, 32), true);

		content->addRecord(pubKeyRecord);

		MessageData data;

		MessageDataRecord_ptr signRecord =
			std::make_shared<MessageDataRecord>(10, signRecordData, true);

		MessageDataRecord_ptr idRecord =
			std::make_shared<MessageDataRecord>(1, HAP_DEVICE_MAC, true);

		data.addRecord(signRecord);
		data.addRecord(idRecord);

		const uint8_t salt[] = "Pair-Verify-Encrypt-Salt";
		const uint8_t info[] = "Pair-Verify-Encrypt-Info";

		hkdf(salt, 24, sharedKey, 32, info, 24, enKey, 32);

		std::string plainMsg = data.rawData();
		std::string encryptMsg = encrypt(plainMsg, (uint8_t*)"PV-Msg02", enKey, 32, true);

		MessageDataRecord_ptr encryptRecord =
			std::make_shared<MessageDataRecord>(5, encryptMsg, true);

		content->addRecord(encryptRecord);

		response = std::make_shared<Response>(200, content);
	}
	break;
	case State_Pair_Verify_M3:
	{
#ifdef HAP_DEBUG
		printf("ConnectionInfo::handlePairVerify : M3\n");
#endif
		std::string encryptedData = request.getContent()->getRecordForIndex(5)->getData();

		std::string decryptData = decrypt(&encryptedData.front(), 
			encryptedData.length(), (uint8_t*)"PV-Msg03", enKey, 32, true);

		if (!encryptedData.empty()) {
			MessageData data(decryptData);

			KeyRecord rec = KeyController::getInstance().getControllerKey(data.getRecordForIndex(1)->getData().c_str());

			uint8_t tempMsg[100];
			bcopy(controllerPublicKey, tempMsg, 32);
			bcopy(data.getRecordForIndex(1)->getData().c_str(), &tempMsg[32], 36);
			bcopy(data.getRecordForIndex(1)->getData().c_str(), identity, 36);
			bcopy(publicKey, &tempMsg[68], 32);

			int err = ed25519_sign_open(tempMsg, 100, (uint8_t*)rec.publicKey, (uint8_t*)data.getRecordForIndex(10)->getData().c_str());

			if (err == 0) {

				hkdf((uint8_t *)"Control-Salt", 12, sharedKey, 32, (uint8_t *)"Control-Read-Encryption-Key", 27, accessoryToControllerKey, 32);
				hkdf((uint8_t *)"Control-Salt", 12, sharedKey, 32, (uint8_t *)"Control-Write-Encryption-Key", 28, controllerToAccessoryKey, 32);

#ifdef HAP_DEBUG
				printf("ConnectionInfo::_handlePairVerify : pair verified succesfully.\n");
#endif
				response = std::make_shared<Response>(200, content);

				_verified.store(true);
			}
			else {
				MessageDataRecord_ptr errorRecord =
					std::make_shared<MessageDataRecord>(7, std::string(1, (char)2), true);

				content->addRecord(errorRecord);

				response = std::make_shared<Response>(200, content);

#ifdef HAP_DEBUG
				printf("ConnectionInfo::_handlePairVerify : pair verification failed.\n");
#endif
			}
		}
	}
	break;

	}

	return response;
}

std::string ConnectionInfo::_handleAccessory(const std::string& request)
{
#ifdef HAP_NET_DEBUG
	printf("ConnectionInfo::_handleAccessory : received request: \n%s\n", request.c_str());
#endif

	// Get HTTP request method
	size_t methodEnd = request.find(' ');
	size_t pathEnd = request.find(' ', methodEnd + 1);
	size_t contentBegin = request.find("\r\n\r\n") + 4;

	std::string method = request.substr(0, methodEnd);
	std::string path = request.substr(methodEnd + 2, pathEnd - methodEnd - 2);
	std::string content = request.substr(contentBegin);

#ifdef HAP_NET_DEBUG
	printf("ConnectionInfo::_handleAccessory : parsed request : \n"
		"\tMethod: %s\n\tPath: %s\n\tContent: %s\n", 
		method.c_str(), path.c_str(), content.c_str());
#endif

	int statusCode = 0;
	std::string responseContent;
	const char *returnType = hapJsonType;

	if (path == "accessories") {
		//Publish the characterists of the accessories
#ifdef HAP_DEBUG
		printf("ConnectionInfo::_handleAccessory : accessories info requested.\n");
#endif
		statusCode = 200;
		responseContent = AccessorySet::getInstance().describe();
	}
	else if (path == "pairings") {

		Message msg(request.c_str(), request.length());
		statusCode = 200;

		uint8_t pairState = (uint8_t)msg.getContent()->getRecordForIndex(0)->getData().front();

#ifdef HAP_DEBUG
		printf("ConnectionInfo::_handleAccessory : pairings : %d\n", pairState);
#endif

		if (pairState == 3) {
			//Pairing with new user

#ifdef HAP_DEBUG
			printf("ConnectionInfo::_handleAccessory : new user pairing\n");
#endif

			KeyRecord controllerRec;
			bcopy(&msg.getContent()->getRecordForIndex(3)->getData().front(), controllerRec.publicKey, 32);
			bcopy(&msg.getContent()->getRecordForIndex(1)->getData().front(), controllerRec.controllerID, 36);
			KeyController::getInstance().addControllerKey(controllerRec);
		}
		else {

#ifdef HAP_DEBUG
			printf("ConnectionInfo::_handleAccessory : delete user pairing.\n");
#endif

			KeyRecord controllerRec;
			bcopy(&msg.getContent()->getRecordForIndex(1)->getData().front(), controllerRec.controllerID, 36);
			KeyController::getInstance().removeControllerKey(controllerRec);
		}

		statusCode = 200;
		returnType = pairingTlv8Type;
		MessageDataRecord_ptr drec =
			std::make_shared<MessageDataRecord>(6, std::string(1, (char)2), true);

		MessageData data;
		data.addRecord(drec);
		responseContent = data.rawData();

		//Pairing status change, so update
		HAPService::getInstance().updatePairable();
	}
	else if (strncmp(path.c_str(), "characteristics", 15) == 0) {
#ifdef HAP_DEBUG
		printf("ConnectionInfo::_handleAccessory : characteristics action required.\n");
#endif

		if (method == "GET") {
			//Read characteristics
			int aid = 0;
			int iid = 0;

			char indexBuffer[1000];
			sscanf(path.c_str(), "characteristics?id=%[^\n]", indexBuffer);

			statusCode = 404;

			responseContent = "[";

			while (strlen(indexBuffer) > 0) {

				char temp[1000];
				//Initial the temp
				temp[0] = 0;
				sscanf(indexBuffer, "%d.%d%[^\n]", &aid, &iid, temp);

				strncpy(indexBuffer, temp, 1000);

				//Trim comma
				if (indexBuffer[0] == ',') {
					indexBuffer[0] = '0';
				}

				Accessory_ptr a = AccessorySet::getInstance().getAccessory(aid);
				if (a != nullptr) {
					Characteristics_ptr c = a->getCharacteristic(iid);
					if (c != nullptr) {
#ifdef HAP_DEBUG
						printf("ConnectionInfo::_handleAccessory : characteristics : get %d.%d\n", aid, iid);
#endif

						std::string s[3] = { std::to_string(aid), std::to_string(iid), c->getValue() };
						std::string k[3] = { "aid", "iid", "value" };
						if (responseContent.length() != 1) {
							responseContent += ",";
						}

						std::string _result = dictionaryWrap(k, s, 3);
						responseContent += _result;
					}

				}
			}

			responseContent += "]";

			std::string d = "characteristics";
			responseContent = dictionaryWrap(&d, &responseContent, 1);

			statusCode = 200;
		}
		else if (method == "PUT") {
			//Change characteristics
			char characteristicsBuffer[1000];
			sscanf(&content.front(), "{\"characteristics\":[{%[^]]s}", characteristicsBuffer);

			char *buffer2 = characteristicsBuffer;
			while (strlen(buffer2) && statusCode != 400) {
				bool updateNotify = false;
				char *buffer1;
				buffer1 = strtok_r(buffer2, "}", &buffer2);
				if (*buffer2 != 0) buffer2 += 2;

				int aid = 0;
				int iid = 0;
				char value[16];
				int result = sscanf(buffer1, "\"aid\":%d,\"iid\":%d,\"value\":%s", &aid, &iid, value);
				if (result == 2) {
					sscanf(buffer1, "\"aid\":%d,\"iid\":%d,\"ev\":%s", &aid, &iid, value);
					updateNotify = true;
				}
				else if (result == 0) {
					result = sscanf(buffer1, "\"remote\":true,\"value\":%[^,],\"aid\":%d,\"iid\":%d", value, &aid, &iid);
					if (result == 2) {
						sscanf(buffer1, "\"remote\":true,\"aid\":%d,\"iid\":%d,\"ev\":%s", &aid, &iid, value);
						updateNotify = true;
					}
				}

#ifdef HAP_DEBUG
				printf("ConnectionInfo::_handleAccessory : characteristics : change %d.%d \n", aid, iid);
#endif

				Accessory_ptr a = AccessorySet::getInstance().getAccessory(aid);
				if (a == nullptr) {
					statusCode = 400;
#ifdef HAP_DEBUG
					printf("ConnectionInfo::_handleAccessory : characteristics : error : accessory %d not found.\n", aid);
#endif
				}
				else {
					Characteristics_ptr c = a->getCharacteristic(iid);

					if (c == nullptr) {
						statusCode = 400;
#ifdef HAP_DEBUG
						printf("ConnectionInfo::_handleAccessory : characteristics : error : %d.%d not found.\n", aid, iid);
#endif
					}
					else {
						if (updateNotify) {
#ifdef HAP_DEBUG
							printf("ConnectionInfo::_handleAccessory : characteristics : notify %d.%d -> %s\n", aid, iid, value);
#endif
							if (c->notifiable()) {
								if (strncmp(value, "1", 1) == 0 || strncmp(value, "true", 4) == 0)
									addNotify(c.get(), aid, iid);
								else
									removeNotify(c.get());

								statusCode = 204;
							}
							else {
								statusCode = 400;
							}
						}
						else {
#ifdef HAP_DEBUG
							printf("ConnectionInfo::_handleAccessory : characteristics : change %d.%d -> %s\n", aid, iid, value);
#endif
							if (c->writable()) {
								c->setValue(value, this);

								BroadcastInfo_ptr info = std::make_shared<BroadcastInfo>();
								info->sender = c.get();
								info->desc = "{\"characteristics\":[{";
								info->desc += buffer1;
								info->desc += "}]}";

								HAPService::getInstance().announce(info);

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
			statusCode = 405;
		}

	}
	else {
		//Error
#ifdef HAP_DEBUG
		printf("ConnectionInfo::_handleAccessory : error: unknown request\n"
			"\tMethod: %s\n\tPath: %s\n\tContent: %s\n",
			method.c_str(), path.c_str(), content.c_str());
#endif

		statusCode = 404;
	}

	std::string response;
	response += "HTTP/1.1 ";
	response += std::to_string(statusCode);
	//if (statusCode == 200) {
		response += " OK";
	//}
	response += "\r\nContent-Type: ";
	response += returnType;
	response += "\r\nContent-Length: ";
	response += std::to_string(responseContent.length());
	response += "\r\n\r\n";
	response += responseContent;

	return response;
}

std::string ConnectionInfo::encrypt(
	const std::string& data,
	uint8_t nonce[8],
	const uint8_t* key,
	uint8_t keyLength,
	bool dataOnly) const
{
	size_t encryptedDataLength = data.length() + 16;
	if (!dataOnly)
		encryptedDataLength += 2;
	std::string encryptedData(encryptedDataLength, 0);

	chacha20_ctx ctx;
	chacha20_setup(&ctx, key, keyLength, nonce);

	uint8_t buffer[64] = { 0 }, polyKey[64];
	chacha20_encrypt(&ctx, buffer, polyKey, 64);

	if (dataOnly) {
		chacha20_encrypt(&ctx, (uint8_t *)&data.front(), (uint8_t *)&encryptedData.front(), data.length());

		Poly1305_GenKey(polyKey, (uint8_t*)&encryptedData.front(), data.length(),
			false, (uint8_t*)&encryptedData[data.length()]);
	}
	else {
		*((uint16_t*)&encryptedData.front()) = data.length();

		chacha20_encrypt(&ctx, (uint8_t *)&data.front(), (uint8_t *)&encryptedData[2], data.length());

		Poly1305_GenKey(polyKey, (uint8_t*)&encryptedData.front(), data.length(),
			true, (uint8_t*)&encryptedData[2 + data.length()]);
	}

	return encryptedData;
}

std::string ConnectionInfo::decrypt(
	const char* encryptedData,
	uint16_t encryptedDataLength,
	uint8_t nonce[8],
	const uint8_t* key,
	uint8_t keyLength,
	bool dataOnly) const
{
	// Decrypt request
	uint16_t dataLength = (dataOnly ? (encryptedDataLength - 16) : *((uint16_t*)encryptedData));
	if (dataLength < (dataOnly ? 17 : 19))
		return "";

	std::string data(dataLength, 0);

	chacha20_ctx chacha20;
	memset(&chacha20, 0, sizeof(chacha20));

	chacha20_setup(&chacha20, key, keyLength, nonce);

	uint8_t zero[64] = { 0 };
	uint8_t polyKey[sizeof(zero)] = { 0 };
	chacha20_encrypt(&chacha20, zero, polyKey, sizeof(zero));

	//Ploy1305 key
	uint8_t verify[16] = { 0 };
	Poly1305_GenKey(polyKey, (uint8_t*)encryptedData, dataLength, !dataOnly, verify);

	if (memcmp((void *)verify, (void *)&encryptedData[dataOnly ? dataLength : 2 + dataLength], 16) == 0) {
		chacha20_encrypt(&chacha20, (uint8_t*)&encryptedData[dataOnly ? 0 : 2], (uint8_t *)&data.front(), dataLength);

		return data;
	}

	return "";
}

// 2ByteS(datalen) + data_buf + 16Bytes(Verfied Key)
// Type_Data_Without_Length
//Passed-in buf is OMIT len and 16Bytes verfied key
// Type_Data_With_Length
//Passed-in buf is len and data_buf
void ConnectionInfo::Poly1305_GenKey(const uint8_t * key, uint8_t * buf, uint16_t len, bool dataWithLength, uint8_t* verify) const
{
    if (key == NULL || buf == NULL || len < 2 || verify == NULL)
        return;
    
    poly1305_context verifyContext; bzero(&verifyContext, sizeof(verifyContext));
    poly1305_init(&verifyContext, key);
    
	uint8_t waste[16] = { 0 };
    
    if (dataWithLength) {
        poly1305_update(&verifyContext, &buf[0], 1);
        poly1305_update(&verifyContext, &buf[1], 1);
        poly1305_update(&verifyContext, waste, 14);
        
        poly1305_update(&verifyContext, &buf[2], len);
    }
    else {
        poly1305_update(&verifyContext, buf, len);
    }
    
    if (len%16 > 0)
        poly1305_update(&verifyContext, waste, 16-(len%16));

   uint8_t _len;
    if (dataWithLength) {
        _len = 2;
    }
    else {
        _len = 0;
    }
    
    poly1305_update(&verifyContext, &_len, 1);
    poly1305_update(&verifyContext, (const unsigned char *)&waste, 7);
    _len = len;
    
    poly1305_update(&verifyContext, &_len, 1);
    _len = len/256;
    poly1305_update(&verifyContext, &_len, 1);
    
    poly1305_update(&verifyContext, (const unsigned char *)&waste, 6);
    
    poly1305_finish(&verifyContext, verify);
}