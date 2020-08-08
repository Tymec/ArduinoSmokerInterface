#include <Debounce.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>
#include <Arduino_FreeRTOS.h>

#include "smoker_helper.h"
#include "smoker_lcdMenu.h"

#define TEMP_PIN 5
#define E_BTN 4
#define W_BTN 7
#define S_BTN 8
#define RELAY_1 10
#define RELAY_2 9
#define RELAY_3 8

#define LCD_ROWS 2
#define LCD_COLS 16
#define DEBOUNCE_DELAY 50

float temperature;
short smokingTime = 10;

OneWire oneWire(TEMP_PIN);
DallasTemperature tempSensor(&oneWire);
LiquidCrystal_I2C lcd(0x3F, 16, 2);
Debounce eButton(E_BTN, DEBOUNCE_DELAY);
Debounce wButton(W_BTN, DEBOUNCE_DELAY);
Debounce sButton(S_BTN, DEBOUNCE_DELAY);

typedef struct {
  float time;
  unsigned short tempValue;
  unsigned short tempMax;
  unsigned short tempMin;
  unsigned short tempOffset;
} TempControlData;
TempControlData tempControlData;

void taskTempControl    ( void *pvParameters );
void taskSmokingControl ( void *pvParameters );

TaskHandle_t taskTempControl_Handler;
TaskHandle_t taskSmokingControl_Handler;

namespace menuCallback {
  bool backlight = true;
  void callback_backlight() {
    backlight = !backlight;
    lcd.setBacklight(backlight);
  }

  bool contrast = true;
  void callback_contrast() {
    contrast = !contrast;
    lcd.setContrast(contrast);
  }
};

extern MenuOption mainMenu[];
/*
extern MenuOption profileMenu[];
extern MenuOption settingsMenu[];
extern MenuOption temperatureMenu[];
extern MenuOption timeMenu[];
*/

MenuOption mainMenu[] = {
  MenuSub("Profiles", nullptr),
  MenuSub("Settings", nullptr),
  MenuOption("Debug"),
  MenuFooter()
};
/*
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
  MenuToggle("Contract", menuCallback::callback_contrast),
  MenuToggle("Backlight", menuCallback::callback_backlight),
  MenuSub("Temperatures", temperatureMenu),
  MenuSub("Times", timeMenu),
  MenuOption("Wi-Fi"),
  MenuFooter()
};
*/
LcdMenu lcdMenu(&lcd, 2, 16, mainMenu);
bool sPressed, wPressed, ePressed;

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.clear();

  pinMode(TEMP_PIN, INPUT);
  pinMode(RELAY_1, OUTPUT);
  pinMode(RELAY_2, OUTPUT);
  pinMode(RELAY_3, OUTPUT);

  while (true) {
    lcdMenu.draw();

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

}

void loop() {
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

  bool sPressed, wPressed;
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

void startDrying() {
  // RELAY 1: 45-90min
  // RELAY 2: 30-40*C
  // RELAY 3: OFF
  lcd.setCursor(0, 0);
  lcd.print("Drying");
  
  // Set time between 50min to 90min (User input)
  float time = getInput("Time (45-90):", "min", 45, 90, 15);

  tempControlData.time = time;
  tempControlData.tempValue = 35;
  tempControlData.tempOffset = 5;

  xTaskCreate(
    taskTempControl,
    "Temp Control",
    128,
    (void*) &tempControlData,
    2,
    &taskTempControl_Handler
  );

  return;
}

void startSmoking() {
  // RELAY 1: 10min / 2-4h
  // RELAY 2: 50-65*C
  // RELAY 3: 10min / OFF
  lcd.setCursor(0, 0);
  lcd.print("Smoking");

  // Set time between 2h to 4h (User input)
  float time = getInput("Time (2-4):", "h", 2, 4, .5);
  
  return;
}

void startThermal() {
  // RELAY 1: 2-4h
  // RELAY 2: 80-90*C
  // RELAY 3: OFF
  lcd.setCursor(0, 0);
  lcd.print("Thermal");
  
  // Set time between 2h to 4h (User input)
  float time = getInput("Time (2-4):", "h", 2, 4, .5);
  
  return;
}

void taskTempControl (void *pvParameters) {
  TempControlData *data = (TempControlData*) pvParameters;
  Serial.println(data->tempValue);
}

void taskSmoking (void *pvParameters) {
  (void*) pvParameters;
}
