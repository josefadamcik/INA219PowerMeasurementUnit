#include "Display.h"
#include "assert.h"

//see https://maxpromer.github.io/LCD-Character-Creator/
uint8_t arrow[8] = {0x00, 0x08, 0x0C, 0x0E, 0x0F, 0x0E, 0x0C, 0x08};
void Display::setup() {
    lcd.init();
    lcd.createChar(0, arrow);
}

void Display::printHello(const char* secondRow) {
    lcd.clear();
    lcd.home ();
    lcd.backlight();
    lcd.print(F("Hello, INA219"));  
    lcd.setCursor(0,1);
    lcd.print(secondRow);
}

void Display::clear() {
    lcd.clear();
}

void Display::printValue(const char* title, const float& number,
                         const char* unit) {
    clear();
    lcd.setCursor(0,0);
    lcd.print(title);
    lcd.setCursor(0,1);
    lcd.print(number);
    lcd.setCursor(16 - strlen(unit),1);
    lcd.print(unit);
}


void Display::printMenuRow(byte row, bool selected, const char* content) {
    assert(row == 0 || row == 1);
    lcd.setCursor(0, row);
    if (selected) {
        lcd.write(0);
        lcd.print(F(" "));
    } else {
        lcd.print(F("  "));
    }
    lcd.print(content);
}