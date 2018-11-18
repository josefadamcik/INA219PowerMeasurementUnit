#include "UserInterface.h"

// PUBLIC

void UserInterface::setup(const Measure::Calibration& calibration) {
  currentCalibration = calibration;
  display.setup();  
  display.printHello(getCalibrationString(currentCalibration));
  
}

void UserInterface::updateLastMeasurement(const Measurement& measurement) {
    lastMeasurement = measurement;
}

void UserInterface::loop() {
    if (processButtonOnNextLoop != NoButton) {
        switch (processButtonOnNextLoop) {
            case UserInterface::Primary:
                if (mode == ModeAuto) {
                    mode = ModeUser;
                    nextScreen();
                    renderScreen(screen);
                } else if (mode == ModeUser) {
                    nextScreen();
                    renderScreen(screen);
                } else if (mode == ModeMenu) {
                    menu.scrollToNext();
                }
                break;
            case UserInterface::Secondary:
                if (mode == ModeMenu) {
                    //select the item
                    if (menu.displayedSubmenu == Menu::NoMenu) {
                        if (menu.displayedItemInMenu == Menu::Exit) {
                            //we are exiting the menu
                            resetModeToAuto();
                            screen = Welcome;
                            renderScreen(screen);
                        } else if (menu.displayedSubmenu != Menu::Welcome && menu.displayedSubmenu != Menu::NoMenu) {
                            menu.enterSubmenu();
                        }
                    } else if (menu.displayedItemInMenu == Menu::Exit) {
                        menu.exitSubmenu();
                    } else if (menu.displayedSubmenu == Menu::Calibration) {
                        //todo: select calibration
                        menu.exitSubmenu();
                    } else if (menu.displayedSubmenu == Menu::MeasurementInterval) {
                        //todo: select calibration
                        menu.exitSubmenu();
                    } else if (menu.displayedSubmenu == Menu::ResetEnergyMeasurement) {
                        //todo: do reset
                        menu.exitSubmenu();
                    }
                } else {
                    mode = ModeMenu;
                    menu.enterMenu();
                }
                break;
            case NoButton:
                //nop, shouldn't happen
                break;
        }
        processButtonOnNextLoop = NoButton;
    } else {
        if (mode == ModeAuto) {
            if (lastAutoChange == 0 || lastAutoChange + autoModeDelay <= millis()) {
                lastAutoChange = millis();
                nextScreen();
                renderScreen(screen);
            }
        } else if (lastUserInteraction + autoUserModeReset <= millis()) {
            //auto quit user mode -> reset to auto mode.
            resetModeToAuto();
            screen = Welcome;
            renderScreen(screen);
        }
    }
}



void UserInterface::updateCalibration(const Measure::Calibration& calibration) {
    currentCalibration = calibration;
    resetModeToAuto();
    screen = None;
} 

void UserInterface::buttonTriggered(UserInterface::Button button) {
    lastUserInteraction = millis();
    processButtonOnNextLoop = button;
}

// PRIVATE Menu

//todo: make use of progmem
//top level menu - string in promem
// const char _top_menu_0[] PROGMEM = "";
// const char _top_menu_1[] PROGMEM = "Menu";
// const char _top_menu_2[] PROGMEM = "Calibration";
// const char _top_menu_3[] PROGMEM = "Interval";
// const char _top_menu_4[] PROGMEM = "Reset energy";
// const char _top_menu_5[] PROGMEM = "Exit";
// const char* const _top_menu_table[] PROGMEM = {_top_menu_0, _top_menu_1, _top_menu_2, _top_menu_3, _top_menu_4, _top_menu_5};


const char _menu_exit[] = "Exit";

const char _top_menu_0[] = "";
const char _top_menu_1[] = "Menu";
const char _top_menu_2[] = "Calibration";
const char _top_menu_3[] = "Interval";
const char _top_menu_4[] = "Reset energy";
const char* _top_menu_5 = _menu_exit;
const char* const _top_menu_table[] = {_top_menu_0, _top_menu_1, _top_menu_2, _top_menu_3, _top_menu_4, _top_menu_5};




void UserInterface::Menu::enterMenu() {
    displayedItemInMenu = Menu::Welcome;
    renderMenu();
}

void UserInterface::Menu::scrollToNext() {
    if (displayedItemInMenu == Menu::Exit) {
        displayedItemInMenu = Menu::Welcome;
    } else {
        displayedItemInMenu++;
    }
    renderMenu();
}

void UserInterface::Menu::enterSubmenu() {
    displayedSubmenu = displayedItemInMenu;
    submenuPosition = 0;
    renderSubmenu();
}

void UserInterface::Menu::renderMenu() {
    display.clear();
    if (displayedItemInMenu == Exit) {
        display.printMenuRow(0, false, _top_menu_table[4]);
        display.printMenuRow(1, true, _top_menu_table[5]);
    } else {
        display.printMenuRow(0, true, _top_menu_table[displayedItemInMenu]);
        display.printMenuRow(1, false, _top_menu_table[displayedItemInMenu + 1]);
    }
}

void UserInterface::Menu::renderSubmenu() {
    switch(displayedSubmenu) {
        case Menu::Calibration: 
            renderSubmenuCalibration();
            break;
        case Menu::MeasurementInterval:
            renderSubmenuInterval();
            break;
        case Menu::ResetEnergyMeasurement:
            renderSubmenuReset();
            break;
        case Menu::NoMenu:
        case Menu::Welcome:
        case Menu::Exit:
            //invalid
            assert(true);
            break;
    }
}

void UserInterface::Menu::renderSubmenuCalibration() {
    display.clear();
//     if (submenuPosition == 3) {
//         display.printMenuRow(0, false, getCalibrabtionString(getCalibration()));
//         display.printMenuRow(1, true, _menu_exit);
//     } else {
//         display.printMenuRow(0, true, _top_menu_table[displayedItemInMenu]);
//         display.printMenuRow(1, false, _top_menu_table[displayedItemInMenu + 1]);
//     }
}

void UserInterface::Menu::renderSubmenuInterval() {

}

void UserInterface::Menu::renderSubmenuReset() {

}

void UserInterface::Menu::exitSubmenu() {
    displayedItemInMenu = displayedSubmenu;
    displayedSubmenu = NoMenu;
    renderMenu();
}

// PRIVATE UserInterface


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

UserInterface::Menu::MenuItem& operator++(UserInterface::Menu::MenuItem& screen)
{
    assert(screen != UserInterface::Menu::Exit);
    return screen = static_cast<UserInterface::Menu::MenuItem>( screen + 1 );
}

UserInterface::Menu::MenuItem operator++(UserInterface::Menu::MenuItem& screen, int)
{
  assert(screen != UserInterface::Menu::Exit);
  UserInterface::Menu::MenuItem tmp(screen);
  ++screen;
  return tmp;
}

void UserInterface::loopAuto() {
}

void UserInterface::loopUser() {
}

void UserInterface::resetModeToAuto() {
    processButtonOnNextLoop = NoButton;
    lastUserInteraction = 0;
    lastAutoChange = 0;
    mode = ModeAuto;
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
            display.printHello(getCalibrationString(currentCalibration));
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

const String UserInterface::getCalibrationString(const Measure::Calibration& calibration) const {
    switch (calibration) {
        case Measure::C16V_400 : return F("16V 400mA");
        case Measure::C32V_1A : return F("32V 1A");
        case Measure::C32V_2A : return F("32V 2A");
        
    }
    return F("n/a");
    
}
