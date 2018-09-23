#include "Display.h"

void Display::setup() {
    lcd.init();
}

void Display::printHello(const String& secondRow) {
    lcd.clear();
    lcd.home ();
    lcd.backlight();
    lcd.print("Hello, INA219");  
    lcd.setCursor(0,1);
    lcd.print(secondRow);
}

void Display::clear() {
    lcd.clear();
}

void Display::printValue(const String& title, const float& number, const String& unit) {
    clear();
    lcd.setCursor(0,0);
    lcd.print(title);
    lcd.setCursor(0,1);
    lcd.print(number);
    lcd.setCursor(14,1);
    lcd.print(unit);
}
        