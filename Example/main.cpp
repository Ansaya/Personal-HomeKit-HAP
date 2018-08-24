#include <cstdio>
#include <fstream>
#include <hap/libHAP.h>
#include <memory>
#include <thread>

using namespace hap;

void dimmLightIdentify(bool oldValue, bool newValue, void* info) {
	printf("\nStart Identify Dimmable Light\n\n");
}

void lightIdentify(bool oldValue, bool newValue, void* info) {
	printf("\nStart Identify Light\n\n");
}

void switchIdentify(bool oldValue, bool newValue, void* info) {
	printf("\nStart Identify Switch\n\n");
}

void outletIdentify(bool oldValue, bool newValue, void* info) {
	printf("\nStart Identify Outlet\n\n");
}

void initAccessorySet() {
	printf("Accessories initialization\n");

	//Add Dimmable Light
	Accessory_ptr dimmLightAcc = std::make_shared<Accessory>();
	AccessorySet::getInstance().addAccessory(dimmLightAcc);

	dimmLightAcc->addInfoService("Dimmable light", "Flow3r", "Dimmable Light", "12345678", &dimmLightIdentify);

	StringCharacteristics_ptr dimmLightName;
	BoolCharacteristics_ptr dimmPowerState;
	IntCharacteristics_ptr dimmBrightnessState;
	Service_ptr dimmLightService = dimmLightAcc->addLightBulbService(dimmLightName, dimmPowerState, dimmBrightnessState);

	dimmLightName->Characteristics::setValue("Dimmable light");
	dimmPowerState->Characteristics::setValue("true");
	dimmBrightnessState->setValueChangeCB([dimmLightAcc, dimmLightName, dimmPowerState](int oldValue, int newValue, void* info) {
		if (newValue != 0) {
			printf("Accessory %d changed service '%s' value to %d.\n", 
				dimmLightAcc->getID(), dimmLightName->getValue().c_str(), newValue);
			dimmPowerState->Characteristics::setValue("true");
		}
	});

	//Add Light
	Accessory_ptr lightAcc = std::make_shared<Accessory>();
	AccessorySet::getInstance().addAccessory(lightAcc);

	lightAcc->addInfoService("Light", "Flow3r", "Light", "12345678", &lightIdentify);

	StringCharacteristics_ptr lightName;
	BoolCharacteristics_ptr powerState;
	Service_ptr lightService = lightAcc->addLightBulbService(lightName, powerState);

	lightName->Characteristics::setValue("Light");
	powerState->Characteristics::setValue("true");


	//Add Switch
	Accessory_ptr switchAcc = std::make_shared<Accessory>();
	AccessorySet::getInstance().addAccessory(switchAcc);

	switchAcc->addInfoService("Switch", "Flow3r", "Switch", "12345678", &switchIdentify);

	StringCharacteristics_ptr switchName;
	BoolCharacteristics_ptr switchPower;
	Service_ptr switchService = switchAcc->addSwithService(switchName, switchPower);

	switchName->Characteristics::setValue("Switch");
	switchPower->Characteristics::setValue("false");
};

int main(int argc, const char * argv[]) {

	if (argc > 1) {
		//If there's some argument
		//Currently means reset
		KeyController::getInstance().resetControllerRecord();
	}

	initAccessorySet();

	net::HAPService::getInstance().setupAndListen("LibHAP tester", "123-45-678");

	printf("Press a key to exit...\n");
	getchar();

	net::HAPService::getInstance().stop();

	return 0;
}
