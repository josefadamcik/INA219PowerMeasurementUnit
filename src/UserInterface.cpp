#include "UserInterface.h"

enum Menu {
    MenuMain,
    MenuCalibration,
    MenuInterval,
    MenuResetEnergy
};

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

const uint8_t menus[][MAX_MENU_ITEMS] = {
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

const char* const command_labels[] = {
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
                        scrollToNext();
                    }
                    break;
                case UserInterface::Secondary:
                    if (mode == ModeMenu) {
                        // select the item
                        if (displayedSubmenu == NoMenu) {
                            if (displayedItemInMenu == Exit) {
                                // we are exiting the menu
                                resetModeToAuto();
                                screen = Welcome;
                                renderScreen(screen);
                            } else if (displayedItemInMenu != WelcomeMenu &&
                                       displayedItemInMenu != NoMenu) {
                                enterSubmenu();
                            }
                        } else if (displayedSubmenu == Calibration) {
                            if (submenuPosition == 3) {
                                exitSubmenu();
                            } else {
                                // TODO: change calibration;
                            }
                        } else if (displayedSubmenu == MeasurementInterval) {
                            exitSubmenu();
                        } else if (displayedSubmenu == ResetEnergyMeasurement) {
                            exitSubmenu();
                        }
                    } else {
                        mode = ModeMenu;
                        enterMenu();
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

    void UserInterface::enterMenu() {
        displayedItemInMenu = WelcomeMenu;
        renderMenu();
    }

    void UserInterface::scrollToNext() {
        if (displayedSubmenu == NoMenu) {
            if (displayedItemInMenu == Exit) {
                displayedItemInMenu = WelcomeMenu;
            } else {
                displayedItemInMenu++;
            }
            renderMenu();
        } else {
            submenuPosition++;
            if (displayedSubmenu == Calibration && submenuPosition == 4) {
                submenuPosition = 0;
            }
            // TODO: other sumbmenus
            renderSubmenu();
        }
    }

    void UserInterface::enterSubmenu() {
        displayedSubmenu = displayedItemInMenu;
        submenuPosition = 0;
        renderSubmenu();
    }

    void UserInterface::renderMenu() {
        display.clear();
        if (displayedItemInMenu == Exit) {
            display.printMenuRow(0, false, _top_menu_table[4]);
            display.printMenuRow(1, true, _top_menu_table[5]);
        } else {
            display.printMenuRow(0, true, _top_menu_table[displayedItemInMenu]);
            display.printMenuRow(1, false,
                                 _top_menu_table[displayedItemInMenu + 1]);
        }
    }

    void UserInterface::renderSubmenu() {
        switch (displayedSubmenu) {
            case Calibration:
                renderSubmenuCalibration();
                break;
            case MeasurementInterval:
                renderSubmenuInterval();
                break;
            case ResetEnergyMeasurement:
                renderSubmenuReset();
                break;
            case NoMenu:
            case Welcome:
            case Exit:
                // invalid
                break;
        }
    }

    void UserInterface::renderSubmenuCalibration() {
        display.clear();
        if (submenuPosition == 0) {
            display.printMenuRow(0, true,
                                 getCalibrationString(Measure::C16V_400));
            display.printMenuRow(1, false,
                                 getCalibrationString(Measure::C32V_1A));
        } else if (submenuPosition == 1) {
            display.printMenuRow(0, true,
                                 getCalibrationString(Measure::C32V_1A));
            display.printMenuRow(1, false,
                                 getCalibrationString(Measure::C32V_2A));
        } else if (submenuPosition == 2) {
            display.printMenuRow(0, true,
                                 getCalibrationString(Measure::C32V_2A));
            display.printMenuRow(1, false, _menu_exit);
        } else if (submenuPosition == 3) {
            display.printMenuRow(0, false,
                                 getCalibrationString(Measure::C32V_2A));
            display.printMenuRow(1, true, _menu_exit);
        }
    }

    void UserInterface::renderSubmenuInterval() {}

    void UserInterface::renderSubmenuReset() {}

    void UserInterface::exitSubmenu() {
        displayedItemInMenu = displayedSubmenu;
        displayedSubmenu = NoMenu;
        renderMenu();
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

    UserInterface::MenuItem& operator++(UserInterface::MenuItem& screen) {
        assert(screen != UserInterface::Exit);
        return screen = static_cast<UserInterface::MenuItem>(screen + 1);
    }

    UserInterface::MenuItem operator++(UserInterface::MenuItem& screen, int) {
        assert(screen != UserInterface::Exit);
        UserInterface::MenuItem tmp(screen);
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
