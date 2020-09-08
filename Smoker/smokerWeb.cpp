#include "SmokerWeb.h"

SmokerWeb::SmokerWeb(WebConfig* webConfig, SmokerController* smokerController) : webConfig(webConfig), smokerController(smokerController) {
    //this->server = new ESP8266WebServer(80);
    this->server = new AsyncWebServer(webConfig->webPort);
    this->smokerSettings = smokerController->getSmokerSettings();
}

void SmokerWeb::init() {
    IPAddress local_ip(192, 168, 1, 1);
    IPAddress gateway(192, 168, 1, 1);
    IPAddress subnet(255, 255, 255, 0);

    //WiFi.mode(WIFI_AP_STA);
    WiFi.mode(WIFI_STA);
    //WiFi.softAP(webConfig->apSsid, webConfig->apPass);
    //WiFi.softAPConfig(local_ip, gateway, subnet);
    WiFi.begin(webConfig->ssid, webConfig->pass);

    while (!isConnected()) {
        delay(500);
    }
    if (!MDNS.begin(webConfig->domainName)) {
        Serial.println("SmokerWeb::init(): Error setting up MDNS responder");
    }
    if (!SPIFFS.begin()) {
        Serial.println("SmokerWeb::init(): Error setting up SPIFFS");
    }
    
    /* ArduinoOTA
    ArduinoOTA.setPort(webConfig->otaPort);
    ArduinoOTA.setHostname(webConfig->otaHost);
    ArduinoOTA.setPassword(webConfig->otaPass);

    ArduinoOTA.onStart(std::bind(&SmokerWeb::ota_OnStart, this));
    ArduinoOTA.onEnd(std::bind(&SmokerWeb::ota_OnEnd, this));
    ArduinoOTA.onProgress(std::bind(ota_onProgress, this, std::placeholders::_1, std::placeholders::_2));
    ArduinoOTA.onError(std::bind(ota_onError, this, std::placeholders::_1));
    */
    /* WebSocket
    this->ws = new AsyncWebSocket("/ws")
    AsyncEventSource events("/events");
    this->ws->onEvent(std::bind(&SmokerWeb::ws_OnEvent, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6));
    this->server->addHandler(&this->ws);
    this->server->addHandler(&events);
    */

    this->server->onNotFound(std::bind(&SmokerWeb::handle_OnNotFound, this, std::placeholders::_1));
    this->server->on("/", std::bind(&SmokerWeb::handle_OnHome, this, std::placeholders::_1));
    this->server->on("/settings", std::bind(&SmokerWeb::handle_OnSettings, this, std::placeholders::_1));

    this->server->on("/api/controller/relay", std::bind(&SmokerWeb::handle_OnApiControllerRelay, this, std::placeholders::_1)); 
    this->server->on("/api/controller/state", std::bind(&SmokerWeb::handle_OnApiControllerState, this, std::placeholders::_1));
    this->server->on("/api/controller/temperature", std::bind(&SmokerWeb::handle_OnApiControllerTemperature, this, std::placeholders::_1));
    
    this->server->on("/api/settings/", std::bind(&SmokerWeb::handle_OnApiSettings, this, std::placeholders::_1));
    this->server->on("/api/settings/temperature", std::bind(&SmokerWeb::handle_OnApiSettingsTemperature, this, std::placeholders::_1));
    this->server->on("/api/settings/time", std::bind(&SmokerWeb::handle_OnApiSettingsTime, this, std::placeholders::_1)); 
    this->server->on("/api/settings/relay", std::bind(&SmokerWeb::handle_OnApiSettingsRelay, this, std::placeholders::_1));
    this->server->on("/api/settings/temperature_pin", std::bind(&SmokerWeb::handle_OnApiSettingsTemperaturePin, this, std::placeholders::_1));

    //this->server->serveStatic("/fs", SPIFFS, "/");

    //ArduinoOTA.begin();
    this->server->begin();
    MDNS.addService("http", "tcp", 80);
}

