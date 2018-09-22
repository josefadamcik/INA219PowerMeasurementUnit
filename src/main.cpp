#include <Arduino.h>
#include "Controlls.h"
#include "Measure.h"
#include "Display.h"



volatile Controlls controlls(PIN2, PIN3);
Measure measure(/*delay*/ 2000, Measure::C16V_400);
Display lcd(0x27);

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
  lcd.setup();  
  lcd.printHello();
}

void loop(void) 
{
  if (controlls.buttonA.checkIfButtonTriggeredAndReset()) {
    Serial.println("Button A triggered");
    lcd.printHello();
  }

  if (controlls.buttonB.checkIfButtonTriggeredAndReset()) {
    Serial.println("Button B triggered");
  }

  if (measure.didIntervalElapsed()) {
    const Measurement& measurement = measure.doNewMeasurement();
    Serial.print("Bus Voltage:   "); Serial.print(measurement.busvoltage); Serial.println(" V");
    Serial.print("Shunt Voltage: "); Serial.print(measurement.shuntvoltage); Serial.println(" mV");
    Serial.print("Load Voltage:  "); Serial.print(measurement.loadvoltage); Serial.println(" V");
    Serial.print("Current:       "); Serial.print(measurement.current_mA); Serial.println(" mA");
    Serial.print("Power:         "); Serial.print(measurement.power_mW); Serial.println(" mW");
    Serial.println("");
  }
}