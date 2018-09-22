#include <Arduino.h>
#include <Adafruit_INA219.h>

#ifndef INC_MEASURE
#define INC_MEASURE


class Measurement {
    public:
        const float shuntvoltage;
        const float busvoltage;
        const float current_mA;
        const float loadvoltage;
        const float power_mW;
        Measurement(float shuntvoltagea, float busvoltage, float current_mA, float power_mW) :
            shuntvoltage(shuntvoltagea), busvoltage(busvoltage), current_mA(current_mA), 
            loadvoltage(busvoltage + (shuntvoltage / 1000)), 
            power_mW(power_mW) {};
    private:
       // Measurement(const Measurement&);   //prevent copy constructor
};


class Measure {
    public: 
        enum Calibration { C32V_2A, C32V_1A, C16V_400 };
        Measure(unsigned long interval, Calibration calibration): 
            interval(interval), calibration(calibration), ina219()
            {};
        void setup();
        /**
         * Returns true if is's at least <interval> millisec since the last
         * measurement.
         */
        bool didIntervalElapsed() const;
        /**
         * Does the measurement and resets the last measurement time.
         */
        const Measurement doNewMeasurement();
    private:
        const unsigned long interval;
        const Calibration calibration;
        unsigned long lastMeasurement = 0;
        Measure(const Measure&);   //prevent copy constructor
        Adafruit_INA219 ina219;
        const Measurement measure();
};


#endif //ifndef INC_MEASURE