void SmokerWeb::update() {
    MDNS.update();
}

bool SmokerWeb::isConnected() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("SmokerWeb::isConnected(): Server is still trying to connect");
        return false;
    }
    Serial.println("SmokerWeb::isConnected(): Server successfully connected");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    return true;
}

bool SmokerWeb::isRequestAuthenticated(AsyncWebServerRequest *request) {
    Serial.println("SmokerWeb::isRequestAuthenticated(): Authentication requested");
    /*
    if (!request->authenticate(this->webConfig->wwwUser, this->webConfig->wwwPass)) {
        request->requestAuthentication();
        return false;
    }
    */
    if (!request->hasParam("token")) {
        Serial.println("SmokerWeb::isRequestAuthenticated(): Authentication failed");
        return false;
    } else {
        AsyncWebParameter* token = request->getParam("token");
        if (token->value() != this->webConfig->wwwApiToken) {
            Serial.println("SmokerWeb::isRequestAuthenticated(): Authentication failed");
            return false;
        }
    }
    Serial.println("SmokerWeb::isRequestAuthenticated(): Authenticated successfully");
    return true;
}

String SmokerWeb::getIP() {
    return WiFi.localIP().toString();
}

/* ArduinoOTA
void SmokerWeb::ota_OnStart() {
    SPIFFS.end();

    ws.enable(false);
    ws.textAll("OTA Update Started");
    ws.closeAll();

    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    logFunctionCall("SmokerWeb::ota_OnEnd()", "Start updating " + type);
}

void SmokerWeb::ota_OnEnd() {
    logFunctionCall("SmokerWeb::ota_OnEnd()", "End");
}

void SmokerWeb::ota_onProgress(unsigned int progress, unsigned int total) {
    char* buf[20];
    printf(buf, "Progress: %u%%\r", (progress / (total / 100)));
    logFunctionCall("SmokerWeb::ota_onProgress", buf);
}

void SmokerWeb::ota_onError(ota_error_t error) {
    char* buf[15];
    printf(buf, "Error[%u]: ", error);
    logFunctionCall("SmokerWeb::ota_onError", buf);

    if (error == OTA_AUTH_ERROR) {
        logFunctionCall("SmokerWeb::ota_onError", "Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
        logFunctionCall("SmokerWeb::ota_onError", "Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
        logFunctionCall("SmokerWeb::ota_onError", "Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
        logFunctionCall("SmokerWeb::ota_onError", "Receive Failed");
    } else if (error == OTA_END_ERROR) {
        logFunctionCall("SmokerWeb::ota_onError", "End Failed");
    }
}
*/
/* WebSocket
    void SmokerWeb::ws_OnEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len) {

    }
*/

void SmokerWeb::handle_OnNotFound(AsyncWebServerRequest *request) {
    request->send(400, "text/html", "<h1>404 Not Found</h1>");
}

void SmokerWeb::handle_OnHome(AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html");
}

void SmokerWeb::handle_OnSettings(AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/settings.html");
}

