 // COSMOS Lab 11 Part 2: Pong On BoosterPack LCD

#include <driverlib.h>

#include "SPI.h"
#include "Screen_HX8353E.h"
Screen_HX8353E myScreen;

uint8_t difficulty = 1;
uint16_t score = 0;

// interrupt Flags
volatile uint8_t timer_flag;
volatile uint8_t adc_flag;

// ball variables
uint8_t x = 50, y = 70;
int8_t vx = 1, vy = -1;
const uint8_t radius = 3;
uint32_t count = 0;

// paddle variables
const uint8_t paddleX = 0;
uint8_t paddleY = 50;
const uint8_t paddleWidth = 4, paddleHeight = 20;
volatile uint16_t joystickY;

void eraseBall(uint8_t x, uint8_t y) {
  // draw over old ball with background color to erase it
  myScreen.circle(x, y, radius, blackColour);
}

void drawBall(uint8_t x, uint8_t y) {
  // draw ball with default foreground color
  myScreen.circle(x, y, radius, whiteColour);
}

void erasePaddle(uint8_t y) {
  // erase paddle
  myScreen.dRectangle(paddleX, y, paddleWidth, paddleHeight, blackColour);
}

void drawPaddle(uint8_t y) {
  // draw new paddle
  myScreen.dRectangle(paddleX, y, paddleWidth, paddleHeight, whiteColour);
}

void gameOver() {
  Interrupt_disableMaster();

  // draw game over text on screen
  myScreen.clear(blackColour);
  myScreen.gText(35, 30, "GAME OVER!");
  myScreen.gText(40, 50, "Score: " + String(score));
  myScreen.gText(12, 80, "S1 -> Play Again.");
  myScreen.gText(5, 100, "S2 -> Difficulty: " + String(difficulty));

  uint8_t S1, S2;
  do {
    S1 = GPIO_getInputPinValue(GPIO_PORT_P5, GPIO_PIN1);
    S2 = GPIO_getInputPinValue(GPIO_PORT_P3, GPIO_PIN5);

    if (S2 == GPIO_INPUT_PIN_LOW) {
      difficulty++;
      if (difficulty > 5)
        difficulty = 1;
      myScreen.gText(5, 100, "S2 -> Difficulty: " + String(difficulty));

      while (S2 == GPIO_INPUT_PIN_LOW)
        S2 = GPIO_getInputPinValue(GPIO_PORT_P3, GPIO_PIN5);
    }
  } while (S1 == GPIO_INPUT_PIN_HIGH);
  
  // reset game
  myScreen.clear(blackColour);
  score = 0;
  x = 50, y = 70;
  vx = difficulty, vy = -difficulty;
  paddleY = 50;
  drawBall(x, y);
  drawPaddle(paddleY);
  Interrupt_enableMaster();
}

void SysTick_Handler() {
  timer_flag = 1;
}

void ADC14_IRQHandler() {
  // clear the interrupt flags
  uint64_t status = ADC14_getEnabledInterruptStatus();
  ADC14_clearInterruptFlag(status);

  // ADC_MEM0 conversion completed
  if (status & ADC_INT0 & (adc_flag == 0)) {
    joystickY = ADC14_getResult(ADC_MEM0);
    adc_flag = 1;
  }
}

void ADC_init() {
  GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4, GPIO_PIN4, GPIO_TERTIARY_MODULE_FUNCTION);

  // Initializing ADC (ADCOSC/4/6)
  ADC14_enableModule();
  ADC14_initModule(ADC_CLOCKSOURCE_ADCOSC, ADC_PREDIVIDER_4,ADC_DIVIDER_6,0);

  // Configuring ADC Memory (ADC_MEM0 - ADC_MEM2 (A11, A13, A14)  with no repeat) with internal 2.5v reference
  ADC14_configureSingleSampleMode(ADC_MEM0, true);
  MAP_ADC14_configureConversionMemory(ADC_MEM0, ADC_VREFPOS_AVCC_VREFNEG_VSS, ADC_INPUT_A9, ADC_NONDIFFERENTIAL_INPUTS);

  ADC14_enableInterrupt(ADC_INT0);
  ADC14_setResolution(ADC_12BIT);

  // Enabling Interrupts
  Interrupt_enableInterrupt(INT_ADC14);
  Interrupt_enableMaster();

  // Setting up the sample timer to automatically step through the sequence convert
  ADC14_enableSampleTimer(ADC_AUTOMATIC_ITERATION);

  // Triggering the start of the sample
  ADC14_registerInterrupt(ADC14_IRQHandler);
  ADC14_enableConversion();
  ADC14_toggleConversionTrigger();
}

void setup() {
  WDT_A_hold(WDT_A_BASE);

  // setup buttons
  GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P5, GPIO_PIN1);
  GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P3, GPIO_PIN5);

  // setup SysTick timer
  SysTick_registerInterrupt(SysTick_Handler);
  SysTick_setPeriod(480000);
  SysTick->VAL = 0;
  SysTick_enableModule();
  SysTick_enableInterrupt();
  
  myScreen.begin();
  myScreen.clear(blackColour);
  myScreen.setPenSolid(false);

  drawBall(x, y);
  drawPaddle(paddleY);

  ADC_init();
}

void loop() {
  // STEP 1: Handle ADC Flag
  while (adc_flag == 0);
  adc_flag = 0;

  // STEP 2: Update Paddle Position
  erasePaddle(paddleY);

  // map ADC 12-bit resolution to LCD pixel resolution
  paddleY = map(joystickY, 0, 4096, 128, 0);
  
  // cast paddleY to a fixed number
  if (paddleY < 10) {
    paddleY = 0;
  } else if (paddleY > 120) {
    paddleY = 110;
  } else {
    paddleY = (paddleY/10)*10;
  }

  drawPaddle(paddleY);

  // STEP 3: Handle SysTick Flag
  while (timer_flag == 0);
  timer_flag = 0;

  // STEP 4: Update Ball Position
  if (y-radius+vy < 0 || y+radius+vy > 127) {
    vy = -vy;
  }

  if (x-radius+vx <= 4) {
    if (paddleY < y+vy && y+vy < paddleY+paddleHeight) {
      vx = -vx;
      score++;
    }
  }

  if (x-radius+vx < 0) {
    gameOver();
  }
  
  if (x+radius+vx > 127) {
    vx = -vx;
  }
  
  eraseBall(x, y);
  x += vx;
  y += vy;
  drawBall(x, y);
}
