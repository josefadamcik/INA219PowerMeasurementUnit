#include "UserInterface.h"
#include "MemoryFree.h"

#define MAX_MENU_ITEMS 6

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
// const char unit_mws[] PROGMEM = "mWs";
const char unit_uws[] PROGMEM = "uWs";

const char str_na[] PROGMEM = "n/a";

enum MenuCommand {
    CommandExit,
    CommandCalibration,
    CommandInterval,
    CommandResetEnergy,
    CommandCalibration1,
    CommandCalibration2,
    CommandCalibration3,
    CommandInterval1,  // 0.05s
    CommandInterval2,  // 0.5s
    CommandInterval3,  // 1s
    CommandInterval4,  // 5s
    CommandInterval5,  // 10s
    CommandDoResetEnergy
};

const uint8_t _menu_commands[][MAX_MENU_ITEMS] = {
    [MenuMain] = {CommandCalibration, CommandInterval, CommandResetEnergy, CommandExit},
    [MenuCalibration] = {CommandCalibration1, CommandCalibration2, CommandCalibration3, CommandExit},
    [MenuInterval] = {CommandInterval1, CommandInterval2, CommandInterval3, CommandInterval4, CommandInterval5, CommandExit},
    [MenuResetEnergy] = {CommandDoResetEnergy, CommandExit}
};

const char str_c_exit[] PROGMEM = "Exit";
const char str_c_calibration[] PROGMEM = "Calibration";
const char str_c_interval[] PROGMEM = "Interval";
const char str_c_reset_energy[] PROGMEM = "Reset energy";
const char str_c_calibration1[] PROGMEM = "16V 400mA";
const char str_c_calibration2[] PROGMEM = "32V 1A";
const char str_c_calibration3[] PROGMEM = "32V 2A";
const char str_c_interval1[] PROGMEM = " 50 ms";
const char str_c_interval2[] PROGMEM = "500 ms";
const char str_c_interval3[] PROGMEM = "  1 s";
const char str_c_interval4[] PROGMEM = "  5 s";
const char str_c_interval5[] PROGMEM = " 10 s";
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
    str_c_interval2,
    str_c_interval3,
    str_c_interval4,
    str_c_interval5,
    // [CommandDoResetEnergy] =
    str_c_do_reset};

void UserInterface::setup(Measure* m) {
    measure = m;
    display.setup();
    display.printHello(getCalibrationString(measure->getCalibration()), measure->getInterval());
}

void UserInterface::updateLastMeasurement(const Measurement& measurement) {
    lastMeasurement = measurement;
    measurementUpdated = true;
}

void UserInterface::loop() {
    if (processButtonOnNextLoop != NoButton) {
        if (!backlight) {
            display.backlightOn();
            backlight = true;
        } else {
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
        }
        processButtonOnNextLoop = NoButton;
    } else {
        if (mode == ModeAuto) {
            if (lastAutoChange == 0 ||
                lastAutoChange + autoModeDelay <= millis()) {
                lastAutoChange = millis();
                nextScreen();
                renderScreen(screen);
            } else if (measurementUpdated && lastDisplayUpdate + displayUpdateTimeout <= millis()) {
                //^ don't render every loop, only once a diplayUpdateTimout.
                renderScreen(screen);
            }
        } else if (lastUserInteraction + autoUserModeReset <= millis()) {
            // auto quit user mode -> reset to auto mode.
            resetModeToAuto();
            screen = Welcome;
            renderScreen(screen);
        } else if (mode != ModeMenu && measurementUpdated &&
                   lastDisplayUpdate + displayUpdateTimeout <= millis()) {
            //^ don't render every loop, only once a diplayUpdateTimout.
            renderScreen(screen);
        }
    }

    if (lastUserInteraction + backlightTimeout <= millis()) {
        display.backlightOff();
        backlight = false;
    }
}

void UserInterface::buttonTriggered(UserInterface::Button button) {
    lastUserInteraction = millis();
    processButtonOnNextLoop = button;
}

