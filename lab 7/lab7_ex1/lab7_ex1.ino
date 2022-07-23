#include <driverlib.h>

volatile uint32_t seconds;
volatile uint8_t hundredths;
volatile uint8_t count;

void RGB_output(uint8_t count) {
  int pins[] = {GPIO_PIN0, GPIO_PIN1, GPIO_PIN2};

  for (uint8_t i=0; i<3; i++) {
    if (count&1 == 1)
      GPIO_setOutputHighOnPin(GPIO_PORT_P2, pins[i]);
    else
      GPIO_setOutputLowOnPin(GPIO_PORT_P2, pins[i]);
    
    count >>= 1;
  }
}

void SysTick_Handler() {
  hundredths++;

  if (hundredths >= 100) {
    hundredths = 0;
    seconds++;

    count = (count+1)%8;
    RGB_output(count);
  }
}

void setup() {
  WDT_A_hold(WDT_A_BASE);

  Serial.begin(115200);

  GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1 | GPIO_PIN4);
  GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2);

  SysTick_registerInterrupt(SysTick_Handler);
  SysTick_setPeriod(480000); // 48,000,000 / 100 = 480,000
  SysTick_disableInterrupt();
}

void loop() {
  Serial.println("Press S1 to Start, S2 to Stop");

  while (GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN1) == GPIO_INPUT_PIN_HIGH)
    for (uint16_t i=0; i<1000; i++);

  seconds = hundredths = count = 0;
  GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2);
  
  SysTick->VAL = 0;
  SysTick_enableInterrupt();

  while (GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN4) == GPIO_INPUT_PIN_HIGH)
    for (uint16_t i=0; i<1000; i++);

  SysTick_disableInterrupt();

  Serial.print(seconds);
  Serial.print(".");
  Serial.println(hundredths);
}
