#include "SmokerBlynk.h"

SmokerBlynk::SmokerBlynk(char* blynkToken, SmokerController* smokerController) : smokerController(smokerController) {
    Blynk.config(blynkToken);
    Blynk.connect();
}

void SmokerBlynk::update() {
    Blynk.run();
}
