#include "UserInterface.h"
#include "MemoryFree.h"

const char str_voltage[] PROGMEM = "Voltage";
const char str_current[] PROGMEM = "Current";
const char str_power[] PROGMEM = "Power";
const char str_energy[] PROGMEM = "Energy";
const char str_energy_time[] PROGMEM = "Energy time";
const char str_memory[] PROGMEM = "Memory";

const char unit_mv[] PROGMEM = "mV";
const char unit_mw[] PROGMEM = "mW";
const char unit_b[] PROGMEM = "b";
const char unit_s[] PROGMEM = "s";
const char unit_ma[] PROGMEM = "mA";
const char unit_mwh[] PROGMEM = "mWh";

const char str_na[] PROGMEM = "n/a";

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

const char str_c_exit[] PROGMEM = "Exit";
const char str_c_calibration[] PROGMEM = "Calibration";
const char str_c_interval[] PROGMEM = "Interval";
const char str_c_reset_energy[] PROGMEM = "Reset energy";
const char str_c_calibration1[] PROGMEM = "16V 400mA";
const char str_c_calibration2[] PROGMEM = "32V 1A";
const char str_c_calibration3[] PROGMEM = "32V 2A";
const char str_c_interval1[] PROGMEM = "1s";
const char str_c_do_reset[] PROGMEM = "Reset";

const char* const _command_labels[] PROGMEM = {
    // [CommandExit] =
    str_c_exit,
    // [CommandCalibration] =
    str_c_calibration,
    // [CommandInterval] =
    str_c_interval,
    // [CommandResetEnergy] =
    str_c_reset_energy,
    // [CommandCalibration1] =
    str_c_calibration1,
    // [CommandCalibration2] =
    str_c_calibration2,
    // [CommandCalibration3] =
    str_c_calibration3,
    // [CommandInterval1] =
    str_c_interval1,
    // [CommandDoResetEnergy] =
    str_c_do_reset};

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
    uint8_t currentCommand = _menu_commands[currentMenu][currentMenuPosition];
    const char* row1;
    const char* row2;

    if ( currentCommand == static_cast<uint8_t>(CommandExit)) {
        uint8_t previousCommand = _menu_commands[currentMenu][currentMenuPosition - 1];
        row1 = (char*)pgm_read_word(&(_command_labels[previousCommand]));
        row2 = (char*)pgm_read_word(&(_command_labels[currentCommand]));
        // display.printMenuRow(0, false, _command_labels[]);
        // display.printMenuRow(1, true, _command_labels[currentCommand]);
    } else {
        uint8_t nextCommand = _menu_commands[currentMenu][currentMenuPosition + 1];
        row1 = (char*)pgm_read_word(&(_command_labels[currentCommand]));
        row2 = (char*)pgm_read_word(&(_command_labels[nextCommand]));
        // display.printMenuRow(0, true, _command_labels[currentCommand]);
        // display.printMenuRow(
        //     1, false,
        //     _command_labels[_menu_commands[currentMenu]
        //                                   [currentMenuPosition + 1]]);
    }
    display.printMenuRow(0, currentCommand != static_cast<uint8_t>(CommandExit),
                         row1);
    display.printMenuRow(1, currentCommand == static_cast<uint8_t>(CommandExit),
                         row2);
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
            display.printValue(str_memory, freeMemory(), unit_b);
            break;
        case Voltage:
            display.printValue(str_voltage,
                                lastMeasurement.loadvoltage * 1000, unit_mv);
            break;
        case Current:
            display.printValue(str_current, lastMeasurement.current_mA,
                                unit_ma);
            break;
        case Power:
            display.printValue(str_power, lastMeasurement.power_mW, unit_mw);
            break;
        case Energy:
            display.printValue(str_energy, lastMeasurement.energy_mWh,
                                unit_mwh);
            break;
        case EnergyTime:
            display.printValue(str_energy_time,
                                lastMeasurement.energyMillis / 1000, unit_s);
            break;
        default:
            display.clear();
    }
}

const char* UserInterface::getCalibrationString(
    const Measure::Calibration& calibration) const {
    switch (calibration) {
        case Measure::C16V_400:
            return str_c_calibration1;
        case Measure::C32V_1A:
            return str_c_calibration2;
        case Measure::C32V_2A:
            return str_c_calibration3;
    }
    return str_na;
}