void UserInterface::menuExecuteAction() {
    uint8_t currentCommand = _menu_commands[currentMenu][currentMenuPosition];
    
    switch (currentCommand)
    {
        case CommandExit:
            if (currentMenu == MenuMain) {
                resetModeToAuto();
                nextScreen();
            } else { //submenu
                currentMenu = MenuMain;
                currentMenuPosition = lastTopMenuPosition; 
                menuRender();
            }
            break;
        case CommandCalibration:
            currentMenu = MenuCalibration;
            lastTopMenuPosition = currentMenuPosition;
            currentMenuPosition = 0;
            menuRender();
            break;
        case CommandInterval:
            currentMenu = MenuInterval;
            lastTopMenuPosition = currentMenuPosition;
            currentMenuPosition = 0;
            menuRender();
            break;
        case CommandInterval1:
            measure->setInterval(pgm_read_word_near(interval_values));
            storeSettingsToEEprom();
            resetModeToAuto();
            nextScreen();
            break;
        case CommandInterval2:
            measure->setInterval(pgm_read_word_near(interval_values + 1));
            storeSettingsToEEprom();
            resetModeToAuto();
            nextScreen();
            break;
        case CommandInterval3:
            measure->setInterval(pgm_read_word_near(interval_values + 2));
            storeSettingsToEEprom();
            resetModeToAuto();
            nextScreen();
            break;
        case CommandInterval4:
            measure->setInterval(pgm_read_word_near(interval_values + 3));
            storeSettingsToEEprom();
            resetModeToAuto();
            nextScreen();
            break;
        case CommandInterval5:
            measure->setInterval(pgm_read_word_near(interval_values + 4));
            storeSettingsToEEprom();
            resetModeToAuto();
            nextScreen();
            break;
        case CommandResetEnergy:
            currentMenu = MenuResetEnergy;
            lastTopMenuPosition = currentMenuPosition;
            currentMenuPosition = 0;
            menuRender();
            break;
        case CommandDoResetEnergy:
            measure->resetEnergyEstimate();
            resetModeToAuto();
            nextScreen();
            break;
        case CommandCalibration1:
            measure->setCalibration(Measure::C16V_400);
            storeSettingsToEEprom();
            resetModeToAuto();
            nextScreen();
            break;
        case CommandCalibration2:
            measure->setCalibration(Measure::C32V_1A);
            storeSettingsToEEprom();
            resetModeToAuto();
            nextScreen();
            break;
        case CommandCalibration3:
            measure->setCalibration(Measure::C32V_2A);
            storeSettingsToEEprom();
            resetModeToAuto();
            nextScreen();
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
    } else {
        uint8_t nextCommand = _menu_commands[currentMenu][currentMenuPosition + 1];
        row1 = (char*)pgm_read_word(&(_command_labels[currentCommand]));
        row2 = (char*)pgm_read_word(&(_command_labels[nextCommand]));
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


void UserInterface::resetModeToAuto() {
    processButtonOnNextLoop = NoButton;
    currentMenuPosition = -1;
    lastTopMenuPosition = -1;
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
    measurementUpdated = false;
    lastDisplayUpdate = millis();
    switch (scrToRender) {
        case Welcome:
            display.printHello(
                getCalibrationString(measure->getCalibration()),
                measure->getInterval()
                );
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
            display.printValue(str_energy, lastMeasurement.energy_uWs,
                                unit_uws);
            break;
        case EnergyTime:
            display.printValue(str_energy_time,
                                lastMeasurement.energyMillis / 1000, unit_s);
            break;
        default:
            display.clear();
    }
}

const char* UserInterface::getCalibrationString( const Measure::Calibration calibration) const {
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

void UserInterface::storeSettingsToEEprom() {
    uint8_t intervalValueIndex = 0; 
    uint16_t intervalValue = measure->getInterval();
    for (uint8_t i = 0; i < 5; i++) {
        if (pgm_read_word_near(interval_values + i) == intervalValue) {
            intervalValueIndex = i;
            break;
        }
    }
    uint8_t calibrationIndex = measure->getCalibration();
    if (EEPROM.read(eepromSaved) != eepromSavedMagic) {
        EEPROM.write(eepromSaved, eepromSavedMagic);
    }    
    if (EEPROM.read(eepromInterval) != intervalValueIndex) {
        EEPROM.write(eepromInterval, intervalValueIndex);
    }
    if (EEPROM.read(eepromCalibration) != calibrationIndex) {
        EEPROM.write(eepromCalibration, calibrationIndex);
    }
}