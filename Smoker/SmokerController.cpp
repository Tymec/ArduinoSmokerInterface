#include "SmokerController.h"

SmokerController::SmokerController(SmokerSettings* smokerSettings) : smokerSettings(smokerSettings) {
    pinMode(smokerSettings->temperatures->tempPin, INPUT);
    this->oneWire = new OneWire(smokerSettings->temperatures->tempPin);
    this->tempSensor = new DallasTemperature(this->oneWire);

    this->powerRelay = new Relay("Power", false, smokerSettings->relays->powerRelay);
    this->smokeRelay = new Relay("Smoke", false, smokerSettings->relays->smokeRelay);
}

void SmokerController::update() {
    this->currentTemperature = readTemperature();
    logFunctionCall("SmokerController::update()", "Temperature updated");
    logFunctionCall("SmokerController::update()", this->currentTemperature);

    switch (this->state) {
        case IDLE:
            logFunctionCall("SmokerController::update()", "State is IDLE");
            break;
        case DRYING:
            logFunctionCall("SmokerController::update()", "State is DRYING");
            handleDryingState();
            break;
        case SMOKING:
            logFunctionCall("SmokerController::update()", "State is SMOKING");
            handleSmokingState();
            break;
        case THERMAL:
            logFunctionCall("SmokerController::update()", "State is THERMAL");
            handleThermalState();
            break;
        default:
            logFunctionCall("SmokerController::update()", "Case is DEFAULT");
            break;
    }
}

SmokerState SmokerController::getState() {
    return this->state;
}

float SmokerController::getTemperature() {
    logFunctionCall("SmokerController::getTemperature()", currentTemperature);
    return this->currentTemperature;
}

SmokerSettings* SmokerController::getSmokerSettings() {
    return this->smokerSettings;
}

bool SmokerController::getRelayState(SmokerSettingsRelayType type) {
    logFunctionCall("SmokerController::getSmokerRelayState()", "Requested Relay State");
    switch (type) {
        case POWER_RELAY_SETTING:
            logFunctionCall("SmokerController::getSmokerRelayState()", this->powerRelay->getName());
            return this->powerRelay->get();
            break;
        case SMOKE_RELAY_SETTING:
            logFunctionCall("SmokerController::getSmokerRelayState()", this->smokeRelay->getName());
            return this->smokeRelay->get();
            break;
        default:
            logFunctionCall("SmokerController::getSmokerRelayState()", "DEFAULT CASE OCCURED");
            return false;
            break;
    }
}

void SmokerController::setRelayState(SmokerSettingsRelayType type, bool state) {
    switch (type) {
        case POWER_RELAY_SETTING:
            logFunctionCall("SmokerController::setSmokerRelayState()", "Set New Power Relay State");
            this->powerRelay->set(state);
            break;
        case SMOKE_RELAY_SETTING:
            logFunctionCall("SmokerController::setSmokerRelayState()", "Set New Smoke Relay State");
            this->smokeRelay->set(state);
            break;
        default:
            logFunctionCall("SmokerController::setSmokerRelayState()", "DEFAULT CASE OCCURED");
            break;
    }
    logFunctionCall("SmokerController::setSmokerRelayState()", state);
}

uint32_t SmokerController::getSmokerSettingsTime(SmokerSettingsTimeType type) {
    logFunctionCall("SmokerController::getSmokerSettingsTime()", "Requested Time Setting");
    switch (type) {
        case DRYING_TIME_SETTING:
            logFunctionCall("SmokerController::getSmokerSettingsTime()", (uint16_t)msToMin(this->smokerSettings->times->dryingTime));
            return this->smokerSettings->times->dryingTime;
            break;
        case SMOKING_TIME_SETTING:
            logFunctionCall("SmokerController::getSmokerSettingsTime()", (uint16_t)msToMin(this->smokerSettings->times->smokingTime));
            return this->smokerSettings->times->smokingTime;
            break;
        case THERMAL_TIME_SETTING:
            logFunctionCall("SmokerController::getSmokerSettingsTime()", (uint16_t)msToMin(this->smokerSettings->times->thermalTime));
            return this->smokerSettings->times->thermalTime;
            break;
        case TURBO_TIME_SETTING:
            logFunctionCall("SmokerController::getSmokerSettingsTime()", (uint16_t)msToMin(this->smokerSettings->times->turboSmokingTime));
            return this->smokerSettings->times->turboSmokingTime;
            break;
        default:
            logFunctionCall("SmokerController::getSmokerSettingsTime()", "DEFAULT CASE OCCURED");
            break;
    }
}

