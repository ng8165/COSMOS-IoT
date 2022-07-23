#include <driverlib.h>

void setup() {
  WDT_A_hold(WDT_A_BASE);

  Serial.begin(115200);

  GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
  GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1);
  GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
}

void setTrafficLight(int color) {
  // 1 = red, 2 = yellow, 3 = green
  GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1);
  
  switch (color) {
    case 1:
      GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN0);
      break;
    case 2:
      GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1);
      break;
    case 3:
      GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN1);
      break;
  }
}

void setPedLight(int state) {
  // 0 = off, 1 = on

  GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);

  if (state == 1)
    GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
}

int delay_and_poll(int seconds) {
  int press = 0;

  for (int i=0; i<seconds; i++) {
    for (int j=0; j<20; j++) {
      delay(50);
      
      if (GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN1) == LOW)
        press = 1;
    }
  }

  return press;
}

void loop() {
  setTrafficLight(3);
  setPedLight(1);

  int ped = delay_and_poll(8);
  while (ped == 0)
    ped = delay_and_poll(1);

  setTrafficLight(2);
  setPedLight(1);

  delay_and_poll(4);

  setTrafficLight(1);
  setPedLight(0);

  delay_and_poll(8);
}
