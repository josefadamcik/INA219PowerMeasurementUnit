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
        enum Button { Primary, Secondary, NoButton };
        UserInterface(Display& display): display(display),  menu(display), lastMeasurement(0,0,0,0,0,0,0)  {};
        void setup(const Measure::Calibration& calibration);
        void loop();
        void updateLastMeasurement(const Measurement& measurement);
        void buttonTriggered(Button button);
        void updateCalibration(const Measure::Calibration& calibration);
        const char* getCalibrationString(
            const Measure::Calibration& calibration) const;

       private:
        enum Mode { ModeAuto, ModeUser, ModeMenu };
        enum Screen { None, Welcome, Voltage, Current, Power, Energy, EnergyTime, LastScr };
        class Menu {
            enum MenuItem { NoMenu, Welcome, Calibration, MeasurementInterval, ResetEnergyMeasurement, Exit};
            Menu(Display& display): display(display) {}; 
            MenuItem displayedItemInMenu = Welcome;
            MenuItem displayedSubmenu = NoMenu;
            byte submenuPosition = 0;
            Display& display;
            friend UserInterface::Menu::MenuItem& operator++(UserInterface::Menu::MenuItem& screen);
            friend UserInterface::Menu::MenuItem operator++(UserInterface::Menu::MenuItem& screen, int);
            void enterMenu();
            void renderMenu();
            void scrollToNext();
            void exitSubmenu();
            void renderSubmenu();
            void enterSubmenu();
            void renderSubmenuCalibration();
            void renderSubmenuInterval();
            void renderSubmenuReset();
            friend UserInterface;
        };
        const unsigned long autoModeDelay = 2000;
        const unsigned long autoUserModeReset = 5000;
        unsigned long lastAutoChange = 0;
        unsigned long lastUserInteraction = 0;
        Measure::Calibration currentCalibration = Measure::C16V_400;
        Button processButtonOnNextLoop = NoButton;
        Screen screen = None;
        Display& display;
        Mode mode = ModeAuto; 
        Menu menu;
        Measurement lastMeasurement;
        void loopAuto();
        void loopUser();
        void nextScreen();
        void renderScreen(Screen scrToRender);
        void resetModeToAuto();
        friend Menu;
        friend UserInterface::Screen& operator++(UserInterface::Screen& screen);
        friend UserInterface::Screen operator++(UserInterface::Screen& screen, int);
        friend UserInterface::Menu::MenuItem& operator++(UserInterface::Menu::MenuItem& screen);
        friend UserInterface::Menu::MenuItem operator++(UserInterface::Menu::MenuItem& screen, int);
        


};




#endif 