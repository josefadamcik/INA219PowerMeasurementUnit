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