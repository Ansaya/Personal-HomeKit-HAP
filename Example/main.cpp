#include <cstdio>
#include <fstream>
#include <libHAP.h>
#include <memory>
#include <thread>

using namespace hap;

void showcaseInitializer() {
	printf("Apple HomeKit accessory showcase initialization...\n");

	//Add Lightbulb
	printf("Adding lightbulb accessory...\n");
	Accessory_ptr lightAcc = std::make_shared<Accessory>();
	AccessorySet::getInstance().addAccessory(lightAcc);

	lightAcc->addInfoService(
		"Lightbulb", 
		"Flow3r", 
		"Lightbulb", 
		"12345678", 
		"1.0.0", 
		[](bool oldValue, bool newValue, void* sender){ printf("\nLight identification routine\n"); });

	BoolCharacteristics_ptr lightPower;
	IntCharacteristics_ptr lightBrightness;
	FloatCharacteristics_ptr lightHue;
	StringCharacteristics_ptr lightName;
	FloatCharacteristics_ptr lightSaturation;
	IntCharacteristics_ptr lightColorTemperature;
	Service_ptr dimmLightService = lightAcc->
		addLightBulbService(&lightPower, &lightBrightness, &lightHue, &lightName, &lightSaturation, &lightColorTemperature);

	lightName->setValue("Dimmable light");
	lightPower->setValue(false);
	lightPower->setValueChangeCB(
		[lightAcc, lightName](bool oldValue, bool newValue, void* sender) {
		printf("Accessory %d changed service %s state to %s.\n",
			lightAcc->getID(), lightName->getValue().c_str(), newValue ? "on" : "off");
	});
	lightBrightness->setValueChangeCB(
		[lightAcc, lightName, lightPower](int oldValue, int newValue, void* sender) {
		printf("Accessory %d changed service %s brightness to %d.\n",
			lightAcc->getID(), lightName->getValue().c_str(), newValue);
		lightPower->setValue("true");
	});
	lightHue->setValueChangeCB(
		[lightAcc, lightName](double oldValue, double newValue, void* sender) {
		printf("Accessory %d changed service %s hue to %d.\n",
			lightAcc->getID(), lightName->getValue().c_str(), newValue);
	});
	lightSaturation->setValueChangeCB(
		[lightAcc, lightName](double oldValue, double newValue, void* sender) {
		printf("Accessory %d changed service %s saturation to %d.\n",
			lightAcc->getID(), lightName->getValue().c_str(), newValue);
	});
	lightColorTemperature->setValueChangeCB(
		[lightAcc, lightName](int oldValue, int newValue, void* sender) {
		printf("Accessory %d changed service %s color temperature to %d.\n",
			lightAcc->getID(), lightName->getValue().c_str(), newValue);
	});

	//Add Switch
	printf("Adding switch accessory...\n");
	Accessory_ptr switchAcc = std::make_shared<Accessory>();
	AccessorySet::getInstance().addAccessory(switchAcc);

	switchAcc->addInfoService(
		"Switch", 
		"Flow3r", 
		"Switch", 
		"12345678", 
		"1.0.0", 
		[](bool oldValue, bool newValue, void* sender) { printf("\nSwitch identification routine\n"); });

	BoolCharacteristics_ptr switchPower;
	StringCharacteristics_ptr switchName;
	Service_ptr switchService = switchAcc->addSwitchService(&switchPower, &switchName);

	switchName->setValue("Switch");
	switchPower->setValue(false);
	switchPower->setValueChangeCB(
		[switchAcc, switchName](bool oldValue, bool newValue, void* sender) {
		printf("Accessory %d changed service %s state to %s.\n",
			switchAcc->getID(), switchName->getValue().c_str(), newValue ? "on" : "off");
	});

	// Add outlet
	printf("Adding outlet accessory...\n");
	Accessory_ptr outletAcc = std::make_shared<Accessory>();
	AccessorySet::getInstance().addAccessory(outletAcc);

	outletAcc->addInfoService(
		"Outlet",
		"Flow3r",
		"Outlet",
		"12345678",
		"1.0.0",
		[](bool oldValue, bool newValue, void* sender) { printf("\nOutlet identification routine\n"); });

	BoolCharacteristics_ptr outletPower;
	BoolCharacteristics_ptr outletInUse;
	StringCharacteristics_ptr outletName;
	Service_ptr outletService = outletAcc->addOutletService(&outletPower, &outletInUse, &outletName);

	outletName->setValue("Outlet");
	outletPower->setValue(true);
	outletPower->setValueChangeCB(
		[outletAcc, outletName](bool oldValue, bool newValue, void* sender) {
		printf("Accessory %d changed service %s state to %s.\n",
			outletAcc->getID(), outletName->getValue().c_str(), newValue ? "on" : "off");
	});
	outletInUse->setValue(true);

	// Add speaker - not supported yet
	printf("Adding speaker accessory...\n");
	Accessory_ptr speakerAcc = std::make_shared<Accessory>();
	AccessorySet::getInstance().addAccessory(speakerAcc);

	speakerAcc->addInfoService(
		"Speaker",
		"Flow3r",
		"Speaker",
		"12345678",
		"1.0.0",
		[](bool oldValue, bool newValue, void* sender) { printf("\nSpeaker identification routine\n"); });

	BoolCharacteristics_ptr speakerMute;
	IntCharacteristics_ptr speakerVolume;
	StringCharacteristics_ptr speakerName;
	speakerAcc->addSpeakerService(&speakerMute, &speakerVolume, &speakerName);

	speakerName->setValue("Speaker");
	speakerMute->setValueChangeCB(
		[speakerAcc, speakerName](bool oldValue, bool newValue, void* sender) {
		printf("Accessory %d changed service %s mute to %s.\n",
			speakerAcc->getID(), speakerName->getValue().c_str(), newValue ? "on" : "off");
	});
	speakerVolume->setValueChangeCB(
		[speakerAcc, speakerName](int oldValue, int newValue, void* sender) {
		printf("Accessory %d changed service %s volume to %d.\n",
			speakerAcc->getID(), speakerName->getValue().c_str(), newValue);
	});

	// Add stateles programmable switch
	printf("Adding programmable switch accessory...\n");
	Accessory_ptr progSwitchAcc = std::make_shared<Accessory>();
	AccessorySet::getInstance().addAccessory(progSwitchAcc);

	progSwitchAcc->addInfoService(
		"Prog switch",
		"Flow3r",
		"Programmable switch",
		"12345678",
		"1.0.0",
		[](bool oldValue, bool newValue, void* sender) { printf("\nProgrammable switch identification routine\n"); });

	IntCharacteristics_ptr progSwitchEventChar;
	StringCharacteristics_ptr progSwitchNameChar;
	progSwitchAcc->addStatelessSwitchService(&progSwitchEventChar, &progSwitchNameChar);

	progSwitchNameChar->setValue("Prog switch");

	// Add window covering
	printf("Adding windows covering accessory...\n");
	Accessory_ptr windowCoveringAcc = std::make_shared<Accessory>();
	AccessorySet::getInstance().addAccessory(windowCoveringAcc);

	windowCoveringAcc->addInfoService(
		"Window covering",
		"Flow3r",
		"Window covering",
		"12345678",
		"1.0.0",
		[](bool oldValue, bool newValue, void* sender) { printf("\nWindow covering identification routine\n"); });

	IntCharacteristics_ptr wcTargetPosition;
	IntCharacteristics_ptr wcCurrentPosition;
	IntCharacteristics_ptr wcPositionState;

	windowCoveringAcc->addWindowCoveringService(&wcTargetPosition, &wcCurrentPosition, &wcPositionState);

	wcCurrentPosition->setValue(0);
	wcPositionState->setValue(2);

	wcCurrentPosition->setValueChangeCB([windowCoveringAcc](int oldValue, int newValue, void* sender) {
		printf("Accessory %d changed current position to %d.\n", windowCoveringAcc->getID(), newValue);
	});
	wcTargetPosition->setValueChangeCB([windowCoveringAcc, wcCurrentPosition, wcPositionState](int oldValue, int newValue, void* sender) {
		printf("User required target position %d on accessory %d.\n", newValue, windowCoveringAcc->getID());
		
		wcPositionState->setValue(oldValue > newValue ? 1 : 0);
		std::this_thread::sleep_for(std::chrono::seconds(2));
		wcCurrentPosition->setValue(newValue);
		wcPositionState->setValue(2);
	});
	wcPositionState->setValueChangeCB([windowCoveringAcc](int oldValue, int newValue, void* sender) {
		printf("Accessory %d changed position state to '%s'.\n", windowCoveringAcc->getID(), 
			newValue == 0 ? "going to minimum" : newValue == 1 ? "going to maximum" : "stopped");
	});
};

int main(int argc, const char * argv[]) {

	if (argc > 1) {
		//If there's some argument
		//Currently means reset
		KeyController::getInstance().resetControllerRecord();
	}

	showcaseInitializer();

	net::HAPService::getInstance().setupAndListen("LibHAP tester", "123-45-678");

	printf("Press a key to exit...\n");
	getchar();

	net::HAPService::getInstance().stop();

	return 0;
}
