#include <Arduino.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>


#ifndef INC_DISPLAY
#define INC_DISPLAY


class Display {
    public:
        Display(uint8_t addr): lcd(addr, 16, 2) {};
        void setup();
        void printHello();
    private:
        LiquidCrystal_I2C lcd;  
};

#endif