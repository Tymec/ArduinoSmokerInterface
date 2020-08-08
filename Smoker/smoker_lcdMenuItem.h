#pragma once

enum MenuType {
    Default,
    Header,
    Footer,
    SubHeader,
    Sub,
    Input,
    Toggle
};

typedef void (*OptionFunction)();

class MenuOption {
    public:
        MenuOption() = default;
        MenuOption(char* text) : text(text) {}
        MenuOption(MenuOption* subMenu, MenuType menuType) : subMenu(subMenu), menuType(menuType) {}
        MenuOption(char* text, MenuOption* subMenu) : text(text), subMenu(subMenu) {}
        MenuOption(char* text, OptionFunction callback) : text(text), callback(callback) {}
        MenuOption(char* text, OptionFunction callback, MenuType menuType) : text(text), callback(callback), menuType(menuType) {}
        MenuOption(char* text, MenuOption* subMenu, MenuType menuType) : text(text), subMenu(subMenu), menuType(menuType) {}
        MenuOption(char* text, OptionFunction callback, MenuOption* subMenu) : text(text), callback(callback), subMenu(subMenu) {}
        MenuOption(char* text, OptionFunction callback, MenuOption* subMenu, MenuType menuType) : text(text), callback(callback), subMenu(subMenu), menuType(menuType) {}

        char* getText() { return this->text; }
        char* getValue() { return this->value; }
        OptionFunction getCallback() { return this->callback; }
        MenuOption* getSubmenu() { return this->subMenu; }
        MenuType getType() { return this->menuType; }

        void setText(char* text) { this->text = text; }
        void setValue(char* value) { this->value = value; }
        void setCallback(OptionFunction callback) { this->callback = callback; }
        void setSubmenu(MenuOption* subMenu) { this->subMenu = subMenu; }
    private:
        char* text = "";
        char* value = "";
        OptionFunction callback = nullptr;
        MenuType menuType = Default;
        MenuOption* subMenu = nullptr;
};

class MenuHeader : public MenuOption {
    public:
        MenuHeader() : MenuOption(this, Header) {}
};

class MenuFooter : public MenuOption {
    public:
        MenuFooter() : MenuOption(nullptr, Footer) {}
};

class MenuSubHeader : public MenuOption {
    public:
        MenuSubHeader(char* text, MenuOption* parent) : MenuOption(text, parent, SubHeader) {}
};

class MenuSub : public MenuOption {
   public:
        MenuSub(char* text, MenuOption* parent) : MenuOption(text, parent, Sub) {}
        MenuSub(char* text, OptionFunction callback, MenuOption* parent) : MenuOption(text, callback, parent, Sub) {}
};

class MenuToggle : public MenuOption {
    public:
        MenuToggle(char* text, OptionFunction callback) : MenuOption(text, callback, Toggle) {}
};

class MenuInput : public MenuOption {
    public:
        MenuInput(char* text, OptionFunction callback) : MenuOption(text, callback, Input) {}
};