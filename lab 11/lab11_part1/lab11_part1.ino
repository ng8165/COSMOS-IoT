// COSMOS Lab 11 Part 1: Bouncing Ball on BoosterPack LCD

#include <driverlib.h>

#include "SPI.h"
#include "Screen_HX8353E.h"
Screen_HX8353E myScreen;

volatile uint8_t timer_flag = 0;
uint8_t x = 50, y = 70, oldx, oldy;
int8_t vx = 1, vy = -1;
uint8_t hits = 0;
const uint8_t radius = 3;
const uint8_t screenMin = 0, screenMax = 127;

void eraseBall(uint8_t x, uint8_t y, uint8_t radius) {
  // draw over old ball with background color to erase it
  myScreen.circle(x, y , radius, blackColour);
}

void drawBall(uint8_t x, uint8_t y, uint8_t radius) {
  // draw ball with default foreground color
  myScreen.circle(x, y , radius, whiteColour);
}

void SysTick_Handler() {
  timer_flag = 1;
}

void setup() {
  WDT_A_hold(WDT_A_BASE);

  // setup SysTick to run every 50ms
  SysTick_registerInterrupt(SysTick_Handler);
  SysTick_setPeriod(2400000);
  SysTick->VAL = 0;
  SysTick_enableModule();
  SysTick_enableInterrupt();
  
  drawBall(x, y, radius);
  
  // Initialize and reset screen
  myScreen.begin();
  myScreen.clear(blackColour);
}

void loop() {
  while (timer_flag == 0);

  timer_flag = 0;
  
  if (x-radius+vx < screenMin || x+radius+vx > screenMax) {
    vx = -vx;
    hits++;
    if (vx > -15 && vx < 15) {
      vx += (vx < 0 ? -1 : 1);
    }
  }
  
  if (y-radius+vy < screenMin || y+radius+vy > screenMax) {
    vy = -vy;
    hits++;
    if (vy > -15 && vy < 15) {
      vy += (vy < 0 ? -1 : 1);
    }
  }

  if (hits > 50) {
    hits = 0;
    vx = (vx < 0 ? -1 : 1);
    vy = (vy < 0 ? -1 : 1);
  }

  oldx = x;
  oldy = y;
  x += vx;
  y += vy;

  eraseBall(oldx, oldy, radius);
  drawBall(x, y, radius);
}
