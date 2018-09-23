#include "Display.h"
#include "Measure.h"
#include <assert.h>

#ifndef INC_USERINTERFACE
#define INC_USERINTERFACE

class UserInterface {
    public:
        enum Mode { Auto, User };
        enum Screen { None, Welcome, Voltage, Current, Power, LastScr };
        UserInterface(Display& display): display(display), lastMeasurement(0,0,0,0) {};
        void setup();
        void loop();
        void updateLastMeasurement(const Measurement& measurement);
        void mainButtonTriggered();
    private:
        const unsigned long autoModeDelay = 2000;
        const unsigned long autoUserModeReset = 5000;
        unsigned long lastAutoChange = 0;
        unsigned long lastUserInteraction = 0;
        bool mainButtonWasTriggered = false;
        Screen screen = None;
        Display& display;
        Mode mode = Auto; 
        Measurement lastMeasurement;
        void loopAuto();
        void loopUser();
        void nextScreen();
        void renderScreen(Screen scrToRender);

};




#endif 