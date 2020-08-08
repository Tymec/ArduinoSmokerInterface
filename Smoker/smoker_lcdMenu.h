#pragma once
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Arduino.h>
#include "smoker_lcdMenuItem.h"

class LcdMenu {
    public:
        LcdMenu(LiquidCrystal_I2C* lcd, uint8_t lcdRows, uint8_t lcdCols, MenuOption* menu);

        void draw();

        uint8_t getIndex();
        void setIndex(uint8_t val);
        void incIndex(uint8_t val);
        
        void moveEnter();
        void moveExit();
        void moveUp();
        void moveDown();
    private:
        void init();
        void drawCursor();
        void drawScrollbar();
        void drawMenu();

        bool isMenuEnd(uint8_t idx);
        void forceRefresh();

        void callOptionFunction();
        void callOptionSubmenu();

        byte _downArrow[8] = {
            0b00100,
            0b00100,
            0b00100,
            0b00100,
            0b00100,
            0b10101,
            0b01110,
            0b00100
        };
        byte _upArrow[8] = {
            0b00100,
            0b01110,
            0b10101,
            0b00100,
            0b00100,
            0b00100,
            0b00100,
            0b00100
        };
        
        LiquidCrystal_I2C* lcd;
        MenuOption *mainMenu, *currentMenu;
        uint8_t lcdRows, lcdCols;

        uint8_t previousIndex = 255;
        uint8_t previousCursorPosition = 255;

        uint8_t index = 0;
        uint8_t cursorPosition = 0;
};