void SmokerWeb::handle_OnApiControllerRelay(AsyncWebServerRequest *request) {
    if (!isRequestAuthenticated(request)) {
        request->send(401, "text/html", "<h1>Missing API key ('token') parameter</h1>");
        return;
    }

    if (!request->hasParam("n")) {
        request->send(400, "text/html", "<h1>Missing name ('n') parameter</h1>");
        return;
    }

    AsyncWebParameter* relayName = request->getParam("n");

    switch (request->method()) {
        case HTTP_GET:
            {
                bool relayValue;
                if (relayName->value() == "power") {
                    relayValue = smokerController->getRelayState(POWER_RELAY_SETTING);
                } else if (relayName->value() == "smoke") {
                    relayValue = smokerController->getRelayState(SMOKE_RELAY_SETTING);
                } else {
                    request->send(400, "text/html", "<h1>Invalid name ('n') parameter value</h1>");
                    return;
                }

                AsyncJsonResponse* response = new AsyncJsonResponse();
                JsonObject root = response->getRoot();
                JsonObject data = root.createNestedObject("data");
                data["value"] = relayValue;
                data["message"] = "Controller relay state requested.";
                data["heap"] = ESP.getFreeHeap();
                response->setLength();
                request->send(response);
            }
           
            break;
        case HTTP_POST:
            {
                if (!request->hasParam("v")) {
                    request->send(400, "text/html", "<h1>Missing value ('v') parameter</h1>");
                    return;
                }

                bool relayValue = request->getParam("v")->value() == "true" || request->getParam("v")->value() == "1";
                if (relayName->value() == "power") {
                    smokerController->setRelayState(POWER_RELAY_SETTING, relayValue);
                } else if (relayName->value() == "smoke") {
                    smokerController->setRelayState(SMOKE_RELAY_SETTING, relayValue);
                } else { 
                    request->send(400, "text/html", "<h1>Invalid name ('n') parameter value</h1>");
                    return;
                }
                request->send(204, "text/plain", "");
            }
            break;
        default:
            request->send(405, "text/html", "<h1>Request method not allowed</h1>");
            break;
    }
}

void SmokerWeb::handle_OnApiControllerState(AsyncWebServerRequest *request) {
    if (!isRequestAuthenticated(request)) {
        request->send(401, "text/html", "<h1>Missing API key ('token') parameter</h1>");
        return;
    }

    switch (request->method()) {
        case HTTP_GET:
            {
                SmokerState stateValue = smokerController->getState();
                char* stateName;
    
                switch (stateValue) {
                    case IDLE:
                        stateName = "IDLE";
                        break;
                    case DRYING:
                        stateName = "DRYING";
                        break;
                    case SMOKING:
                        stateName = "SMOKING";
                        break;
                    case THERMAL:
                        stateName = "THERMAL";
                        break;
                    default:
                        request->send(500, "text/html", "<h1>Invalid controller state</h1>");
                        return;
                        break;
                }

                AsyncJsonResponse* response = new AsyncJsonResponse();
                JsonObject root = response->getRoot();
                JsonObject data = root.createNestedObject("data");
                data["value"] = stateName;
                data["message"] = "Controller state requested.";
                data["heap"] = ESP.getFreeHeap();
                response->setLength();
                request->send(response);
            }
            break;
        case HTTP_POST:
            {
                if (!request->hasParam("v")) {
                    request->send(400, "text/html", "<h1>Missing value ('v') parameter</h1>");
                    return;
                }
    
                AsyncWebParameter* stateName = request->getParam("v");
                if (stateName->value() == "idle") {
                    smokerController->setState(IDLE);
                } else if (stateName->value() == "drying") {
                    smokerController->setState(DRYING);
                } else if (stateName->value() == "smoking") {
                    smokerController->setState(SMOKING);
                } else if (stateName->value() == "thermal") {
                    smokerController->setState(THERMAL);
                } else {
                    request->send(400, "text/html", "<h1>Invalid value ('v') parameter value</h1>");
                    return;
                }
                request->send(204, "text/plain", "");
            }
            break;
        default:
            request->send(405, "text/html", "<h1>Request method not allowed</h1>");
            break;
    }
}

void SmokerWeb::handle_OnApiControllerTemperature(AsyncWebServerRequest *request) {
    if (!isRequestAuthenticated(request)) {
        request->send(401, "text/html", "<h1>Missing API key ('token') parameter</h1>");
        return;
    }
    
    switch (request->method()) {
        case HTTP_GET:
            {
                AsyncJsonResponse* response = new AsyncJsonResponse();
                JsonObject root = response->getRoot();
                JsonObject data = root.createNestedObject("data");
                data["value"] = String(smokerController->getTemperature());
                data["message"] = "Controller temperature requested.";
                data["heap"] = ESP.getFreeHeap();
                response->setLength();
                request->send(response);
            }
            break;
        default:
            request->send(405, "text/html", "<h1>Request method not allowed</h1>");
            break;
    }
}

