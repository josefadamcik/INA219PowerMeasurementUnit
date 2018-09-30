#include "Display.h"
#include "Measure.h"
#include <assert.h>

#ifndef INC_USERINTERFACE
#define INC_USERINTERFACE


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
        enum Button { Primary, Secondary };
        UserInterface(Display& display): display(display), lastMeasurement(0,0,0,0,0,0,0) {};
        void setup(const Measure::Calibration& calibration);
        void loop();
        void updateLastMeasurement(const Measurement& measurement);
        void buttonTriggered(Button button);
        void updateCalibration(const Measure::Calibration& calibration);
        const String getCalibrationString() const;
    private:
        enum Mode { Auto, User, Menu };
        enum MenuLevel1Screen { Calibration, MeasurementInterval, ResetEnergyMeasurement, Exit};
        enum Screen { None, Welcome, Voltage, Current, Power, Energy, EnergyTime, LastScr };
        const unsigned long autoModeDelay = 2000;
        const unsigned long autoUserModeReset = 5000;
        unsigned long lastAutoChange = 0;
        unsigned long lastUserInteraction = 0;
        Measure::Calibration currentCalibration = Measure::C16V_400;
        bool mainButtonWasTriggered = false;
        Screen screen = None;
        Display& display;
        Mode mode = Auto; 
        Measurement lastMeasurement;
        void loopAuto();
        void loopUser();
        void nextScreen();
        void renderScreen(Screen scrToRender);
        void resetModeToAuto();
        friend UserInterface::MenuLevel1Screen& operator++(UserInterface::MenuLevel1Screen& screen);
        friend UserInterface::MenuLevel1Screen operator++(UserInterface::MenuLevel1Screen& screen, int);
        friend UserInterface::Screen& operator++(UserInterface::Screen& screen);
        friend UserInterface::Screen operator++(UserInterface::Screen& screen, int);


};




#endif 