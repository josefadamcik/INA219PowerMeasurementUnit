#include "UserInterface.h"

UserInterface::Screen& operator++(UserInterface::Screen& screen)
{
    assert(screen != UserInterface::LastScr);
    return screen = static_cast<UserInterface::Screen>( screen + 1 );
}

UserInterface::Screen operator++(UserInterface::Screen& screen, int)
{
  assert(screen != UserInterface::LastScr);
  UserInterface::Screen tmp(screen);
  ++screen;
  return tmp;
}

void UserInterface::setup(const Measure::Calibration& calibration) {
  currentCalibration = calibration;
  display.setup();  
  display.printHello(getCalibrationString());
  
}

void UserInterface::updateLastMeasurement(const Measurement& measurement) {
    lastMeasurement = measurement;
}

void UserInterface::loop() {
    if (mode == Auto) {
        loopAuto();
    } else if (mode == User) {
        loopUser();
    }
}

void UserInterface::loopAuto() {
    if (lastAutoChange == 0 || lastAutoChange + autoModeDelay <= millis()) {
        lastAutoChange = millis();
        nextScreen();
        renderScreen(screen);
    }
}

void UserInterface::loopUser() {
    if (mainButtonWasTriggered) {
        mainButtonWasTriggered = false; //consume event
        nextScreen();
        renderScreen(screen);
    } else if (lastUserInteraction + autoUserModeReset <= millis()) {
        //auto quit user mode -> reset to auto mode.
        resetModeToAuto();
        screen = Welcome;
        renderScreen(screen);
    }
}

void UserInterface::resetModeToAuto() {
    mainButtonWasTriggered = false;
    lastUserInteraction = 0;
    lastAutoChange = 0;
    mode = Auto;

}


void UserInterface::nextScreen() {
    if (screen == Power) { //the last one
        screen = Welcome;
    } else {
        screen++;
    }
}

void UserInterface::renderScreen(Screen scrToRender) {
    switch (scrToRender) {
        case Welcome: 
            display.printHello(getCalibrationString());
            break;
        case Voltage:
            display.printValue(F("Voltage"), lastMeasurement.loadvoltage * 1000, F("mV"));
            break;
        case Current:
            display.printValue(F("Current"), lastMeasurement.current_mA, F("mA"));
            break;
        case Power:
            display.printValue(F("Power"), lastMeasurement.power_mW, F("mW"));
            break;
        default:
            display.clear();   
    }
}

const String UserInterface::getCalibrationString() const {
    switch(currentCalibration) {
        case Measure::C16V_400:
            return F("16V 400mA");
        case Measure::C32V_1A:
            return F("32V 1A");
        case Measure::C32V_2A:
            return F("32V 2A");
    }
    return F("n/a");
}

void UserInterface::mainButtonTriggered() {
    if (mode == Auto) {
        mode = User;
    }

    lastUserInteraction = millis();
    mainButtonWasTriggered = true;

}



void UserInterface::updateCalibration(const Measure::Calibration& calibration) {
    currentCalibration = calibration;
    resetModeToAuto();
    screen = None;
} 


