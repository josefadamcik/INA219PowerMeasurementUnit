#include "Display.h"
#include "Measure.h"
#include <assert.h>

#ifndef INC_USERINTERFACE
#define INC_USERINTERFACE

class UserInterface {
    public:
        enum Mode { Auto, User };
        enum Screen { None, Welcome, Voltage, Current, Power, Energy, EnergyTime, LastScr };
        UserInterface(Display& display): display(display), lastMeasurement(0,0,0,0,0,0,0) {};
        void setup(const Measure::Calibration& calibration);
        void loop();
        void updateLastMeasurement(const Measurement& measurement);
        void mainButtonTriggered();
        void updateCalibration(const Measure::Calibration& calibration);
        const String getCalibrationString() const;
    private:
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

};




#endif 