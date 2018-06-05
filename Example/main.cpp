#include <cstdio>
#include <fstream>
#include <hap/libHAP.h>
#include <memory>
#include <thread>

using namespace hap;

void lightIdentify(bool oldValue, bool newValue, net::ConnectionInfo* info) {
	printf("Start Identify Light\n");
}

void fanIdentify(bool oldValue, bool newValue, net::ConnectionInfo* info) {
	printf("Start Identify Fan\n");
}

void initAccessorySet() {
	net::HAPService::getInstance().setup(deviceType_bridge);

	printf("Initial Accessory\n");

	Accessory_ptr lightAcc = std::make_shared<Accessory>();

	//Add Light
	lightAcc->addInfoService("Light 1", "ET", "Light", "12345678", &lightIdentify);
	AccessorySet::getInstance().addAccessory(lightAcc);

	Service *lightService = new Service(service_lightBulb);
	lightAcc->addService(lightService);

	StringCharacteristics *lightServiceName = new StringCharacteristics(char_serviceName, permission_read, 0);
	lightServiceName->setValue("Light", NULL);
	lightAcc->addCharacteristics(lightService, lightServiceName);

	BoolCharacteristics *powerState = new BoolCharacteristics(char_on, permission_all);
	powerState->setValue("true", NULL);
	lightAcc->addCharacteristics(lightService, powerState);

	IntCharacteristics *brightnessState = new IntCharacteristics(char_brightness, permission_all, 0, 100, 1, unit_percentage);
	brightnessState->setValue("50", NULL);
	brightnessState->valueChangeFunctionCall = [brightnessState](int oldValue, int newValue, net::ConnectionInfo* info) {
		if (oldValue != newValue) {
			printf("\n\nValue change\n\n");
			brightnessState->Characteristics::setValue(std::to_string(newValue));
		}
	};
	lightAcc->addCharacteristics(lightService, brightnessState);

	std::thread t([brightnessState]() { 
		std::this_thread::sleep_for(std::chrono::seconds(20)); 
		brightnessState->Characteristics::setValue("0"); 
		brightnessState->notify();
		printf("\n\nLight Off\n\n");
	});
	t.detach();

	//Add fan
	Accessory_ptr fan = std::make_shared<Accessory>();
	fan->addInfoService("Fan 1", "ET", "Fan", "12345678", &fanIdentify);
	AccessorySet::getInstance().addAccessory(fan);

	Service *fanService = new Service(service_fan);
	fan->addService(fanService);

	StringCharacteristics *fanServiceName = new StringCharacteristics(char_serviceName, permission_read, 0);
	fanServiceName->setValue("Fan", nullptr);
	fan->addCharacteristics(fanService, fanServiceName);

	BoolCharacteristics *fanPower = new BoolCharacteristics(char_on, permission_all);
	fanPower->setValue("true", nullptr);
	fan->addCharacteristics(fanService, fanPower);
};

int main(int argc, const char * argv[]) {

	if (argc > 1) {
		//If there's some argument
		//Currently means reset
		KeyController::getInstance().resetControllerRecord();
	}

	initAccessorySet();

	do {
		net::HAPService::getInstance().handleConnection();
	} while (true);

	return 0;
}
