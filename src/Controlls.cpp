#include "Controlls.h"

void Button::setup(void (*interruptfunc)(void)) volatile {
    pinMode(pin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(pin), interruptfunc, CHANGE);
}


/**
 * Button interrupt and debouncing.
 */ 
void  Button::interrupt() volatile {
    int buttonState = digitalRead(pin);
    if (lastState == HIGH && buttonState == LOW) { // button press, start deboucne
        //start debouncing every time
        changeDebounceStarted = millis();
        debouncing = true;
    } else if (lastState == LOW && buttonState == HIGH) { 
        if (debouncing && changeDebounceStarted + debounceTime < millis()) {
            triggerObserved = true;
            debouncing =  false;
        }
    }

    lastState = buttonState;
}

bool Button::checkIfButtonTriggeredAndReset() volatile {
    if (triggerObserved) {
        triggerObserved = false;
        return true;
    } else {
        return false;
    }
}


