#ifndef HAP_CHAR_TYPE
#define HAP_CHAR_TYPE

namespace hap {

enum char_type {

    char_adminOnlyAccess    = 0x1,
    char_audioChannels      = 0x2,
    char_audioCodexName     = 0x3,
    char_audioCodexParameter= 0x4,
    char_audioFeedback      = 0x5,
    char_audioPropAttr      = 0x6,
    char_audioValAttr       = 0x7,
    char_brightness         = 0x8,
    char_cameraNightVision  = 0x9,
    char_cameraPan          = 0xA,
    char_cameraTilt         = 0xB,
    char_cameraZoom         = 0xC,
    char_coolingThreshold   = 0xD,
    char_currentDoorState   = 0xE,
    char_currentHeatCoolMode= 0xF,
    char_currentHumidity    = 0x10,
    char_currentTemperature = 0x11,
    char_heatingThreshold   = 0x12,
    char_hue                = 0x13,
    char_identify           = 0x14,
    char_inputVolume        = 0x15,
    char_ipCameraStart      = 0x16,
    char_ipCameraStop       = 0x17,
    char_lockControlPoint   = 0x19,
    char_lockAutoTimeout    = 0x1A,
    char_lockLastAction     = 0x1C,
    char_lockCurrentState   = 0x1D,
    char_lockTargetState    = 0x1E,
    char_logs               = 0x1F,
    char_manufactuer        = 0x20,
    char_modelName          = 0x21,
    char_motionDetect       = 0x22,
    char_serviceName        = 0x23,
    char_obstruction        = 0x24,
    char_on                 = 0x25,
    char_outletInUse        = 0x26,
    char_outputVolume       = 0x27,
    char_rotationDirection  = 0x28,
    char_rotationSpeed      = 0x29,
    char_rtcpExtProp        = 0x2A,
    char_rtcpVideoPayload   = 0x2B,
    char_rtcpAudioPayload   = 0x2C,
    char_rtcpAudioClock     = 0x2D,
    char_rtcpProtocol       = 0x2E,
    char_saturation         = 0x2F,
    char_serialNumber       = 0x30,
    char_srtpCyptoSuite     = 0x31,
    char_targetDoorState    = 0x32,
    char_targetHeatCoolMode = 0x33,
    char_targetHumidity     = 0x34,
    char_targetTemperature  = 0x35,
    char_temperatureUnit    = 0x36,
    char_version            = 0x37,
    char_videoCodexName     = 0x38,
    char_videoCodexPara     = 0x39,
    char_videoMirror        = 0x3A,
    char_videoPropAttr      = 0x3B,
    char_videoRotation      = 0x3C,
    char_videoValAttr       = 0x3D,
    
// - The following is only default by the device after iOS 9
    
    char_firmwareRevision   = 0x52,
    char_hardwareRevision   = 0x53,
    char_softwareRevision   = 0x54,
    
    char_reachable          = 0x63,
    
    char_airParticulateDensity = 0x64,
    char_airParticulateSize = 0x65,
    char_airQuality         = 0x95,
    char_carbonDioxideDetected = 0x92,
    char_carbonMonoxideDetected = 0x69,
    char_carbonDioxideLevel = 0x93,
    char_carbonMonoxideLevel = 0x90,
    char_carbonDioxidePeakLevel = 0x94,
    char_carbonMonoxidePeakLevel = 0x91,
    char_smokeDetected      = 0x76,
    
    char_alarmCurrentState  = 0x66,
    char_alarmTargetState   = 0x67,
    char_batteryLevel       = 0x68,
    char_contactSensorState = 0x6A,
    char_holdPosition       = 0x6F,
    char_leakDetected       = 0x70,
    char_occupancyDetected  = 0x71,
    
    char_currentAmbientLightLevel = 0x6B,
    char_currentHorizontalTiltAngle = 0x6C,
    char_targetHorizontalTiltAngle = 0x7B,
    char_currentPosition    = 0x6D,
    char_targetPosition     = 0x7C,
    char_currentVerticalTiltAngle = 0x6E,
    char_targetVerticalTiltAngle = 0x7D,
    
    char_positionState      = 0x72,
    char_programmableSwitchEvent = 0x73,
    char_programmableSwitchOutputState = 0x74,
    
    char_sensorActive       = 0x75,
    char_sensorFault        = 0x77,
    char_sensorJammed       = 0x78,
    char_sensorLowBattery   = 0x79,
    char_sensorTampered     = 0x7A,
    char_sensorChargingState= 0x8F,
    
// - The following is for bluetooth characteristic
    btchar_pairSetup = 0x4C,
    btchar_pairVerify = 0x4E,
    btchar_pairingFeature = 0x4F,
    btchar_pairings = 0x50,
    btchar_serviceInstanceID = 0x51
    
};

}

#endif