#ifndef SMOKER_CONTROLLER_H
#define SMOKER_CONTROLLER_H
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Arduino.h>
#include "SmokerHelper.h"

class SmokerController {
    public:
        SmokerController(SmokerSettings* smokerSettings);
        
        void update();
        float getTemperature();
        SmokerSettings* getSmokerSettings();
        SmokerState getState();
        void setState(SmokerState newState);
        bool getRelayState(SmokerSettingsRelayType type);
        void setRelayState(SmokerSettingsRelayType type, bool state);
        void setManualMode(bool value);
        bool isManualMode();

        uint32_t getSmokerSettingsTime(SmokerSettingsTimeType type);
        void setSmokerSettingsTime(SmokerSettingsTimeType type, uint32_t v);
        uint16_t getSmokerSettingsTemperature(SmokerSettingsTemperatureType type);
        void setSmokerSettingsTemperature(SmokerSettingsTemperatureType type, uint16_t v);
        uint8_t getSmokerSettingsRelay(SmokerSettingsRelayType type);
        uint16_t getTimeLeft(SmokerSettingsRelayType type);

        bool isStateFinished();
        bool isSmokingFinished();
    private:
        void handleDryingState();
        void handleSmokingState();
        void handleThermalState();
        void resetState();
        float readTemperature();

        float currentTemperature;
        unsigned long previousTime;
        unsigned long stateTime;
        unsigned long smokingTime;

        unsigned long timeLeft;
        unsigned long smokingTimeLeft;
        
        bool manualMode = false;

        Relay* powerRelay;
        Relay* smokeRelay;
        Relay* statusIndicator;

        OneWire oneWire;
        DallasTemperature tempSensor;

        SmokerSettings* smokerSettings;
        SmokerState state = IDLE;
};
#endif