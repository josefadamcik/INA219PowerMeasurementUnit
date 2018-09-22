#include "Measure.h"


void Measure::setup() {
  // Initialize the INA219.
  // By default the initialization will use the largest range (32V, 2A).  However
  // you can call a setCalibration function to change this range (see comments).
  ina219.begin();
  // To use a slightly lower 32V, 1A range (higher precision on amps):
  //ina219.setCalibration_32V_1A();
  // Or to use a lower 16V, 400mA range (higher precision on volts and amps):
  ina219.setCalibration_16V_400mA();
}

const Measurement Measure::measure() {
    const Measurement measurement =  Measurement(
        ina219.getShuntVoltage_mV(),
        ina219.getBusVoltage_V(),
        ina219.getCurrent_mA(),
        ina219.getPower_mW()
    );
   return measurement;
}

const Measurement Measure::doNewMeasurement() {
    lastMeasurement = millis();
    return measure();
}

bool Measure::didIntervalElapsed() const {
    return lastMeasurement == 0 ||  millis() >= lastMeasurement + interval;
}