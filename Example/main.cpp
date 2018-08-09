#include <cstdio>
#include <fstream>
#include <hap/libHAP.h>
#include <memory>
#include <thread>

using namespace hap;

void lightIdentify(bool oldValue, bool newValue, void* info) {
	printf("\nStart Identify Light\n\n");
}

void fanIdentify(bool oldValue, bool newValue, void* info) {
	printf("\nStart Identify Fan\n\n");
}

void initAccessorySet() {
	printf("Accessories initialization\n");

	Accessory_ptr lightAcc = std::make_shared<Accessory>();

	//Add Light
	lightAcc->addInfoService("Light 1", "ET", "Light", "12345678", &lightIdentify);
	AccessorySet::getInstance().addAccessory(lightAcc);

	Service_ptr lightService = std::make_shared<Service>(service_lightBulb);
	lightAcc->addService(lightService);

	StringCharacteristics *lightServiceName = new StringCharacteristics(char_serviceName, permission_read, 0);
	lightServiceName->Characteristics::setValue("Light");
	lightService->addCharacteristic(std::shared_ptr<Characteristics>(lightServiceName));

	BoolCharacteristics *powerState = new BoolCharacteristics(char_on, permission_all);
	powerState->Characteristics::setValue("true");
	lightService->addCharacteristic(std::shared_ptr<Characteristics>(powerState));

	IntCharacteristics *brightnessState = new IntCharacteristics(char_brightness, permission_all, 0, 100, 1, unit_percentage);
	brightnessState->Characteristics::setValue("50");
	brightnessState->setValueChangeCB([brightnessState](int oldValue, int newValue, void* info) {
		if (oldValue != newValue) {
			printf("\n\nValue change\n\n");
			brightnessState->Characteristics::setValue(std::to_string(newValue));
		}
	});
	lightService->addCharacteristic(std::shared_ptr<Characteristics>(brightnessState));

	/*std::thread t([brightnessState]() { 
		std::this_thread::sleep_for(std::chrono::seconds(20)); 
		brightnessState->Characteristics::setValue("0");
		printf("\n\nLight Off\n\n");
	});
	t.detach();*/

	//Add fan
	Accessory_ptr fan = std::make_shared<Accessory>();
	fan->addInfoService("Fan 1", "ET", "Fan", "12345678", &fanIdentify);

	Service_ptr fanService = std::make_shared<Service>(service_fan);
	fan->addService(fanService);

	StringCharacteristics *fanServiceName = new StringCharacteristics(char_serviceName, permission_read, 0);
	fanServiceName->Characteristics::setValue("Fan");
	fanService->addCharacteristic(std::shared_ptr<Characteristics>(fanServiceName));

	BoolCharacteristics *fanPower = new BoolCharacteristics(char_on, permission_all);
	fanPower->Characteristics::setValue("true");
	fanService->addCharacteristic(std::shared_ptr<Characteristics>(fanPower));

	AccessorySet::getInstance().addAccessory(fan);
};

int main(int argc, const char * argv[]) {

	if (argc > 1) {
		//If there's some argument
		//Currently means reset
		KeyController::getInstance().resetControllerRecord();
	}

	initAccessorySet();

	net::HAPService::getInstance().setupAndListen(deviceType_bridge);

	printf("Press a key to exit...\n");
	getchar();

	net::HAPService::getInstance().stop();

	return 0;
}
