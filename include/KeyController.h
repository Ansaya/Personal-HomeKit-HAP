#ifndef HAP_KEY_CONTROLLER
#define HAP_KEY_CONTROLLER

#include <mutex>
#include <string>
#include <vector>

namespace hap {

struct KeyRecord {
    char controllerID[36];
    char publicKey[32];
};

class KeyController {

public:
	static KeyController &getInstance();

	/**
	 *	@brief Change current key record file path
	 *
	 *	@note Old file with saved client keys is copied to the new location without any data loss
	 *
	 *	@param filePath new file path for key record file
	 *
	 *	@return True if new file is created succesfully, false else
	 */
	bool setKeyRecordPath(const std::string& filePath);

	void resetControllerRecord();

	bool hasController() const;

	void addControllerKey(const KeyRecord& record);

	bool doControllerKeyExist(const KeyRecord& record) const;

	void removeControllerKey(const KeyRecord& record);

	const KeyRecord getControllerKey(const char key[32]) const;


private:
	std::mutex _file;
	std::string _filePath;
	std::vector<KeyRecord> _records;

	KeyController();
	KeyController(const KeyController&) = delete;
	void operator=(const KeyController&) = delete;

	void _writeBack();

};

}

#endif // !HAP_KEY_CONTROLLER
