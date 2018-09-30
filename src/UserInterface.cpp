#include "UserInterface.h"

// PUBLIC

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

void UserInterface::updateCalibration(const Measure::Calibration& calibration) {
    currentCalibration = calibration;
    resetModeToAuto();
    screen = None;
} 

void UserInterface::buttonTriggered(UserInterface::Button button) {
    switch (button) {
        case UserInterface::Primary:
            if (mode == Auto) {
                mode = User;
            } else if (mode == User) {
                lastUserInteraction = millis();
                mainButtonWasTriggered = true;
            }
            break;
        case UserInterface::Secondary:
            break;
    }
}


// PRIVATE

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

UserInterface::MenuLevel1Screen& operator++(UserInterface::MenuLevel1Screen& screen)
{
    assert(screen != UserInterface::Exit);
    return screen = static_cast<UserInterface::MenuLevel1Screen>( screen + 1 );
}

UserInterface::MenuLevel1Screen operator++(UserInterface::MenuLevel1Screen& screen, int)
{
  assert(screen != UserInterface::Exit);
  UserInterface::MenuLevel1Screen tmp(screen);
  ++screen;
  return tmp;
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
    if (screen == LastScr - 1) { //the last one
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
        case Energy:
            display.printValue(F("Energy"), lastMeasurement.energy_mWh, F("mWh"));
            break;
        case EnergyTime:
            display.printValue(F("Energy time"), lastMeasurement.energyMillis / 1000, F("s"));
            break;
        default:
            display.clear();   
    }
}

