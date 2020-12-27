#ifndef LCD_MENU_H
#define LCD_MENU_H
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Arduino.h>
#include "LcdMenuOption.h"
#include "SmokerController.h"

class LcdMenu {
    public:
        LcdMenu(LiquidCrystal_I2C* lcd, uint8_t lcdRows, uint8_t lcdCols, MenuOption* menu, SmokerController* smokerController);
        
        void init();
        void draw();

        uint8_t getIndex();
        void setIndex(uint8_t val);
        void incIndex(uint8_t val);
        
        void moveEnter();
        void moveExit();
        void moveUp();
        void moveDown();
    private:
        void drawCursor();
        void drawScrollbar();
        void drawMenu();

        bool isMenuEnd(uint8_t idx);
        void forceRefresh();

        void callOptionFunction();
        void callOptionSubmenu();
        
        LiquidCrystal_I2C* lcd;
        SmokerController* smokerController;
        MenuOption *mainMenu, *currentMenu;
        uint8_t lcdRows, lcdCols;

        uint8_t previousIndex = 255;
        uint8_t previousCursorPosition = 255;

        uint8_t index = 0;
        uint8_t cursorPosition = 0;
};
#endif
