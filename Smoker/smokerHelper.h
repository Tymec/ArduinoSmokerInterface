#ifndef SMOKER_HELPER_H
#define SMOKER_HELPER_H
#include <Arduino.h>
#include <FS.h>

#define MILLIS_IN_MIN 60000
#define MILLIS_IN_H   3600000

enum SmokerSettingsTimeType {
    DRYING_TIME_SETTING,
    SMOKING_TIME_SETTING,
    THERMAL_TIME_SETTING,
    TURBO_TIME_SETTING
};

enum SmokerSettingsTemperatureType {
    DRYING_MIN_TEMP_SETTING,
    DRYING_MAX_TEMP_SETTING,

    SMOKING_MIN_TEMP_SETTING,
    SMOKING_MAX_TEMP_SETTING,

    THERMAL_MIN_TEMP_SETTING,
    THERMAL_MAX_TEMP_SETTING
};

enum SmokerSettingsRelayType {
    POWER_RELAY_SETTING,
    SMOKE_RELAY_SETTING,

    TEMP_PIN_SETTING,
    STATUS_PIN_SETTING
};

enum SmokerState {
    IDLE,
    DRYING,
    SMOKING,
    THERMAL
};

class Relay {
    public:
        Relay(char* name, bool defaultState, uint8_t pin) : name(name), state(defaultState), pin(pin) {
            pinMode(pin, OUTPUT);
        }
        bool get() { return this->state; }
        void setPin(uint8_t i) { 
          this->pin = i;
          pinMode(this->pin, OUTPUT);
          digitalWrite(this->pin, this->state);
        }
        char* getName() { return this->name; }
        bool read() { 
            this->state = digitalRead(this->pin);
            return this->state;
        }
        void set(bool state) {
            this->state = state;
            digitalWrite(this->pin, this->state);
        }
    private:
        char* name;
        bool state;
        uint8_t pin;
};

typedef struct {
  unsigned long dryingTime;
  unsigned long smokingTime;
  unsigned long thermalTime;

  unsigned long turboSmokingTime;
} TimeSettings;

typedef struct {
  uint16_t dryingMinTemp;
  uint16_t dryingMaxTemp;

  uint16_t smokingMinTemp;
  uint16_t smokingMaxTemp;

  uint16_t thermalMinTemp;
  uint16_t thermalMaxTemp;
} TemperatureSettings;

typedef struct {
  uint8_t powerRelay;
  uint8_t smokeRelay;

  uint8_t tempPin;
  uint8_t statusPin;
} RelaySettings;

typedef struct {
  TimeSettings* times;
  TemperatureSettings* temperatures;
  RelaySettings* relays;
} SmokerSettings;

inline unsigned short getSizeOfInt(int i) {
  char buffer[6];
  sprintf(buffer, "%d", abs(i));
  return(strlen(buffer));
}

inline unsigned short getSizeOfFloat(float i) {
  char buffer[10];
  dtostrf(i, 5, 1, buffer);
  return(strlen(buffer) - 1);
}

inline unsigned long msToMin(unsigned long milliseconds) {
  return milliseconds / MILLIS_IN_MIN;
} 

inline unsigned long minToMs(unsigned long minutes) {
  return minutes * MILLIS_IN_MIN;
}

inline unsigned long hToMs(unsigned long hours) {
  return hours * MILLIS_IN_H;
}

inline void logFunctionCall(char* functionName, char* functionValue) {
  return;
  Serial.println("");
  Serial.print(functionName);
  Serial.print(": ");
  Serial.print(functionValue);
  Serial.println("");
}

inline void logFunctionCall(char* functionName, int functionValue) {
  return;
  Serial.println("");
  Serial.print(functionName);
  Serial.print(": ");
  Serial.print(functionValue);
  Serial.println("");
}

inline void logFunctionCall(char* functionName, float functionValue) {
  return;
  Serial.println("");
  Serial.print(functionName);
  Serial.print(": ");
  Serial.print(functionValue);
  Serial.println("");
}

#endif
