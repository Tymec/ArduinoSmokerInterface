#include <LiquidCrystal_I2C.h>

#define LCD_ROWS 2
#define LCD_COLS 16

LiquidCrystal_I2C lcd(0x3F, 16, 2);

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

void setup() {
  lcd.init();
  lcdMenu.init();
  lcd.backlight();
  lcd.clear();
}

void loop() {
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