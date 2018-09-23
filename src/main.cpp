#include <Arduino.h>
#include "Controlls.h"
#include "Measure.h"
#include "Display.h"
#include "UserInterface.h"



volatile Controlls controlls(PIN2, PIN3);
Measure measure(/*delay*/ 2000, Measure::C16V_400);
Display lcd(0x27);
UserInterface ui(lcd);
Measurement lastMeasurement(0,0,0,0);

void btn_interrupt_a() {
  controlls.buttonA.interrupt();
}

void btn_interrupt_b() {
  controlls.buttonB.interrupt();
}


void setup(void) 
{
  Serial.begin(115200);
  while (!Serial) {
      // will pause Zero, Leonardo, etc until serial console opens
      delay(1);
  }

  controlls.setup();
  attachInterrupt(digitalPinToInterrupt(controlls.buttonA.pin), btn_interrupt_a, CHANGE);
  attachInterrupt(digitalPinToInterrupt(controlls.buttonB.pin), btn_interrupt_b, CHANGE);
  measure.setup();
  ui.setup();
}

void loop(void) 
{

  if (measure.didIntervalElapsed()) {
    lastMeasurement = measure.doNewMeasurement();
    Serial.print("Bus Voltage:   "); Serial.print(lastMeasurement.busvoltage); Serial.println(" V");
    Serial.print("Shunt Voltage: "); Serial.print(lastMeasurement.shuntvoltage); Serial.println(" mV");
    Serial.print("Load Voltage:  "); Serial.print(lastMeasurement.loadvoltage); Serial.println(" V");
    Serial.print("Current:       "); Serial.print(lastMeasurement.current_mA); Serial.println(" mA");
    Serial.print("Power:         "); Serial.print(lastMeasurement.power_mW); Serial.println(" mW");
    Serial.println("");
    ui.updateLastMeasurement(lastMeasurement);
  }

  if (controlls.buttonA.checkIfButtonTriggeredAndReset()) {
    Serial.println("Button A triggered");
  }

  if (controlls.buttonB.checkIfButtonTriggeredAndReset()) {
    Serial.println("Button B triggered");
  }


  ui.loop();
}