#ifndef LCD_MENU_OPTION_H
#define LCD_MENU_OPTION_H

enum MenuType {
    Placeholder,
    Header,
    Footer,
    SubHeader,
    Sub,
    Input,
    Toggle
};

class ToggleProperties{
    public:
        ToggleProperties(char* textOn, char* textOff, bool isOn) : _textOn(textOn), _textOff(textOff), _isOn(isOn) {}
        char* getTextOff() { return this->_textOff; }
        char* getTextOn() { return this->_textOn; }
        bool isOn() { return this->_isOn; }
    private:
        char* _textOn;
        char* _textOff;
        bool _isOn;
};

typedef void (*OptionFunction)(void*);
//typedef void (*OptionFunction)();

class MenuOption {
    public:
        MenuOption() = default;
        MenuOption(char* text) : text(text) {}
        MenuOption(MenuOption* subMenu) : subMenu(subMenu) {}
        MenuOption(char* text, MenuOption* subMenu) : text(text), subMenu(subMenu) {}
        MenuOption(char* text, OptionFunction callback) : text(text), callback(callback) {}
        MenuOption(char* text, OptionFunction callback, MenuOption* subMenu) : text(text), callback(callback), subMenu(subMenu) {}

        char* getText() { return this->text; }
        void* getValue() { return this->value; }
        OptionFunction getCallback() { return this->callback; }
        MenuOption* getSubmenu() { return this->subMenu; }
        MenuType getType() { return this->menuType; }

        void setText(char* text) { this->text = text; }
        void setValue(void* value) { this->value = value; }
        void setCallback(OptionFunction callback) { this->callback = callback; }
        void setSubmenu(MenuOption* subMenu) { this->subMenu = subMenu; }
        void setType(MenuType menuType) { this->menuType = menuType; }
    private:
        char* text = "";
        void* value = nullptr;
        OptionFunction callback = nullptr;
        MenuType menuType = Placeholder;
        MenuOption* subMenu = nullptr;
};

class MenuHeader : public MenuOption {
    public:
        MenuHeader() : MenuOption(this) {
            setType(Header);
        }
};

class MenuFooter : public MenuOption {
    public:
        MenuFooter() : MenuOption((MenuOption*)nullptr) {
            setType(Footer);
        }
};

class MenuSubHeader : public MenuOption {
    public:
        MenuSubHeader(char* text, MenuOption* parent) : MenuOption(text, parent) {
            setType(SubHeader);
        }
};

class MenuSub : public MenuOption {
   public:
        MenuSub(char* text, MenuOption* parent) : MenuOption(text, parent) {
            setType(Sub);
        }
        MenuSub(char* text, OptionFunction callback, MenuOption* parent) : MenuOption(text, callback, parent) {
            setType(Sub);
        }
};

class MenuToggle : public MenuOption {
    public:
        MenuToggle(char* text, OptionFunction callback, ToggleProperties value) : MenuOption(text, callback) {
            setType(Toggle);
            setValue(&value);
        }
};

class MenuInput : public MenuOption {
    public:
        MenuInput(char* text, OptionFunction callback) : MenuOption(text, callback) {
            setType(Input);
        }
};
#endif