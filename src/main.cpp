#include <Arduino.h>
// #include "Controlls.h"
// #include "Measure.h"
#include "Display.h"
// #include "UserInterface.h"


// volatile Button buttonA(PIN3);
// volatile Button buttonB(PIN4);
// Measure measure(/*delay*/ 5000, Measure::C16V_400);
Display lcd(0x27);
// UserInterface ui(lcd);
// Measurement lastMeasurement(0,0,0,0,0,0,0);

// void btn_interrupt_a() {
//   buttonA.interrupt();
// }

// void btn_interrupt_b() {
//   buttonB.interrupt();
// }

void setup(void) 
{
  // Serial.begin(115200);
  // while (!Serial) {
  //     // will pause Zero, Leonardo, etc until serial console opens
  //     delay(1);
  // }

  // buttonA.setup(btn_interrupt_a);
  // buttonB.setup(btn_interrupt_b);
  // measure.setup();
  // ui.setup(measure.getCalibration());
  lcd.setup();
  lcd.printHello("tralala");
  const char _top_menu_2[] = "Calibration";
  lcd.printMenuRow(0, false, _top_menu_2);
  lcd.printValue(F("Voltage"), 1000, F("mV"));
  lcd.clear();
}

void loop(void) 
{

  // if (measure.didIntervalElapsed()) {
    // lastMeasurement = measure.doNewMeasurement();
    // Serial.print(F("Bus Voltage:     ")); Serial.print(lastMeasurement.busvoltage); Serial.println(F(" V"));
    // Serial.print(F("Shunt Voltage:   ")); Serial.print(lastMeasurement.shuntvoltage); Serial.println(F(" mV"));
    // Serial.print(F("Load Voltage:    ")); Serial.print(lastMeasurement.loadvoltage); Serial.println(F(" V"));
    // Serial.print(F("Current:         ")); Serial.print(lastMeasurement.current_mA); Serial.println(F(" mA"));
    // Serial.print(F("Power:           ")); Serial.print(lastMeasurement.power_mW); Serial.println(F(" mW"));
    // Serial.print(F("Energy:          ")); Serial.print(lastMeasurement.energy_mWh); Serial.println(F(" mWh"));
    // Serial.print(F("Energy interval: ")); Serial.print(lastMeasurement.energyMillis / 1000); Serial.println(F(" s"));
    // Serial.print(F("delta(Energy):   ")); Serial.print(lastMeasurement.energyDelta); Serial.println(F(" mWh"));
    // Serial.println();
    // ui.updateLastMeasurement(lastMeasurement);
  // }

  // if (buttonA.checkIfButtonTriggeredAndReset()) {
  //   // Serial.println(F("Button A triggered"));
  //   ui.buttonTriggered(UserInterface::Primary);
  // }

  // if (buttonB.checkIfButtonTriggeredAndReset()) {
  //   // Serial.println(F("Button B triggered"));
  //   ui.buttonTriggered(UserInterface::Secondary);
  // }


  // ui.loop();
}