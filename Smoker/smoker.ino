#include <Debounce.h>

#include "SmokerController.h"
#include "SmokerHelper.h"
#include "SmokerWeb.h"
#include "secrets.h"

TimeSettings timeSettings = {
  minToMs(45),  // DRYING TIME        (45-90min)
  hToMs(2),     // SMOKING TIME       (2-4h)
  hToMs(2),     // THERMAL TIME       (2-4h)
  minToMs(10)   // TURBO SMOKING TIME (10min)
};
TemperatureSettings temperatureSettings = {
  30, // DRYING MIN TEMP
  40, // DRYING MAX TEMP
  50, // SMOKING MIN TEMP
  65, // SMOKING MAX TEMP
  80, // THERMAL MIN TEMP
  90 // THERMAL MAX TEMP
};
RelaySettings relaySettings = { 
  D4, // POWER RELAY
  D3, // SMOKE RELAY

  12,  // TEMP PIN
  D5  // STATUS PIN
};
SmokerSettings smokerSettings = {
  &timeSettings,        // TIME SETTINGS
  &temperatureSettings, // TEMPERATURE SETTINGS
  &relaySettings        // RELAY SETTINGS
};
SmokerController smokerController(&smokerSettings);

#define E_BTN D6
#define W_BTN D7
#define S_BTN D8

#define DEBOUNCE_DELAY 50

Debounce eButton(E_BTN, DEBOUNCE_DELAY);
Debounce wButton(W_BTN, DEBOUNCE_DELAY);
Debounce sButton(S_BTN, DEBOUNCE_DELAY);

WebConfig webConfig = {
  WIFI_SSID,        // SSID
  WIFI_PASS,     // PASS

  "SmokerAP",         // AP SSID
  "Smoker2017",       // AP PASS

  "6jUKI1F7NvhWoNF",  // WWW API TOKEN 

  "smerk0r",          // OTA PASS
  "smokerMCU",        // OTA HOSTNAME
  "8266",             // OTA PORT

  "smoker",           // DOMAIN NAME
  "espSmoker",        // HOST NAME
  80                  // WEB PORT
};
SmokerWeb smokerWeb(&webConfig, &smokerController);

bool sPressed, wPressed, ePressed, controlsLocked;

void setup() {
  Serial.begin(115200);
  smokerWeb.init();

  smokerController.setState(IDLE);
}

void loop() {
  smokerController.update();
  smokerWeb.update();
}