void SmokerWeb::handle_OnApiSettings(AsyncWebServerRequest *request) {
    if (!isRequestAuthenticated(request)) {
        request->send(401, "text/html", "<h1>Missing API key ('token') parameter</h1>");
        return;
    }
    
    switch (request->method()) {
        case HTTP_GET:
            {
                AsyncJsonResponse* response = new AsyncJsonResponse();
                JsonObject root = response->getRoot();
    
                JsonObject relays = root.createNestedObject("relays");
                relays["power"] = this->smokerSettings->relays->powerRelay;
                relays["smoke"] = this->smokerSettings->relays->smokeRelay;
    
                JsonObject times = root.createNestedObject("times");
                times["drying"] = this->smokerSettings->times->dryingTime;
                times["smoking"] = this->smokerSettings->times->smokingTime;
                times["thermal"] = this->smokerSettings->times->thermalTime;
                times["turbo"] = this->smokerSettings->times->turboSmokingTime;
    
                JsonObject temperatures = root.createNestedObject("temperatures");
                JsonObject temperatures_drying = temperatures.createNestedObject("drying");
                temperatures_drying["min"] = this->smokerSettings->temperatures->dryingMinTemp;
                temperatures_drying["max"] = this->smokerSettings->temperatures->dryingMaxTemp;
                JsonObject temperatures_smoking = temperatures.createNestedObject("smoking");
                temperatures_smoking["min"] = this->smokerSettings->temperatures->smokingMinTemp;
                temperatures_smoking["max"] = this->smokerSettings->temperatures->smokingMaxTemp;
                JsonObject temperatures_thermal = temperatures.createNestedObject("thermal");
                temperatures_thermal["min"] = this->smokerSettings->temperatures->thermalMinTemp;
                temperatures_thermal["max"] = this->smokerSettings->temperatures->thermalMaxTemp;
                temperatures["temp_pin"] = this->smokerSettings->temperatures->tempPin;
                
                JsonObject system = root.createNestedObject("system");
                system["heap"] = ESP.getFreeHeap();

                response->setLength();
                request->send(response);
            }
            break;
        default:
            request->send(405, "text/html", "<h1>Request method not allowed</h1>");
            break;
    }
}

