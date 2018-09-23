#include <Arduino.h>
#include "Controlls.h"
#include "Measure.h"
#include "Display.h"
#include "UserInterface.h"



volatile Button buttonA(PIN2);
volatile Button buttonB(PIN3);
Measure measure(/*delay*/ 5000, Measure::C16V_400);
Display lcd(0x27);
UserInterface ui(lcd);
Measurement lastMeasurement(0,0,0,0);

void btn_interrupt_a() {
  buttonA.interrupt();
}

void btn_interrupt_b() {
  buttonB.interrupt();
}

void setup(void) 
{
  Serial.begin(115200);
  while (!Serial) {
      // will pause Zero, Leonardo, etc until serial console opens
      delay(1);
  }

  buttonA.setup(btn_interrupt_a);
  buttonB.setup(btn_interrupt_b);
  measure.setup();
  ui.setup(measure.getCalibration());
}

void loop(void) 
{

  if (measure.didIntervalElapsed()) {
    lastMeasurement = measure.doNewMeasurement();
    Serial.print(F("Bus Voltage:   ")); Serial.print(lastMeasurement.busvoltage); Serial.println(F(" V"));
    Serial.print(F("Shunt Voltage: ")); Serial.print(lastMeasurement.shuntvoltage); Serial.println(F(" mV"));
    Serial.print(F("Load Voltage:  ")); Serial.print(lastMeasurement.loadvoltage); Serial.println(F(" V"));
    Serial.print(F("Current:       ")); Serial.print(lastMeasurement.current_mA); Serial.println(F(" mA"));
    Serial.print(F("Power:         ")); Serial.print(lastMeasurement.power_mW); Serial.println(F(" mW"));
    Serial.println();
    ui.updateLastMeasurement(lastMeasurement);
  }

  if (buttonA.checkIfButtonTriggeredAndReset()) {
    Serial.println(F("Button A triggered"));
    ui.mainButtonTriggered();
  }

  if (buttonB.checkIfButtonTriggeredAndReset()) {
    Serial.println(F("Button B triggered"));
    ui.updateCalibration(measure.nextCalibration());
  }


  ui.loop();
}