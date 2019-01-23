#include "Display.h"
#include "Measure.h"
#include <assert.h>

#ifndef INC_USERINTERFACE
#define INC_USERINTERFACE

enum Menu { MenuMain, MenuCalibration, MenuInterval, MenuResetEnergy };

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
 *      - 100 msec
 *      - 250 msec
 *      - 500 msec
 *      - 1 sec
 *      - 2 sec
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
        void setup(const Measure::Calibration& calibration);
        void loop();
        void updateLastMeasurement(const Measurement& measurement);
        void buttonTriggered(Button button);
        void updateCalibration(const Measure::Calibration& calibration);
        const char* getCalibrationString(
            const Measure::Calibration& calibration) const;
        bool ledon = false;
        bool led2on = false;
       private:
        enum Mode { ModeAuto, ModeUser, ModeMenu };
        enum Screen { None, Welcome, Voltage, Current, Power, Energy, EnergyTime, LastScr };
        Menu currentMenu = MenuMain;
        int currentMenuPosition = -1;
        Display& display;
        void menuEnter();
        void menuRender();
        void menuNextCommand();
        void menuExecuteAction();
        const unsigned long autoModeDelay = 2000;
        const unsigned long autoUserModeReset = 5000;
        unsigned long lastAutoChange = 0;
        unsigned long lastUserInteraction = 0;
        Measure::Calibration currentCalibration = Measure::C16V_400;
        Button processButtonOnNextLoop = NoButton;
        Screen screen = None;
        Mode mode = ModeAuto; 
        Measurement lastMeasurement;
        void loopAuto();
        void loopUser();
        void nextScreen();
        void renderScreen(Screen scrToRender);
        void resetModeToAuto();
        friend UserInterface::Screen& operator++(UserInterface::Screen& screen);
        friend UserInterface::Screen operator++(UserInterface::Screen& screen, int);
};




#endif 