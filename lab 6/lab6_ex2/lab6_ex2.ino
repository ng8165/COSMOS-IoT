// COSMOS Lab 6, Part 2 - An Implementation of Morse Code

#include <driverlib.h>
#include <stdio.h>

const int threshold = 200000; // Compare time_passed to this to see if input is dot or dash
int input[5]; // Store either dot or dash in this array

// Establish the Morse Code Reference Arrays
const int zero[5] = {1, 1, 1, 1, 1};
const int one[5] = {0, 1, 1, 1, 1};
const int two[5] = {0, 0, 1, 1, 1};
const int three[5] = {0, 0, 0, 1, 1};
const int four[5] = {0, 0, 0, 0, 1};
const int five[5] = {0, 0, 0, 0, 0};
const int six[5] = {1, 0, 0, 0, 0};
const int seven[5] = {1, 1, 0, 0, 0};
const int eight[5] = {1, 1, 1, 0, 0};
const int nine[5] = {1, 1, 1, 1, 0};

void setup() {
  WDT_A_hold(WDT_A_BASE);

  Serial.begin(115200);

  GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
}

int compare_array(int a[], const int b[], int size) {
  for (int i=0; i<size; i++) {
    if (a[i] != b[i])
      return 0;
  }
  
  return 1;
}

void loop() {  
  for (int i=0; i<5; i++) {
    while (GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN1) == HIGH);
    
    int pressed = 0;
    while (GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN1) == LOW)
      pressed++;

    input[i] = pressed >= threshold ? 1 : 0;
  }

  if (compare_array(input, zero, 5))
    Serial.println("0");
  else if (compare_array(input, one, 5))
    Serial.println("1");
  else if (compare_array(input, two, 5))
    Serial.println("2");
  else if (compare_array(input, three, 5))
    Serial.println("3");
  else if (compare_array(input, four, 5))
    Serial.println("4");
  else if (compare_array(input, five, 5))
    Serial.println("5");
  else if (compare_array(input, six, 5))
    Serial.println("6");
  else if (compare_array(input, seven, 5))
    Serial.println("7");
  else if (compare_array(input, eight, 5))
    Serial.println("8");
  else if (compare_array(input, nine, 5))
    Serial.println("9");
  else
    Serial.println("Invalid Input!");
}
