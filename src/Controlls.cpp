#include "Controlls.h"

void Button::setup() volatile {
    pinMode(pin, INPUT_PULLUP);
}

/**
 * Button interrupt and debouncing.
 */
void Button::interrupt() volatile {
    int buttonState = digitalRead(pin);
    if (buttonState == LOW && lastState == HIGH) {
       changeDebounceStarted = millis();
       debouncing = true;
    } else if (buttonState == HIGH) {
        debouncing = false;
    } 
    lastState = buttonState;
}

bool Button::checkIfButtonTriggeredAndReset() volatile {
    if (digitalRead(pin) == LOW 
        && debouncing 
        && changeDebounceStarted + debounceTime < millis()) {
            debouncing = false;
            triggerObserved = true;
    }
    if (triggerObserved) {
        triggerObserved = false;
        return true;
    } else {
        return false;
    }
}
