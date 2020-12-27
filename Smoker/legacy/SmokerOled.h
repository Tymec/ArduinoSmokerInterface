#ifndef SMOKER_OLED_H
#define SMOKER_OLED_H
#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

#include "SmokerController.h"

class SmokerOled {
    public:
        SmokerOled(SmokerController* smokerController);
        void init();
    private:
};

#endif
