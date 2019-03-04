#include "Display.h"
#include "Measure.h"
#include <assert.h>
#include <EEPROM.h>

#ifndef INC_USERINTERFACE
#define INC_USERINTERFACE

enum Menu { MenuMain, MenuCalibration, MenuInterval, MenuResetEnergy };

const uint8_t eepromSavedMagic = 0xF0;
const uint8_t eepromSaved = 0;
const uint8_t eepromCalibration = 1;
const uint8_t eepromInterval = 2;

const PROGMEM uint16_t interval_values[] = {50, 500, 1000, 5000, 10000};

/**
 * We have 16x2 LCD display and 2 buttons (primay and secondary).
 * 
 * There are several modes:
 * - Auto (default) -> display cycles information, primary button switches to the mode User
 * - User -> user can cycle through displayed information by pressing the primary button. This 
 * mode resets after some time without any user interaction and returns to the Auto mode.
 * - Menu - can be entered by pressing the secondary button in both modes (Auto, User). 
 * Primary button is used to scroll through options and secondary to select. This mode resets after some
 * time without any user interaction and returns to the Auto mode.
 * 
 * Menu tree:
 * - select calibration (selected calibration is marked by arrow)
 *      - calibration a
 *      - calibratin b
 *      - calibration c
 *      - exit
 * - select measurement interval (msec)
 *      - 50 msec
 *      - 500 msec
 *      - 1 sec
 *      - 5 sec
 *      - 10 sec
 *      - exit
 * - reset energy measurement
 *      - confirm 
 *      - exit
 * - exit
 */ 
class UserInterface {
    public:
        enum Button { Primary, Secondary, NoButton };
        UserInterface(Display& display): display(display),  lastMeasurement(0,0,0,0,0,0,0)  {};
        void setup(Measure* measure);
        void loop();
        void updateLastMeasurement(const Measurement& measurement);
        void buttonTriggered(Button button);
        const char* getCalibrationString(
            const Measure::Calibration calibration) const;
       private:
        enum Mode { ModeAuto, ModeUser, ModeMenu };
        enum Screen { None, Welcome, Memory,  Voltage, Current, Power, Energy, EnergyTime, LastScr };
        Menu currentMenu = MenuMain;
        int currentMenuPosition = -1;
        int lastTopMenuPosition = -2;
        Display& display;
        Measure* measure;
        void menuEnter();
        void menuRender();
        void menuNextCommand();
        void menuExecuteAction();
        const unsigned long autoModeDelay = 5000 * timeCorrection;
        const unsigned long autoUserModeReset = 10000 * timeCorrection;
        const unsigned long backlightTimeout = 10000 * timeCorrection; 
        const unsigned long displayUpdateTimeout = 1000 * timeCorrection;
        unsigned long lastAutoChange = 0;
        unsigned long lastUserInteraction = 0;
        unsigned long lastDisplayUpdate = 0;
        bool backlight = true;
        Button processButtonOnNextLoop = NoButton;
        Screen screen = None;
        Mode mode = ModeAuto; 
        Measurement lastMeasurement;
        bool measurementUpdated = false;
        void nextScreen();
        void renderScreen(Screen scrToRender);
        void resetModeToAuto();
        void storeSettingsToEEprom();
        friend UserInterface::Screen& operator++(UserInterface::Screen& screen);
        friend UserInterface::Screen operator++(UserInterface::Screen& screen, int);
};




#endif 