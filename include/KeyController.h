#ifndef HAP_KEY_CONTROLLER
#define HAP_KEY_CONTROLLER

#include <vector>

namespace hap {

struct KeyRecord {
    char controllerID[36];
    char publicKey[32];
};

class KeyController {

public:
	static KeyController &getInstance();

	void resetControllerRecord();

	bool hasController() const;

	void addControllerKey(const KeyRecord& record);

	bool doControllerKeyExist(const KeyRecord& record) const;

	void removeControllerKey(const KeyRecord& record);

	const KeyRecord getControllerKey(const char key[32]) const;


private:
	std::vector<KeyRecord> _records;

	KeyController();
	KeyController(const KeyController&) = delete;
	void operator=(const KeyController&) = delete;

};

}

#endif // !HAP_KEY_CONTROLLER
