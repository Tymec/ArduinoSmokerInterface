#ifndef SMOKER_WEB_H
#define SMOKER_WEB_H
#include <Arduino.h>
#include <ESP8266WiFi.h>
//#include <ESP8266WebServer.h>
#include "ESPAsyncTCP.h"
#include <ESPAsyncWebServer.h>
#include <ESP8266mDNS.h>
//#include <WiFiUdp.h>
//#include <ArduinoOTA.h>
#include <AsyncJson.h>
#include <ArduinoJson.h>

//#include <uri/UriBraces.h>
//#include <uri/UriRegex.h>

#include "SmokerController.h"

typedef struct {
    char* ssid;
    char* pass;

    char* apSsid;
    char* apPass;

    char* wwwApiToken;

    char* otaPass;
    char* otaHost;
    char* otaPort;

    char* domainName;
    uint16_t webPort;
} WebConfig;

class SmokerWeb {
    public:
        SmokerWeb(WebConfig* webConfig, SmokerController* smokerController);
        void init();
        void update();
        String getIP();
    private:
        bool isConnected();
        bool isRequestAuthenticated(AsyncWebServerRequest *request);

        /* ArduinoOTA
        void ota_OnStart();
        void ota_OnEnd();
        void ota_onProgress(unsigned int progress, unsigned int total);
        void ota_onError(ota_error_t error);
        */
        /* WebSocket
        AsyncWebSocket* ws;
        void ws_OnEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len);
        */

        void handle_OnNotFound(AsyncWebServerRequest *request);
        void handle_OnHome(AsyncWebServerRequest *request);
        void handle_OnSettings(AsyncWebServerRequest *request);
 
        void handle_OnApiControllerRelay(AsyncWebServerRequest *request);
        void handle_OnApiControllerState(AsyncWebServerRequest *request);
        void handle_OnApiControllerTemperature(AsyncWebServerRequest *request);

        void handle_OnApiSettings(AsyncWebServerRequest *request);
        void handle_OnApiSettingsTemperature(AsyncWebServerRequest *request);
        void handle_OnApiSettingsTime(AsyncWebServerRequest *request);
        void handle_OnApiSettingsRelay(AsyncWebServerRequest *request);
        void handle_OnApiSettingsTemperaturePin(AsyncWebServerRequest *request);
        
        SmokerController* smokerController;
        SmokerSettings* smokerSettings;
        AsyncWebServer* server;
        WebConfig* webConfig;
};
#endif
