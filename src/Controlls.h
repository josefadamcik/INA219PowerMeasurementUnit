#include <Arduino.h>

#ifndef INC_CONTROLLS
#define INC_CONTROLLS

/**
 * Encapsulates a button (button should bring pin to ground, pin has a pull up resistor).
 * Class supports debouncing, but its client is required to attach interrupt himself and call interrupt() on the instance.
 */
class Button {
    public:
        Button(int pin): pin(pin) {};
        void setup() volatile;
        void interrupt() volatile;
        bool checkIfButtonTriggeredAndReset() volatile;
        const int pin;
    private:
        const unsigned long debounceTime = 150;
        volatile boolean debouncing = false;
        volatile int lastState = HIGH;
        volatile unsigned long changeDebounceStarted;
        volatile boolean triggerObserved = false;
};

class Controlls {
    public:
        Controlls(int pinA, int pinB):  buttonA(pinA), buttonB(pinB) {};
        void setup() volatile;
        volatile Button buttonA;
        volatile Button buttonB;
    private:
};



#endif //ifndef INC_CONTROLLS