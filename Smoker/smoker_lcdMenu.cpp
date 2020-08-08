#include "smoker_lcdMenu.h"

LcdMenu::LcdMenu(LiquidCrystal_I2C* lcd, uint8_t lcdRows, uint8_t lcdCols, MenuOption* menu) {
    this->lcd = lcd;
    this->mainMenu = menu;
    this->lcdRows = lcdRows;
    this->lcdCols = lcdCols;
    init();
}

void LcdMenu::init() {
    lcd->createChar(0, _upArrow);
    lcd->createChar(1, _downArrow);
    this->currentMenu = this->mainMenu;
}

void LcdMenu::drawScrollbar() {
    if (cursorPosition != previousCursorPosition) {
        if ((index - cursorPosition) != 0) {
            lcd->setCursor(lcdCols - 1, 0);
            lcd->write(0x00);
        }
        if (!isMenuEnd(index + 1)) {
            lcd->setCursor(lcdCols - 1, lcdRows - 1);
            lcd->write(0x01);
        }
    }
}

void LcdMenu::drawCursor() {
    LiquidCrystal_I2C* lcd = this->lcd;

    if (previousCursorPosition != cursorPosition) {
        for (uint8_t i = 0; i < lcdRows; i++) {
            lcd->setCursor(0, i);
            lcd->write(' ');
        }

        lcd->setCursor(0, cursorPosition);
        lcd->write((char)0x7E);

        previousCursorPosition = cursorPosition;
    }
}

void LcdMenu::drawMenu() {
    LiquidCrystal_I2C* lcd = this->lcd;

    if (previousIndex != index) {
        lcd->clear();
        for (uint8_t i = 0; i < lcdRows; i++) {
            //if ((index - cursorPosition + i) > menuSize) {
            //    break;
            //}
            if (isMenuEnd(index)) {
                break;
            }

            lcd->setCursor(1, i);
            //lcd->print(currentMenu[index - cursorPosition + i].text);
            lcd->print(currentMenu[index - cursorPosition + i].getText());
        }
        previousIndex = index;
    }
}

void LcdMenu::draw() {
    cursorPosition = index % lcdRows;
    drawMenu();
    drawScrollbar();
    drawCursor();
}

uint8_t LcdMenu::getIndex() {
    return this->index;
}

void LcdMenu::setIndex(uint8_t val) {
    this->index = val;
}

void LcdMenu::incIndex(uint8_t val) {
    index += val;
}

bool LcdMenu::isMenuEnd(uint8_t idx) {
    if (currentMenu[idx].getType() == Footer) {
        return true;
    }
    return false;
}

void LcdMenu::forceRefresh() {
    previousCursorPosition = 255;
    previousIndex = 255;
}

void LcdMenu::callOptionFunction() {
    OptionFunction optionCallback = currentMenu[index].getCallback();
    if (optionCallback != nullptr) {
        optionCallback();
    }
}

void LcdMenu::callOptionSubmenu() {

    MenuOption* optionSubmenu = currentMenu[index].getSubmenu();
    if (optionSubmenu != nullptr) {
        currentMenu = optionSubmenu;
        index = 0;
        cursorPosition = 0;

        forceRefresh();
    }
}

void LcdMenu::moveEnter() {
    //Serial.println(currentMenu[index].getText());

    callOptionFunction();
    callOptionSubmenu();
    /*
    switch (currentMenu[index].getType()) {
        case Footer:
            break;
        case Input:
            break;
        case Toggle:
            MenuToggleProperties *properties = (MenuToggleProperties*) currentMenu[index].getProperties();
            Serial.println(properties->getState());
            break;
        case Default:
        case SubHeader:
        case Sub:
            
        case Header:

        default:
            
            break;
    }
    */
}

void LcdMenu::moveExit() {
    return;
}

void LcdMenu::moveUp() {
    if (index <= 0)
        index = 0;
    else
        index--;
}

void LcdMenu::moveDown() {
    if (isMenuEnd(index + 1))
        return;
    else
        index++;
}