void SmokerController::setSmokerSettingsTime(SmokerSettingsTimeType type, uint32_t v) {
    switch (type) {
        case DRYING_TIME_SETTING:
            logFunctionCall("SmokerController::setSmokerSettingsTime()", "Set New Drying Time Setting");
            this->smokerSettings->times->dryingTime = v;
            break;
        case SMOKING_TIME_SETTING:
            logFunctionCall("SmokerController::setSmokerSettingsTime()", "Set New Smoking Time Setting");
            this->smokerSettings->times->smokingTime = v;
            break;
        case THERMAL_TIME_SETTING:
            logFunctionCall("SmokerController::setSmokerSettingsTime()", "Set New Thermal Time Setting");
            this->smokerSettings->times->thermalTime = v;
            break;
        case TURBO_TIME_SETTING:
            logFunctionCall("SmokerController::setSmokerSettingsTime()", "Set New Turbo Smoking Time Setting");
            this->smokerSettings->times->turboSmokingTime = v;
            break;
        default:
            logFunctionCall("SmokerController::setSmokerSettingsTime()", "DEFAULT CASE OCCURED");
            break;
    }
    logFunctionCall("SmokerController::setSmokerSettingsTime()", (uint16_t)msToMin(v));
}

uint16_t SmokerController::getSmokerSettingsTemperature(SmokerSettingsTemperatureType type) {
    logFunctionCall("SmokerController::getSmokerSettingsTemperature()", "Requested Temperature Setting");
    switch (type) {
        case DRYING_MIN_TEMP_SETTING:
            logFunctionCall("SmokerController::getSmokerSettingsTemperature()", this->smokerSettings->temperatures->dryingMinTemp);
            return this->smokerSettings->temperatures->dryingMinTemp;
            break;
        case DRYING_MAX_TEMP_SETTING:
            logFunctionCall("SmokerController::getSmokerSettingsTemperature()", this->smokerSettings->temperatures->dryingMaxTemp);
            return this->smokerSettings->temperatures->dryingMaxTemp;
            break;
            
        case SMOKING_MIN_TEMP_SETTING:
            logFunctionCall("SmokerController::getSmokerSettingsTemperature()", this->smokerSettings->temperatures->smokingMinTemp);
            return this->smokerSettings->temperatures->smokingMinTemp;
            break;
        case SMOKING_MAX_TEMP_SETTING:
            logFunctionCall("SmokerController::getSmokerSettingsTemperature()", this->smokerSettings->temperatures->smokingMaxTemp);
            return this->smokerSettings->temperatures->smokingMaxTemp;
            break;

        case THERMAL_MIN_TEMP_SETTING:
            logFunctionCall("SmokerController::getSmokerSettingsTemperature()", this->smokerSettings->temperatures->thermalMinTemp);
            return this->smokerSettings->temperatures->thermalMinTemp;
            break;
        case THERMAL_MAX_TEMP_SETTING:
            logFunctionCall("SmokerController::getSmokerSettingsTemperature()", this->smokerSettings->temperatures->thermalMaxTemp);
            return this->smokerSettings->temperatures->thermalMaxTemp;
            break;

        case TEMP_PIN_SETTING:
            logFunctionCall("SmokerController::getSmokerSettingsTemperature()", "Requested Temperature Sensor Pin");
            logFunctionCall("SmokerController::getSmokerSettingsTemperature()", this->smokerSettings->temperatures->tempPin);
            return this->smokerSettings->temperatures->tempPin;
            break;
        default:
            logFunctionCall("SmokerController::getSmokerSettingsTemperature()", "DEFAULT CASE OCCURED");
            break;
    }
}

void SmokerController::setSmokerSettingsTemperature(SmokerSettingsTemperatureType type, uint16_t v) {
    switch (type) {
        case DRYING_MIN_TEMP_SETTING:
            logFunctionCall("SmokerController::setSmokerSettingsTemperature()", "Set New Drying Min Temp");
            this->smokerSettings->temperatures->dryingMinTemp = v;
            break;
        case DRYING_MAX_TEMP_SETTING:
            logFunctionCall("SmokerController::setSmokerSettingsTemperature()", "Set New Drying Max Temp");
            this->smokerSettings->temperatures->dryingMaxTemp = v;
            break;

        case SMOKING_MIN_TEMP_SETTING:
            logFunctionCall("SmokerController::setSmokerSettingsTemperature()", "Set New Smoking Min Temp");
            this->smokerSettings->temperatures->smokingMinTemp = v;
            break;
        case SMOKING_MAX_TEMP_SETTING:
            logFunctionCall("SmokerController::setSmokerSettingsTemperature()", "Set New Smoking Max Temp");
            this->smokerSettings->temperatures->smokingMaxTemp = v;
            break;

        case THERMAL_MIN_TEMP_SETTING:
            logFunctionCall("SmokerController::setSmokerSettingsTemperature()", "Set New Thermal Min Temp");
            this->smokerSettings->temperatures->thermalMinTemp = v;
            break;
        case THERMAL_MAX_TEMP_SETTING:
            logFunctionCall("SmokerController::setSmokerSettingsTemperature()", "Set New Thermal Max Temp");
            this->smokerSettings->temperatures->thermalMaxTemp = v;
            break;
        default:
            logFunctionCall("SmokerController::setSmokerSettingsTemperature()", "DEFAULT CASE OCCURED");
            break;
    }
    logFunctionCall("SmokerController::setSmokerSettingsTemperature()", v);
}

