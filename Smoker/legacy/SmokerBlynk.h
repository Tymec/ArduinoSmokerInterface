#ifndef SMOKER_BLYNK_H
#define SMOKER_BLYNK_H
#include <Arduino.h>

#include <ESP8266WiFi.h>
#include "SmokerController.h"
#include <BlynkSimpleEsp8266.h>

//#define BLYNK_TOKEN "0JIqHaYATIR3-MMpZi4-KQCSrU_2UEJd"

class SmokerBlynk {
    public:
        SmokerBlynk(char* blynkToken, SmokerController* smokerController);

        void update();
    private:
        SmokerController* smokerController;
};

#endif
