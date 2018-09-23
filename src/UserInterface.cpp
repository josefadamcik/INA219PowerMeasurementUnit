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

void UserInterface::setup() {
  display.setup();  
  display.printHello();
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
        lastUserInteraction = 0;
        mode = Auto;
        screen = Welcome;
        renderScreen(screen);
    }
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
            display.printHello();
            break;
        case Voltage:
            display.printValue("Voltage", lastMeasurement.loadvoltage, "mV");
            break;
        case Current:
            display.printValue("Current", lastMeasurement.current_mA, "mA");
            break;
        case Power:
            display.printValue("Power", lastMeasurement.power_mW, "mW");
            break;
        default:
            display.clear();   
    }
}

void UserInterface::mainButtonTriggered() {
    if (mode == Auto) {
        mode = User;
    }

    lastUserInteraction = millis();
    mainButtonWasTriggered = true;

}





