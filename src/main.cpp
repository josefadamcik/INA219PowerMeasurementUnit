#include <Arduino.h>
#include "Controlls.h"
#include "Measure.h"
#include "Display.h"
#include "UserInterface.h"
#include "avr/interrupt.h"

volatile Button buttonA(PIN_B3);
volatile Button buttonB(PIN_B4);
Measure measure(/*delay*/ 5000, Measure::C16V_400);
Display lcd(0x27);
UserInterface ui(lcd);
Measurement lastMeasurement(0,0,0,0,0,0,0);

// const unsigned long debounceTime = 150;
// volatile bool ledon = false;
// volatile unsigned long btnChangeTime = 0;
// volatile byte btnLastValue = HIGH;
// volatile bool led2on = false;
// // const byte ledpin = PIN_B4;88
// // const byte btnpin = PIN_B3;
// const byte btn2pin = PIN_B4;

ISR(PCINT0_vect) {
    buttonA.interrupt();
    buttonB.interrupt();
    // byte btnValue = digitalRead(btnpin);
    // if (btnValue != btnLastValue) {
    //     btnChangeTime = millis();
    //     btnLastValue = btnValue;
    // } else if (btnChangeTime > 0 && millis() - btnChangeTime >= debounceTime) {
    //     if (btnValue == LOW) {
    //         ledon = !ledon;
    //     }
    //     btnChangeTime = 0;
    // }
    // if (digitalRead(btn2pin) == LOW) {
    //     led2on = !led2on;
    // }
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
  PCMSK = 0b00011000;  // turn on interrupts on pins PB3 and PB4
  sei();               // enables interrupts

  // pinMode(btnpin, INPUT_PULLUP);
  // pinMode(btn2pin, INPUT_PULLUP);

  buttonA.setup();
  buttonB.setup();
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
    // ui.buttonTriggered(UserInterface::Primary);
    ui.ledon = !ui.ledon;
  }

  if (buttonB.checkIfButtonTriggeredAndReset()) {
    // Serial.println(F("Button B triggered"));
    // ui.buttonTriggered(UserInterface::Secondary);
    ui.led2on = !ui.led2on;
  }

  ui.loop();
}