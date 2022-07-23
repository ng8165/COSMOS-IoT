// COSMOS Lab 8, Part 1 - Gesture Detection
// Using Interrupts and ADC14
// Implementing a simple state machine

#include <stdio.h>
#include <driverlib.h>

// LCD Screen libraries
#include <LCD_screen.h>
#include <LCD_screen_font.h>
#include <LCD_utilities.h>
#include <Screen_HX8353E.h>
#include <Terminal12e.h>
#include <Terminal6e.h>
#include <Terminal8e.h>
Screen_HX8353E myScreen;

volatile uint16_t aZ;
volatile uint32_t ms = 0; // milliseconds since last state change
volatile uint16_t gestures_detected = 0;
volatile uint8_t current_state = 0;
const uint16_t threshold = 8000;
const uint16_t tau = 2000;

void drawAccelData() {  
  myScreen.gText(40, 40, "Z: " + String(aZ) + "      ");
  myScreen.gText(40, 60, "G: " + String(gestures_detected) + "      ");
  myScreen.gText(40, 80, "T: " + String(ms/1000) + "." + String(ms%1000/100) + "      ");
  myScreen.gText(40, 100, "S: " + String(current_state));
}

void ADC14_IRQHandler() {
  uint64_t status = MAP_ADC14_getEnabledInterruptStatus();
  MAP_ADC14_clearInterruptFlag(status);

  ms += 100; // every interrupt interation is ~100ms

  if (status & ADC_INT2) { // ADC_MEM2 conversion completed
      aZ = ADC14_getResult(ADC_MEM2);

      drawAccelData();
      
      if (current_state == 0 && aZ < threshold && ms >= tau) {
        current_state = 1;
        ms = 0;
      } else if (current_state == 1 && aZ >= threshold && ms >= tau && ms < 2*tau) {
        current_state = 2;
        ms = 0;
      } else if (current_state == 1 && ms >= 2*tau) {
        current_state = 0;
        ms = 0;
      } else if (current_state == 2 && ms >= tau) {
        current_state = 0;
        ms = 0;
        gestures_detected++;
      }      
  }
}

void LCD_init() {
  myScreen.begin();

  // Find the LCD Screen Size
  Serial.print("X Screen Size: ");
  Serial.println(myScreen.screenSizeX());
  Serial.print("Y Screen Size: ");
  Serial.println(myScreen.screenSizeY());

  // add a title to the screen
  myScreen.gText(8, 15, "Accelerometer Data");
}

void ADC_init() {
  // configure ACC X, Y, Z as ADC inputs
  GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4, GPIO_PIN0 | GPIO_PIN2, GPIO_TERTIARY_MODULE_FUNCTION);
  GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6, GPIO_PIN1, GPIO_TERTIARY_MODULE_FUNCTION);

  ADC14_registerInterrupt(ADC14_IRQHandler);

  // initialize ADC (ADCOSC/64/8)
  ADC14_enableModule();
  ADC14_initModule(ADC_CLOCKSOURCE_ADCOSC, ADC_PREDIVIDER_64, ADC_DIVIDER_8, 0);

  // Configuring ADC Memory (ADC_MEM0 - ADC_MEM2 with no repeat) with internal 2.5v reference
  ADC14_configureMultiSequenceMode(ADC_MEM0, ADC_MEM2, true);
  ADC14_configureConversionMemory(ADC_MEM0, ADC_VREFPOS_AVCC_VREFNEG_VSS, ADC_INPUT_A14, ADC_NONDIFFERENTIAL_INPUTS);
  ADC14_configureConversionMemory(ADC_MEM1, ADC_VREFPOS_AVCC_VREFNEG_VSS, ADC_INPUT_A13, ADC_NONDIFFERENTIAL_INPUTS);
  ADC14_configureConversionMemory(ADC_MEM2, ADC_VREFPOS_AVCC_VREFNEG_VSS, ADC_INPUT_A11, ADC_NONDIFFERENTIAL_INPUTS);

  // enabling the interrupt when a conversion on channel 2 (end of sequence) is complete and enabling conversions
  ADC14_enableInterrupt(ADC_INT2);

  // Enabling Interrupts
  Interrupt_enableInterrupt(INT_ADC14);
  Interrupt_enableMaster();

  // Setting up the sample timer to automatically step through the sequence convert.
  ADC14_enableSampleTimer(ADC_AUTOMATIC_ITERATION);

  // Triggering the start of the sample
  ADC14_enableConversion();
  ADC14_toggleConversionTrigger();
}

void setup() {
  WDT_A_hold(WDT_A_BASE);

  Serial.begin(115200);

  LCD_init();
  ADC_init();
}

void loop() {}
