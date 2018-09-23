#include <Arduino.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>


#ifndef INC_DISPLAY
#define INC_DISPLAY


class Display {
    public:
        Display(uint8_t addr): lcd(addr, 16, 2) {};
        void setup();
        void printHello(const String& secondRow);
        void printValue(const String& title, const float& number, const String& unit);
        void clear();
    private:
        LiquidCrystal_I2C lcd;  
        // Prevent copying
        Display & operator = (const Display & other);
        Display(const Display & other);
};

#endif