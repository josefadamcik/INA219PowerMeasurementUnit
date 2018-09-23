#include "Display.h"

void Display::setup() {
    lcd.init();
}

void Display::printHello() {
    lcd.clear();
    lcd.home ();
    lcd.backlight();
    lcd.print("Hello, INA219");  
}

void Display::clear() {
    lcd.clear();
}

void Display::printValue(String title, float number, String unit) {
    clear();
    lcd.setCursor(0,0);
    lcd.print(title);
    lcd.setCursor(0,1);
    lcd.print(number);
    lcd.setCursor(16-3,1);
    lcd.print(unit);
}
        