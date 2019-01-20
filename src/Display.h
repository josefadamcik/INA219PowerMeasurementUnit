#include <Arduino.h>
#include <LiquidCrystal_I2C.h>


#ifndef INC_DISPLAY
#define INC_DISPLAY


class Display {
    public:
        Display(uint8_t addr): lcd(addr, 16, 2) {};
        void setup();
        void printHello(const char* secondRow);
        void printValue(const char* title, const float& number,
                        const char* unit);
        void printMenuRow(byte row, bool selected, const char* content);
        void clear();
        LiquidCrystal_I2C lcd;

       private:

        // Prevent copying
        Display & operator = (const Display & other);
        Display(const Display & other);
};

#endif