void SmokerWeb::handle_OnApiSettingsTemperature(AsyncWebServerRequest *request) {
    if (!isRequestAuthenticated(request)) {
        request->send(401, "text/html", "<h1>Missing API key ('token') parameter</h1>");
        return;
    }
    
    if (!request->hasParam("t")) {
        request->send(400, "text/html", "<h1>Missing type ('t') parameter</h1>");
        return;
    }
    if (!request->hasParam("n")) {
        request->send(400, "text/html", "<h1>Missing name ('n') parameter</h1>");
        return;
    }

    AsyncWebParameter* tempName = request->getParam("n");
    AsyncWebParameter* tempType = request->getParam("t");

    switch (request->method()) {
        case HTTP_GET:
            {
                uint16_t tempValue;
                if (tempType->value() == "min") {
                    if (tempName->value() == "drying") {
                        tempValue = smokerController->getSmokerSettingsTemperature(DRYING_MIN_TEMP_SETTING);
                    } else if (tempName->value() == "smoking") {
                        tempValue = smokerController->getSmokerSettingsTemperature(SMOKING_MIN_TEMP_SETTING);
                    } else if (tempName->value() == "thermal") {
                        tempValue = smokerController->getSmokerSettingsTemperature(THERMAL_MIN_TEMP_SETTING);
                    } else {
                        request->send(400, "text/html", "<h1>Invalid name ('n') parameter value</h1>");
                        return;
                    }
                } else if (tempType->value() == "max") {
                    if (tempName->value() == "drying") {
                        tempValue = smokerController->getSmokerSettingsTemperature(DRYING_MAX_TEMP_SETTING);
                    } else if (tempName->value() == "smoking") {
                        tempValue = smokerController->getSmokerSettingsTemperature(SMOKING_MAX_TEMP_SETTING);
                    } else if (tempName->value() == "thermal") {
                        tempValue = smokerController->getSmokerSettingsTemperature(THERMAL_MAX_TEMP_SETTING);
                    } else {
                        request->send(400, "text/html", "<h1>Invalid name ('n') parameter value</h1>");
                        return;
                    }
                } else {
                    request->send(400, "text/html", "<h1>Invalid type ('t') parameter value</h1>");
                    return;
                }

                AsyncJsonResponse* response = new AsyncJsonResponse();
                JsonObject root = response->getRoot();
                JsonObject data = root.createNestedObject("data");
                data["value"] = tempValue;
                data["message"] = "Settings temperature requested.";
                data["heap"] = ESP.getFreeHeap();
                response->setLength();
                request->send(response);
            }
            break;
        case HTTP_POST:
            {
                if (!request->hasParam("v")) {
                    request->send(400, "text/html", "<h1>Missing value ('v') parameter</h1>");
                    return;
                }
                        
                uint16_t tempValue = request->getParam("v")->value().toInt();
                if (tempType->value() == "min") {
                    if (tempName->value() == "drying") {
                        smokerController->setSmokerSettingsTemperature(DRYING_MIN_TEMP_SETTING, tempValue);
                    } else if (tempName->value() == "smoking") {
                        smokerController->setSmokerSettingsTemperature(SMOKING_MIN_TEMP_SETTING, tempValue);
                    } else if (tempName->value() == "thermal") {
                        smokerController->setSmokerSettingsTemperature(THERMAL_MIN_TEMP_SETTING, tempValue);
                    } else {
                        request->send(400, "text/html", "<h1>Invalid name ('n') parameter value</h1>");
                        return;
                    }
                } else if (tempType->value() == "max") {
                    if (tempName->value() == "drying") {
                        smokerController->setSmokerSettingsTemperature(DRYING_MAX_TEMP_SETTING, tempValue);
                    } else if (tempName->value() == "smoking") {
                        smokerController->setSmokerSettingsTemperature(SMOKING_MAX_TEMP_SETTING, tempValue);
                    } else if (tempName->value() == "thermal") {
                        smokerController->setSmokerSettingsTemperature(THERMAL_MAX_TEMP_SETTING, tempValue);
                    } else {
                        request->send(400, "text/html", "<h1>Invalid name ('n') parameter value</h1>");
                        return;
                    }
                } else {
                    request->send(400, "text/html", "<h1>Invalid type ('t') parameter value</h1>");
                    return;
                }
                request->send(204, "text/plain", "");
            }
            break;
        default:
            request->send(405, "text/html", "<h1>Request method not allowed</h1>");
            break;
    }
}

