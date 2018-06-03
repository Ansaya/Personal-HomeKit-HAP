/*
 * This accessory.cpp is configurated for light accessory + fan accessory
 */

#include "Accessory.h"

#include <libHAP.h>

using namespace hap;

//Global Level of light strength
int lightStength = 0;
int fanSpeedVal = 0;

void lightIdentify(bool oldValue, bool newValue, net::ConnectionInfo* info) {
    printf("Start Identify Light\n");
}

void fanIdentify(bool oldValue, bool newValue, net::ConnectionInfo* info) {
    printf("Start Identify Fan\n");
}

AccessorySet *accSet;

void initAccessorySet() {
	net::HAPService::getInstance().setup(deviceType_bridge);
    
    printf("Initial Accessory\n");

    Accessory *lightAcc = new Accessory();

    //Add Light
    accSet = &AccessorySet::getInstance();
    lightAcc->addInfoService("Light 1", "ET", "Light", "12345678", &lightIdentify);
    accSet->addAccessory(lightAcc);

    Service *lightService = new Service(serviceType_lightBulb);
    lightAcc->addService(lightService);

    StringCharacteristics *lightServiceName = new StringCharacteristics(charType_serviceName, permission_read, 0);
    lightServiceName->setValue("Light", NULL);
    lightAcc->addCharacteristics(lightService, lightServiceName);

    BoolCharacteristics *powerState = new BoolCharacteristics(charType_on, permission_read|permission_write|permission_notify);
    powerState->setValue("true", NULL);
    lightAcc->addCharacteristics(lightService, powerState);

    IntCharacteristics *brightnessState = new IntCharacteristics(charType_brightness, permission_read|permission_write, 0, 100, 1, unit_percentage);
    brightnessState->setValue("50", NULL);
    lightAcc->addCharacteristics(lightService, brightnessState);

    //Add fan
    Accessory *fan = new Accessory();
    fan->addInfoService("Fan 1", "ET", "Fan", "12345678", &fanIdentify);
    accSet->addAccessory(fan);

    Service *fanService = new Service(serviceType_fan);
    fan->addService(fanService);

    StringCharacteristics *fanServiceName = new StringCharacteristics(charType_serviceName, permission_read, 0);
    fanServiceName->setValue("Fan", NULL);
    fan->addCharacteristics(fanService, lightServiceName);

    BoolCharacteristics *fanPower = new BoolCharacteristics(charType_on, permission_read|permission_write|permission_notify);
    fanPower->setValue("true", NULL);
    fan->addCharacteristics(fanService, fanPower);
};
