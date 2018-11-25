#include <Arduino.h>
#include "Controlls.h"
#include "Measure.h"
#include "Display.h"
#include "UserInterface.h"
#include "avr/interrupt.h"

volatile Button buttonA(PIN3);
volatile Button buttonB(PIN4);
Measure measure(/*delay*/ 5000, Measure::C16V_400);
Display lcd(0x27);
UserInterface ui(lcd);
Measurement lastMeasurement(0,0,0,0,0,0,0);



void btn_interrupt_a() {
  buttonA.interrupt();
}

void btn_interrupt_b() {
  buttonB.interrupt();
}

ISR(PCINT0_vect) {
    buttonA.interrupt();
    buttonB.interrupt();
}

void setup(void) 
{
  // Serial.begin(115200);
  // while (!Serial) {
  //     // will pause Zero, Leonardo, etc until serial console opens
  //     delay(1);
  // }

  //interrupts on attiny85
  GIMSK = 0b00100000;  // turns on pin change interrupts
  PCMSK = 0b00011000;  // turn on interrupts on pins PB0, PB1, &amp;amp; PB4
  sei();               // enables interrupts
  
  // buttonA.setup(btn_interrupt_a);
  // buttonB.setup(btn_interrupt_b);
  measure.setup();
  ui.setup(measure.getCalibration());
}

void loop(void) 
{

  if (measure.didIntervalElapsed()) {
    lastMeasurement = measure.doNewMeasurement();
    // Serial.print(F("Bus Voltage:     ")); Serial.print(lastMeasurement.busvoltage); Serial.println(F(" V"));
    // Serial.print(F("Shunt Voltage:   ")); Serial.print(lastMeasurement.shuntvoltage); Serial.println(F(" mV"));
    // Serial.print(F("Load Voltage:    ")); Serial.print(lastMeasurement.loadvoltage); Serial.println(F(" V"));
    // Serial.print(F("Current:         ")); Serial.print(lastMeasurement.current_mA); Serial.println(F(" mA"));
    // Serial.print(F("Power:           ")); Serial.print(lastMeasurement.power_mW); Serial.println(F(" mW"));
    // Serial.print(F("Energy:          ")); Serial.print(lastMeasurement.energy_mWh); Serial.println(F(" mWh"));
    // Serial.print(F("Energy interval: ")); Serial.print(lastMeasurement.energyMillis / 1000); Serial.println(F(" s"));
    // Serial.print(F("delta(Energy):   ")); Serial.print(lastMeasurement.energyDelta); Serial.println(F(" mWh"));
    // Serial.println();
    ui.updateLastMeasurement(lastMeasurement);
  }

  if (buttonA.checkIfButtonTriggeredAndReset()) {
    // Serial.println(F("Button A triggered"));
    ui.buttonTriggered(UserInterface::Primary);
  }

  if (buttonB.checkIfButtonTriggeredAndReset()) {
    // Serial.println(F("Button B triggered"));
    ui.buttonTriggered(UserInterface::Secondary);
  }


  ui.loop();
}