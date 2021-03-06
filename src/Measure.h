#include <Arduino.h>
#include <Adafruit_INA219.h>
#include <assert.h>

#ifndef INC_MEASURE
#define INC_MEASURE

// we need to correct millis, reported are rougly 2times more than it should be.
// Maybe because we are runnning 16mhz.
const unsigned long timeCorrection = 2;

class Measurement {
    public:
        float shuntvoltage;
        float busvoltage;
        float current_mA;
        float loadvoltage;
        float power_mW;
        float energy_uWs;     // estimated energy (sum of energyDelta since the
                              // last reset)
        float energyDelta;    // power * time (sinec the least measurement)
        unsigned long energyMillis; //elaspsed time since the energy estimeate was reset
        Measurement(float shuntvoltagea, float busvoltage, float current_mA,
                    float power_mW, float energy_uWs,
                    unsigned long energyMillis, float energyDelta)
            : shuntvoltage(shuntvoltagea),
              busvoltage(busvoltage),
              current_mA(current_mA),
              loadvoltage(busvoltage + (shuntvoltage / 1000)),
              power_mW(power_mW),
              energy_uWs(energy_uWs),
              energyDelta(energyDelta),
              energyMillis(energyMillis){};

       private:
};


class Measure {
    public: 
        enum Calibration { C32V_2A, C32V_1A, C16V_400 };
        Measure(unsigned long interval, Calibration calibration): 
            interval(interval), calibration(calibration)//, ina219()
            {};
        void setup();
        void setup(uint16_t interval, uint8_t calibrationIndex);
        /**
         * Returns true if is's at least <interval> millisec since the last
         * measurement.
         */
        bool didIntervalElapsed() const;
        /**
         * Does the measurement and resets the last measurement time.
         */
        const Measurement doNewMeasurement();
        Calibration getCalibration() const;
        void resetEnergyEstimate();
        void setCalibration(const Measure::Calibration& cal);
        void setInterval(const unsigned long interval);
        inline unsigned long getInterval() { return interval / timeCorrection;};
    private: 
        unsigned long interval;
        Calibration calibration;
        unsigned long lastMeasurement = 0;
        unsigned long energyEstimateResetMillis = 0;
        float energyEstimate = 0;
        Measure(const Measure&);   //prevent copy constructor
        Adafruit_INA219 ina219;
        void configureIna(const Measure::Calibration& cal);
};


#endif //ifndef INC_MEASURE