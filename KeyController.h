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

	KeyController(const KeyController& copy) = delete;

	void resetControllerRecord();

	bool hasController() const;

	void addControllerKey(const KeyRecord& record);

	bool doControllerKeyExist(const KeyRecord& record) const;

	void removeControllerKey(const KeyRecord& record);

	const KeyRecord& getControllerKey(char key[32]) const;


private:
	static KeyController _instance;

	std::vector<KeyRecord> _records;

	KeyController();

};

}

#endif // !HAP_KEY_CONTROLLER
