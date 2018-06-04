#ifndef HAP_SERVICE_TYPE
#define HAP_SERVICE_TYPE


namespace hap {

enum service_type {

	service_accessoryInfo = 0x3E,
	service_camera = 0x3F,
	service_fan = 0x40,
	service_garageDoorOpener = 0x41,
	service_lightBulb = 0x43,
	service_lockManagement = 0x44,
	service_lockMechanism = 0x45,
	service_microphone = 0x46,
	service_outlet = 0x47,
	service_speaker = 0x48,
	service_switch = 0x49,
	service_thermostat = 0x4A,

	service_alarmSystem = 0x7E,
	service_bridgingState = 0x62,
	service_carbonMonoxideSensor = 0x7F,
	service_contactSensor = 0x80,
	service_door = 0x81,
	service_humiditySensor = 0x82,
	service_leakSensor = 0x83,
	service_lightSensor = 0x84,
	service_motionSensor = 0x85,
	service_occupancySensor = 0x86,
	service_smokeSensor = 0x87,
	service_programmableSwitch_stateful = 0x88,
	service_programmableSwitch_stateless = 0x89,
	service_temperatureSensor = 0x8A,
	service_window = 0x8B,
	service_windowCover = 0x8C,
	service_airQualitySensor = 0x8C,
	service_securityAlarm = 0x8E,
	service_charging = 0x8F,

	service_battery = 0x96,
	service_carbonDioxideSensor = 0x97,

#pragma - The following is for bluetooth service
	btservice_accessoryInformation = 0xFED3,
	btservice_camera = 0xFEC9,
	btservice_fan = 0xFECB,
	btservice_garageDoorOpener = 0xFECE,
	btservice_lightBulb = 0xFED2,
	btservice_lockManagement = 0xFECF,
	btservice_lockMechanism = 0xFED0,
	btservice_microphone = 0xFEC8,
	btservice_outlet = 0xFECC,
	btservice_speaker = 0xFEC7,
	btservice_switch = 0xFECD,

};

}

#endif