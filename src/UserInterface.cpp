#include "UserInterface.h"

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
    [MenuInterval] = {CommandInterval1, CommandExit},
    [MenuResetEnergy] = {CommandDoResetEnergy, CommandExit}
};

const char _menu_exit[] = "Exit";
const char _top_menu_0[] = "";
const char _top_menu_1[] = "Menu";
const char _top_menu_2[] = "Calibration";
const char _top_menu_3[] = "Interval";
const char _top_menu_4[] = "Reset energy";
const char* _top_menu_5 = _menu_exit;
const char* const _top_menu_table[] = {_top_menu_0, _top_menu_1, _top_menu_2,
                                       _top_menu_3, _top_menu_4, _top_menu_5};

const char* const _command_labels[] = {
    [CommandExit] = "Exit",
    [CommandCalibration] = "Calibration",
    [CommandInterval] = "Interval",
    [CommandResetEnergy] = "Reset energy",
    [CommandCalibration1] = "16V 400mA",
    [CommandCalibration2] = "32V 1A",
    [CommandCalibration3] = "32V 2A",
    [CommandInterval1] = "1s",
    [CommandDoResetEnergy] = "Reset"
};

const char str_voltage[] = "Voltage";
const char str_current[] = "Current";
const char str_power[] = "Power";
const char str_energy[] = "Energy";
const char str_energy_time[] = "Energy time";

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
    int menuItemCount = 0;
    for (int i = 0; i < MAX_MENU_ITEMS; i++) {
        menuItemCount++;
        if (_menu_commands[currentMenu][i] == static_cast<uint8_t>(CommandExit)) break;
    }
    uint8_t currentCommand = _menu_commands[currentMenu][currentMenuPosition];
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
