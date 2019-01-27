#include "Measure.h"

const unsigned long millisInHour = 3600000;


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
  configureIna(calibration);
}

void Measure::setCalibration(const Measure::Calibration& cal) {
    calibration = cal;
    configureIna(calibration); 
}

void Measure::setInterval(const unsigned long newInterval) {
     interval = newInterval * timeCorrection;
}

const Measurement Measure::doNewMeasurement() {
    float power_mW = ina219.getPower_mW();
    unsigned long now = millis();
    // Serial.print("now "); Serial.print(now); Serial.print(" last "); Serial.println(lastMeasurement);
    //compute estimated energy
    //energy = power * time = delta(elergy) + power * delta(time); unit = mW * h = mWh
    //TODO: beware float arithmetics
    unsigned long deltaTime = (now - max(lastMeasurement, energyEstimateResetMillis)) / timeCorrection;
    float deltaEnergy = (power_mW * deltaTime) / (float)millisInHour ;
    // Serial.println(power_mW * (now - lastMeasurement));
    // Serial.println((float)millisInHour);
    // Serial.println((power_mW * (now - lastMeasurement)) / (float)millisInHour);
    // Serial.println(deltaEnergy);
    energyEstimate += deltaEnergy;
    
    lastMeasurement = now;
    return Measurement(
        ina219.getShuntVoltage_mV(),
        ina219.getBusVoltage_V(),
        ina219.getCurrent_mA(),
        power_mW,
        energyEstimate,
        (now - energyEstimateResetMillis) / timeCorrection,
        deltaEnergy
    );
}

bool Measure::didIntervalElapsed() const {
    return lastMeasurement == 0 ||  millis() >= lastMeasurement + interval;
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

void Measure::resetEnergyEstimate() {
    energyEstimate = 0;
    energyEstimateResetMillis = millis();
}