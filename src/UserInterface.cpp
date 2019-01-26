#include "UserInterface.h"
#include "MemoryFree.h"

const char str_voltage[] = "Voltage";
const char str_current[] = "Current";
const char str_power[] = "Power";
const char str_energy[] = "Energy";
const char str_energy_time[] = "Energy time";
const char str_memory[] = "Memory";

enum MenuCommand {
    CommandExit,
    CommandCalibration,
    CommandInterval,
    CommandResetEnergy,
    CommandCalibration1,
    CommandCalibration2,
    CommandCalibration3,
    CommandInterval1,
    CommandDoResetEnergy
};
#define MAX_MENU_ITEMS 4

const uint8_t _menu_commands[][MAX_MENU_ITEMS] = {
    [MenuMain] = {CommandCalibration, CommandInterval, CommandResetEnergy, CommandExit},
    [MenuCalibration] = {CommandCalibration1, CommandCalibration2, CommandCalibration3, CommandExit},
    [MenuInterval] = {CommandInterval1, CommandExit, 0, 0},
    [MenuResetEnergy] = {CommandDoResetEnergy, CommandExit, 0, 0}
};

const char str_c_exit[] = "Exit";
const char str_c_calibration[] = "Calibration";
const char str_c_interval[] = "Interval";
const char str_c_reset_energy[] = "Reset energy";
const char str_c_calibration1[] = "16V 400mA";
const char str_c_calibration2[] = "32V 1A";
const char str_c_calibration3[] = "32V 2A"; 
const char str_c_interval1[] = "1s";
const char str_c_do_reset[] = "Reset";

const char* _command_labels[] = {
    // [CommandExit] =
    str_c_exit,
    // [CommandCalibration] = 
    // str_c_calibration,
    // [CommandInterval] = 
    // str_c_interval,
    // [CommandResetEnergy] = 
    // str_c_reset_energy,
    // [CommandCalibration1] = 
    // str_c_calibration1,
    // [CommandCalibration2] = 
    // str_c_calibration2,
    // [CommandCalibration3] = 
    // str_c_calibration3,
    // [CommandInterval1] = 
    // str_c_interval1,
    // [CommandDoResetEnergy] = 
    str_c_do_reset
};

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
                    menuNextCommand();
                }
                break;
            case UserInterface::Secondary:
                if (mode == ModeMenu) {
                    menuExecuteAction();
                } else {
                    mode = ModeMenu;
                    menuEnter();
                }
                break;
            case NoButton:
                // nop, shouldn't happen
                break;
        }
        processButtonOnNextLoop = NoButton;
    } else {
        if (mode == ModeAuto) {
            if (lastAutoChange == 0 ||
                lastAutoChange + autoModeDelay <= millis()) {
                lastAutoChange = millis();
                nextScreen();
                renderScreen(screen);
            }
        } else if (lastUserInteraction + autoUserModeReset <= millis()) {
            // auto quit user mode -> reset to auto mode.
            resetModeToAuto();
            screen = Welcome;
            renderScreen(screen);
        }
    }
}


void UserInterface::updateCalibration(
    const Measure::Calibration& calibration) {
    currentCalibration = calibration;
    resetModeToAuto();
    screen = None;
}

void UserInterface::buttonTriggered(UserInterface::Button button) {
    lastUserInteraction = millis();
    processButtonOnNextLoop = button;
}

// PRIVATE Menu

void UserInterface::menuExecuteAction() {
    uint8_t currentCommand = _menu_commands[currentMenu][currentMenuPosition];
    
    switch (currentCommand)
    {
        case CommandExit:
            if (currentMenu == MenuMain) {
                currentMenuPosition = -1;
                resetModeToAuto();
                nextScreen();
            } else { //submenu
                currentMenu = MenuMain;
                currentMenuPosition = 0; 
                menuRender();
            }
            break;
        case CommandCalibration:
            currentMenu = MenuCalibration;
            currentMenuPosition = 0;
            menuRender();
            break;
        case CommandInterval:
            currentMenu = MenuInterval;
            currentMenuPosition = 0;
            menuRender();
            break;
        case CommandResetEnergy:
            currentMenu = MenuResetEnergy;
            currentMenuPosition = 0;
            menuRender();
            break;
        default:
            break;
    }
}

void UserInterface::menuNextCommand() {
    uint8_t currentCommand = _menu_commands[currentMenu][currentMenuPosition];
    if (currentCommand == static_cast<uint8_t>(CommandExit)) {
        currentMenuPosition = 0;
    } else {
        currentMenuPosition++;
    } 
    menuRender();
}

void UserInterface::menuEnter() {
    currentMenu = MenuMain;
    currentMenuPosition = 0;
    menuRender();
}

void UserInterface::menuRender() {
    display.clear();

    // int menuItemCount = 0;
    // for (int i = 0; i < MAX_MENU_ITEMS; i++) {
    //     menuItemCount++;
    //     if (_menu_commands[currentMenu][i] ==
    //     static_cast<uint8_t>(CommandExit)) break;
    // }
    uint8_t currentCommand = _menu_commands[currentMenu][currentMenuPosition];
    // const char* label1 = _command_labels[currentCommand];
    // // const char* label2 = _command_labels[currentCommand + 1];
    // display.printMenuRow( 0, false, label1);
    // display.printMenuRow(1, true,  "b");

    if ( currentCommand == static_cast<uint8_t>(CommandExit)) {
        display.printMenuRow(
            0, false,
            _command_labels[_menu_commands[currentMenu][currentMenuPosition - 1]]);
        display.printMenuRow(1, true, _command_labels[currentCommand]);
    } else {
        display.printMenuRow(0, true, _command_labels[currentCommand]);
        display.printMenuRow(
            1, false,
            _command_labels[_menu_commands[currentMenu]
                                          [currentMenuPosition + 1]]);
    }
}


// PRIVATE UserInterface

UserInterface::Screen& operator++(UserInterface::Screen& screen) {
    assert(screen != UserInterface::LastScr);
    return screen = static_cast<UserInterface::Screen>(screen + 1);
}

UserInterface::Screen operator++(UserInterface::Screen& screen, int) {
    assert(screen != UserInterface::LastScr);
    UserInterface::Screen tmp(screen);
    ++screen;
    return tmp;
}


void UserInterface::loopAuto() {}

void UserInterface::loopUser() {}

void UserInterface::resetModeToAuto() {
    processButtonOnNextLoop = NoButton;
    lastUserInteraction = 0;
    lastAutoChange = 0;
    mode = ModeAuto;
}

void UserInterface::nextScreen() {
    if (screen == LastScr - 1) {  // the last one
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
        case Memory:
            display.printValue(str_memory, freeMemory(), "b");
            break;
        case Voltage:
            display.printValue(str_voltage,
                                lastMeasurement.loadvoltage * 1000, "mV");
            break;
        case Current:
            display.printValue(str_current, lastMeasurement.current_mA,
                                "mA");
            break;
        case Power:
            display.printValue(str_power, lastMeasurement.power_mW, "mW");
            break;
        case Energy:
            display.printValue(str_energy, lastMeasurement.energy_mWh,
                                "mWh");
            break;
        case EnergyTime:
            display.printValue(str_energy_time,
                                lastMeasurement.energyMillis / 1000, "s");
            break;
        default:
            display.clear();
    }
}

const char* UserInterface::getCalibrationString(
    const Measure::Calibration& calibration) const {
    switch (calibration) {
        case Measure::C16V_400:
            return "16V 400mA";
        case Measure::C32V_1A:
            return "32V 1A";
        case Measure::C32V_2A:
            return "32V 2A";
    }
    return "n/a";
}
