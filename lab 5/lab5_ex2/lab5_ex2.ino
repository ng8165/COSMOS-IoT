#include <driverlib.h>

int count = 0;

void setup() {
  // GPIO documentation: https://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSPWare/2_00_00_41/exports/MSPWare_2_00_00_41/driverlib/doc/MSP430F5xx_6xx/html/group__gpio__api.html
  WDT_A_hold(WDT_A_BASE);

  GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2);
  GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2);
  GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
}

void RGB_output(int count) {
  int pins[] = {GPIO_PIN0, GPIO_PIN1, GPIO_PIN2};

  for (int i=0; i<3; i++) {
    if (count&1 == 1)
      GPIO_setOutputHighOnPin(GPIO_PORT_P2, pins[i]);
    else
      GPIO_setOutputLowOnPin(GPIO_PORT_P2, pins[i]);
    
    count >>= 1;
  }
}

void loop() {
  while (GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN1) == LOW);
  
  count = (count+1)%8;
  RGB_output(count);

  delay(100);
  while (GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN1) == HIGH);
  delay(100);
}
