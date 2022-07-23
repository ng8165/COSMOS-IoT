#include <driverlib.h>

int count = 0;

void setup() {
  // put your setup code here, to run once:

  WDT_A_hold(WDT_A_BASE);

  // Configure the pin as an output
  GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2);
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
  RGB_output(count);
  count = (count+1)%8;
  delay(1000);
}