void SmokerWeb::handle_OnApiSettingsTime(AsyncWebServerRequest *request) {
    if (!isRequestAuthenticated(request)) {
        request->send(401, "text/html", "<h1>Missing API key ('token') parameter</h1>");
        return;
    }
    
    if (!request->hasParam("n")) {
        request->send(400, "text/html", "<h1>Missing name ('n') parameter</h1>");
        return;
    }

    AsyncWebParameter* timeName = request->getParam("n");

    switch (request->method()) {
        case HTTP_GET:
            {
                uint32_t timeValue;
                if (timeName->value() == "drying") {
                    timeValue = smokerController->getSmokerSettingsTime(DRYING_TIME_SETTING);
                } else if (timeName->value() == "smoking") {
                    timeValue = smokerController->getSmokerSettingsTime(SMOKING_TIME_SETTING);
                } else if (timeName->value() == "thermal") {
                    timeValue = smokerController->getSmokerSettingsTime(THERMAL_TIME_SETTING);
                } else {
                    request->send(400, "text/html", "<h1>Invalid name ('n') parameter value</h1>");
                    return;
                }

                AsyncJsonResponse* response = new AsyncJsonResponse();
                JsonObject root = response->getRoot();
                JsonObject data = root.createNestedObject("data");
                data["value"] = timeValue;
                data["message"] = "Settings time requested.";
                data["heap"] = ESP.getFreeHeap();
                response->setLength();
                request->send(response);
            }
            break;
        case HTTP_POST:
            {
                if (!request->hasParam("v")) {
                    request->send(400, "text/html", "<h1>Missing value ('v') parameter</h1>");
                    return;
                }
    
                uint32_t timeValueM = request->getParam("v")->value().toInt();
                uint32_t timeValue;
                
                if (!request->hasParam("u")) {
                    timeValue = minToMs(timeValueM);
                } else {
                    AsyncWebParameter* timeUnit = request->getParam("u");
                    if (timeUnit->value() == "h") 
                        timeValue = hToMs(timeValueM);
                    else if (timeUnit->value() == "m")
                        timeValue = minToMs(timeValueM);
                    else if (timeUnit->value() == "ms")
                        timeValue = timeValueM;                
                }
    
                if (timeName->value() == "drying") {
                    smokerController->setSmokerSettingsTime(DRYING_TIME_SETTING, timeValue);
                } else if (timeName->value() == "smoking") {
                    smokerController->setSmokerSettingsTime(SMOKING_TIME_SETTING, timeValue);
                } else if (timeName->value() == "thermal") {
                    smokerController->setSmokerSettingsTime(THERMAL_TIME_SETTING, timeValue);
                } else {
                    request->send(400, "text/plain", "<h1>Invalid name ('n') parameter value</h1>");
                    return;
                }
    
                request->send(204, "text/plain", "");
            }
            break;
        default:
            request->send(405, "text/html", "<h1>Request method not allowed</h1>");
            break;
    }
}

void SmokerWeb::handle_OnApiSettingsRelay(AsyncWebServerRequest *request) {
    if (!isRequestAuthenticated(request)) {
        request->send(401, "text/html", "<h1>Missing API key ('token') parameter</h1>");
        return;
    }
    
    if (!request->hasParam("n")) {
        request->send(400, "text/html", "<h1>Missing name ('n') parameter</h1>");
        return;
    }

    AsyncWebParameter* relayName = request->getParam("n");

    switch (request->method()) {
        case HTTP_GET:
            {
                uint8_t relayValue;
    
                if (relayName->value() == "power") {
                    relayValue = smokerController->getSmokerSettingsRelay(POWER_RELAY_SETTING);
                } else if (relayName->value() == "smoke") {
                    relayValue = smokerController->getSmokerSettingsRelay(SMOKE_RELAY_SETTING);
                } else {
                    request->send(400, "text/html", "<h1>Invalid name ('n') parameter value</h1>");
                    return;
                }

                AsyncJsonResponse* response = new AsyncJsonResponse();
                JsonObject root = response->getRoot();
                JsonObject data = root.createNestedObject("data");
                data["value"] = relayValue;
                data["message"] = "Settings relay requested.";
                data["heap"] = ESP.getFreeHeap();
                response->setLength();
                request->send(response);
            }
            break;
        default:
            request->send(405, "text/html", "<h1>Request method not allowed</h1>");
            break;
    }
}

void SmokerWeb::handle_OnApiSettingsTemperaturePin(AsyncWebServerRequest *request) {
    if (!isRequestAuthenticated(request)) {
        request->send(401, "text/html", "<h1>Missing API key ('token') parameter</h1>");
        return;
    }
    
    switch (request->method()) {
        case HTTP_GET:
            {
                AsyncJsonResponse* response = new AsyncJsonResponse();
                JsonObject root = response->getRoot();
                JsonObject data = root.createNestedObject("data");
                data["value"] = String(smokerController->getSmokerSettingsTemperature(TEMP_PIN_SETTING));
                data["message"] = "Settings temperature pin requested.";
                data["heap"] = ESP.getFreeHeap();
                response->setLength();
                request->send(response);
            }
            break;
        default:
            request->send(405, "text/html", "<h1>Request method not allowed</h1>");
            break;
    }
}
