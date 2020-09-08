#include <Debounce.h>
#include <LiquidCrystal_I2C.h>

#include "SmokerController.h"
#include "smokerHelper.h"
#include "LcdMenu.h"
#include "SmokerWeb.h"

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
  90, // THERMAL MAX TEMP
  D5   // TEMP PIN
};
RelaySettings relaySettings = { 
  D4, // POWER RELAY
  D3   // SMOKE RELAY
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

#define LCD_ROWS 2
#define LCD_COLS 16
#define DEBOUNCE_DELAY 50

LiquidCrystal_I2C lcd(0x3F, 16, 2);
Debounce eButton(E_BTN, DEBOUNCE_DELAY);
Debounce wButton(W_BTN, DEBOUNCE_DELAY);
Debounce sButton(S_BTN, DEBOUNCE_DELAY);

namespace menuCallback {
  bool backlight = true;
  void callback_backlight(void* pv) {
    ToggleProperties* tPv = (ToggleProperties*) pv;
    Serial.println(tPv->getTextOff());

    backlight = !backlight;
    lcd.setBacklight(backlight);
  }

  bool contrast = true;
  void callback_contrast(void* pv) {
    contrast = !contrast;
    lcd.setContrast(contrast);
  }
};

extern MenuOption mainMenu[];
extern MenuOption profileMenu[];
extern MenuOption settingsMenu[];
extern MenuOption temperatureMenu[];
extern MenuOption timeMenu[];

MenuOption mainMenu[] = {
  MenuSub("Profiles", profileMenu),
  MenuSub("Settings", settingsMenu),
  MenuOption("Debug"),
  MenuFooter()
};
MenuOption profileMenu[] = {
  MenuSubHeader("Back", mainMenu),
  MenuInput("Drying", nullptr),
  MenuInput("Smoking", nullptr),
  MenuInput("Thermal", nullptr),
  MenuFooter()
};
MenuOption temperatureMenu[] = {
  MenuSubHeader("Back", settingsMenu),
  MenuInput("Drying", nullptr),
  MenuInput("Smoking", nullptr),
  MenuInput("Thermal", nullptr),
  MenuFooter()
};
MenuOption timeMenu[] = {
  MenuSubHeader("Back", settingsMenu),
  MenuInput("Drying", nullptr),
  MenuInput("Smoking", nullptr),
  MenuInput("Thermal", nullptr),
  MenuFooter()
};
MenuOption settingsMenu[] = {
  MenuSubHeader("Back", mainMenu),
  MenuToggle("Contract", menuCallback::callback_contrast, ToggleProperties("On", "Off", true)),
  MenuToggle("Backlight", menuCallback::callback_backlight, ToggleProperties("On", "Off", true)),
  MenuSub("Temperatures", temperatureMenu),
  MenuSub("Times", timeMenu),
  MenuOption("Wi-Fi"),
  MenuFooter()
};
LcdMenu lcdMenu(&lcd, 2, 16, mainMenu, &smokerController);

WebConfig webConfig = {
  "ASUS_2Ghz",        // SSID
  "Maranata2017",     // PASS

  "SmokerAP",         // AP SSID
  "Smoker2017",       // AP PASS

  "6jUKI1F7NvhWoNF",  // WWW API TOKEN 

  "smerk0r",          // OTA PASS
  "smokerMCU",        // OTA HOSTNAME
  "8266",             // OTA PORT

  "smoker",           // DOMAIN NAME
  80                  // WEB PORT
};
SmokerWeb smokerWeb(&webConfig, &smokerController);

bool sPressed, wPressed, ePressed, controlsLocked;

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcdMenu.init();
  smokerWeb.init();
  lcd.backlight();
  lcd.clear();

  //smokerController.setState(DRYING);
}

void loop() {
  smokerController.update();
  smokerWeb.update();

  /*
  lcdMenu.draw();

  if (!controlsLocked) {
    if (eButton.read() && !ePressed) {
      lcdMenu.moveEnter();
      ePressed = true;
    } else if (wButton.read() && !wPressed) {
      lcdMenu.moveUp();
      wPressed = true;
    } else if (sButton.read() && !sPressed) {
      lcdMenu.moveDown();
      sPressed = true;
    } 
    
    if (!eButton.read() && ePressed) {
      ePressed = false;
    } else if (!wButton.read() && wPressed) {
      wPressed = false;
    } else if (!sButton.read() && sPressed) {
      sPressed = false;
    }
  }
  */
}

float getInput(char* prompt, char* unit, uint16_t minValue, uint16_t maxValue, float stepValue) {
  // -        0x2D
  // ->       0x7E
  // <-       0x7F
  // <        0x3C 
  // >        0x3E
  // dot      0xA5
  // degrees  0xA1
  lcd.clear();
  
  lcd.setCursor(0, 0);
  lcd.print(prompt);

  lcd.setCursor(0, 1);
  lcd.print((char)0x3C);

  float input = minValue;
  float prevInput;
  
  while (!eButton.read()) {
    if (input >= maxValue) {
      input = maxValue;
    } else if (input <= minValue) {
      input = minValue;
    }

    if (prevInput != input) {
      lcd.setCursor(1, 1);
      lcd.print(input);

      lcd.setCursor(getSizeOfFloat(input) + 1, 1);
      lcd.print(unit);

      lcd.setCursor(getSizeOfFloat(input) + strlen(unit) + 1, 1);
      lcd.print((char)0x3E);

      prevInput = input;
    }

    if (wButton.read() && !wPressed) {
      input += stepValue;
      wPressed = true;
    } else if (sButton.read() && !sPressed) {
      input -= stepValue;
      sPressed = true;
    } 
    if (!wButton.read() && wPressed) {
      wPressed = false;
    } else if (!sButton.read() && sPressed) {
      sPressed = false;
    }

  }
  lcd.clear();
  return input;
}
