#include "Display.h"
#include "assert.h"

//see https://maxpromer.github.io/LCD-Character-Creator/
uint8_t arrow[8] = {0x00, 0x08, 0x0C, 0x0E, 0x0F, 0x0E, 0x0C, 0x08};
void Display::setup() {
    lcd.init();
    lcd.createChar(0, arrow);
}

const char str_hello[] PROGMEM = "Hello, INA219";
const char str_space[] PROGMEM = " ";

void Display::printHello(const char* calibration, const int interval) {
    lcd.clear();
    lcd.home ();
    lcd.backlight();
    printProgmem(str_hello);
    lcd.setCursor(0,1);
    printProgmem(calibration);
    printProgmem(str_space);
    lcd.print(interval);
}

void Display::printProgmem(const char* text) {
    static char buffer[17];
    strcpy_P(buffer, text);
    lcd.print(buffer);
}

void Display::clear() {
    lcd.clear();
}

void Display::printValue(const char* title, const float& number,
                         const char* unit) {
    clear();
    lcd.setCursor(0,0);
    printProgmem(title);
    lcd.setCursor(0,1);
    lcd.print(number);
    lcd.setCursor(16 - strlen_P(unit),1);
    printProgmem(unit);
}


void Display::printMenuRow(byte row, bool selected, const char* content) {
    assert(row == 0 || row == 1);
    lcd.setCursor(0, row);
    if (selected) {
        lcd.write(0);
        printProgmem(str_space);
    } else {
        printProgmem(str_space);
        printProgmem(str_space);
    }
    printProgmem(content);
}