uint8_t SmokerController::getSmokerSettingsRelay(SmokerSettingsRelayType type) {
    logFunctionCall("SmokerController::getSmokerSettingsRelay()", "Requested Relay Pin");
    switch (type) {
        case POWER_RELAY_SETTING:
            logFunctionCall("SmokerController::getSmokerSettingsRelay()", this->smokerSettings->relays->powerRelay);
            return this->smokerSettings->relays->powerRelay;
            break;
        case SMOKE_RELAY_SETTING:
            logFunctionCall("SmokerController::getSmokerSettingsRelay()", this->smokerSettings->relays->smokeRelay);
            return this->smokerSettings->relays->smokeRelay;
            break;
        default:
            logFunctionCall("SmokerController::getSmokerSettingsRelay()", "DEFAULT CASE OCCURED");
            break;
    }
}

void SmokerController::setState(SmokerState newState) {
    this->state = newState;
    if (this->state == IDLE) {
        logFunctionCall("SmokerController::setState()", "State changed to IDLE");
        this->stateTime = 0;
        powerRelay->set(false);
        smokeRelay->set(false);
        return; 
    }

    switch (this->state) {
        case DRYING:
            logFunctionCall("SmokerController::setState()", "State changed to DRYING");
            this->stateTime = smokerSettings->times->dryingTime;
            break;
        case SMOKING:
            logFunctionCall("SmokerController::setState()", "State changed to SMOKING");
            this->stateTime = smokerSettings->times->smokingTime;
            this->smokingTime = smokerSettings->times->turboSmokingTime;
            this->smokeRelay->set(true);
            break;
        case THERMAL:
            logFunctionCall("SmokerController::setState()", "State changed to THERMAL");
            this->stateTime = smokerSettings->times->thermalTime;
            break;
        default:
            logFunctionCall("SmokerController::setState()", "DEFAULT CASE OCCURED");
            break;
    }
    this->previousTime = millis();
}

void SmokerController::handleDryingState() {
    if (isStateFinished()) {
        logFunctionCall("SmokerController::handleDryingState()", "State Finished");
        resetState();
        return;
    }

    if (getTemperature() >= this->smokerSettings->temperatures->dryingMaxTemp) {
        logFunctionCall("SmokerController::handleDryingState()", "Temperature Too High");
        this->powerRelay->set(false);
    } else if (getTemperature() <= this->smokerSettings->temperatures->dryingMinTemp) {
        logFunctionCall("SmokerController::handleDryingState()", "Temperature Too Low");
        this->powerRelay->set(true);
    }
}

void SmokerController::handleSmokingState() {
    if (isStateFinished()) {
        logFunctionCall("SmokerController::handleSmokingState()", "State Finished");
        resetState();
        return;
    }

    if (isSmokingFinished()) {
        logFunctionCall("SmokerController::handleSmokingState()", "Turbo Smoking Finished");
        this->smokeRelay->set(false);
    }

    if (getTemperature() >= this->smokerSettings->temperatures->smokingMaxTemp) {
        logFunctionCall("SmokerController::handleSmokingState()", "Temperature Too High");
        this->powerRelay->set(false);
    } else if (getTemperature() <= this->smokerSettings->temperatures->smokingMinTemp) {
        logFunctionCall("SmokerController::handleSmokingState()", "Temperature Too Low");
        this->powerRelay->set(true);
    }

}

void SmokerController::handleThermalState() {
    if (isStateFinished()) {
        logFunctionCall("SmokerController::handleThermalState()", "State Finished");
        resetState();
        return;
    }

    if (getTemperature() >= this->smokerSettings->temperatures->thermalMaxTemp) {
        logFunctionCall("SmokerController::handleThermalState()", "Temperature Too High");
        this->powerRelay->set(false);
    } else if (getTemperature() <= this->smokerSettings->temperatures->thermalMinTemp) {
        logFunctionCall("SmokerController::handleThermalState()", "Temperature Too Low");
        this->powerRelay->set(true);
    }
}

void SmokerController::resetState() {
    logFunctionCall("SmokerController::resetState()", "State Finished");
    setState(IDLE);
}

bool SmokerController::isStateFinished() {
    if ((unsigned long)(millis() - this->previousTime) >= this->stateTime) {
        return true;
    }
    return false;
}

bool SmokerController::isSmokingFinished() {
    if ((unsigned long)(millis() - this->previousTime) >= this->smokingTime) {
        return true;
    }
    return false;
}

float SmokerController::readTemperature() {
    this->tempSensor->requestTemperatures();
    return this->tempSensor->getTempCByIndex(0);
}

