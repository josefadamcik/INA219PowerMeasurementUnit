#include "Measure.h"

Measure::Calibration& operator++(Measure::Calibration& screen)
{
    assert(screen != Measure::C16V_400);
    return screen = static_cast<Measure::Calibration>( screen + 1 );
}

Measure::Calibration operator++(Measure::Calibration& screen, int)
{
  assert(screen != Measure::C16V_400);
  Measure::Calibration tmp(screen);
  ++screen;
  return tmp;
}


void Measure::setup() {
  ina219.begin();
  configureIna(calibration);
}

const Measurement Measure::measure() {
    return Measurement(
        ina219.getShuntVoltage_mV(),
        ina219.getBusVoltage_V(),
        ina219.getCurrent_mA(),
        ina219.getPower_mW()
    );
}

const Measurement Measure::doNewMeasurement() {
    lastMeasurement = millis();
    return measure();
}

bool Measure::didIntervalElapsed() const {
    return lastMeasurement == 0 ||  millis() >= lastMeasurement + interval;
}


Measure::Calibration Measure::nextCalibration() {
    if (calibration == C16V_400) {
        calibration = C32V_2A;
    } else {
        calibration++;
    }
    configureIna(calibration);
    return calibration;
}

Measure::Calibration Measure::getCalibration() const {
    return calibration;
}

void Measure::configureIna(const Measure::Calibration& cal) {
    switch (cal) {
        case Measure::C16V_400:
            ina219.setCalibration_16V_400mA();
            break;
        case Measure::C32V_1A:
            ina219.setCalibration_32V_1A();
            break;
        case Measure::C32V_2A:
            ina219.setCalibration_32V_2A();
            break;
    }
}