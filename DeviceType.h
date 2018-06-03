#ifndef HAP_DEVICE_TYPE
#define HAP_DEVICE_TYPE

namespace hap {

	typedef enum {
		deviceType_other = 0,
		deviceType_bridge = 2,
		deviceType_fan = 3,
		deviceType_garageDoorOpener = 4,
		deviceType_lightBulb = 5,
		deviceType_doorLock = 6,
		deviceType_outlet = 7,
		deviceType_switch = 8,
		deviceType_thermostat = 9,
		deviceType_sensor = 10,
		deviceType_alarmSystem = 11,
		deviceType_door = 12,
		deviceType_window = 13,
		deviceType_windowCover = 14,
		deviceType_programmableSwitch = 15,
	} deviceType;

}

#endif // !HAP_DEVICE_TYPE
