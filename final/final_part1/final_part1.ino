#include <stdio.h>
#include <driverlib.h>
#include <SPI.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <stdlib.h>

// LCD screen libraries
#include <LCD_screen.h>
#include <LCD_screen_font.h>
#include <LCD_utilities.h>
#include <Screen_HX8353E.h>
#include <Terminal12e.h>
#include <Terminal6e.h>
#include <Terminal8e.h>
Screen_HX8353E myScreen;

// networking
char ssid[] = "eec172"; // network name
char server[] = "io.adafruit.com"; // MQTT server
WiFiClient wifiClient;
PubSubClient client(server, 1883, callback, wifiClient);

// acceleration buffers and variables
volatile uint16_t bufferX[5], bufferY[5];
volatile uint32_t sumX = 0, sumY = 0;
volatile uint16_t oldest = 0;

// sensor variables
const uint8_t freq = 5;
volatile uint16_t accX = 0, accY = 0;

// state variables
volatile uint32_t ms = 0;
volatile uint32_t tau = 0; // time since last state change
volatile uint16_t gestures = 0;
volatile float state = 0;

// publish and poll data/variables
const uint16_t publishTime = 3000;
volatile uint8_t gestureFlag = 0;
uint8_t sendValue; // 0 = send x data, 1 = send y data

void drawAccelData() {
  myScreen.gText(40, 15,  "X: " + String(accX) + "     ");
  myScreen.gText(40, 25,  "Y: " + String(accY) + "     ");
  myScreen.gText(40, 105,  "G: " + String(gestures) + "     ");
  myScreen.gText(40, 115,  "S: " + String(state) + "     ");
}

void callback(char* topic, byte* payload, unsigned int length) {
  char* str = (char*) payload;
  sendValue = str[1] == 'N';
}

void ADC14_IRQHandler() {
  uint64_t status = MAP_ADC14_getEnabledInterruptStatus();
  MAP_ADC14_clearInterruptFlag(status);

  // ADC_MEM2 conversion completed
  if (status & ADC_INT2 && ms < publishTime) {
    // Store and average ADC14 conversion results
    
    sumX -= bufferX[oldest];
    bufferX[oldest] = ADC14_getResult(ADC_MEM0); // x accelerometer
    sumX += bufferX[oldest];
    
    sumY -= bufferY[oldest];
    bufferY[oldest] = ADC14_getResult(ADC_MEM1); // y accelerometer
    sumY += bufferY[oldest];

    accX = sumX/5, accY = sumY/5;
    oldest = (oldest+1)%5;

    // gesture detection

    const uint8_t threshold = 200;

    if (state != 0 && tau >= 1000) {
      // timeout, go back to state 0
      state = 0;
      tau = 0;
    }
    
    if (state == 0 && accY > 515 && (accX >= 500 && accX <= 510)) {
      // not moving in x direction, accelerating down in y direction
      // transition to intermediate state and wait for accelerating up in y direction
      state = 0.5;
      tau = 0;
    } else if (state == 0.5 && tau >= threshold && accY < 505 && (accX >= 500 && accX <= 510)) {
      // accelerated up in y direction, not moving in x direction
      // start time buffer
      state = 1;
      tau = 0;
    } else if (state == 1 && accX < 500 && (accY >= 505 && accY <= 515)) {
      // accelerated right in x direction, not moving in y direction
      if (tau < threshold) {
        // if done before the threshold, is invalid
        state = 0;
      } else {
        // otherwise, await accelerate left in x direction
        state = 1.5;
      }

      tau = 0;
    } else if (state == 1.5 && tau >= threshold && accX > 510 && (accY >= 505 && accY <= 515)) {
      // accelerated left in x direction, not moving in y direction
      // start time buffer
      state = 2;
      tau = 0;
    } else if (state == 2 && tau >= 500) {
      gestures++;
      state = 0;
      tau = 0;
      gestureFlag = 1;
    }

    // every interrupt cycle takes around 100 ms (frequency around 10 Hz)
    ms += 100;
    tau += 100;
    
    drawAccelData();
  }
}

void LCD_init() {
  myScreen.begin();
  
  myScreen.gText(8, 5, "Acceleration Data");
  // myScreen.gText(8, 35, "Velocity Data");
  // myScreen.gText(8, 65, "Displacement Data");
  myScreen.gText(8, 95, "Gesture Detected");
}

void ADC_init() {
  // Configures pin 4.0 (ACC Y) and 6.1 (ACC X) as ADC inputs
  GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4, GPIO_PIN0, GPIO_TERTIARY_MODULE_FUNCTION);
  GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6, GPIO_PIN1, GPIO_TERTIARY_MODULE_FUNCTION);
  
  ADC14_registerInterrupt(ADC14_IRQHandler);

  // Initializing ADC (ADCOSC/64/8)
  ADC14_enableModule();
  ADC14_initModule(ADC_CLOCKSOURCE_ADCOSC, ADC_PREDIVIDER_64, ADC_DIVIDER_8, 0);

  // Configuring ADC Memory (ADC_MEM0 and ADC_MEM1 (A13, A14) with no repeat) with internal 2.5v reference
  ADC14_configureMultiSequenceMode(ADC_MEM0, ADC_MEM2, true);
  ADC14_configureConversionMemory(ADC_MEM0, ADC_VREFPOS_AVCC_VREFNEG_VSS, ADC_INPUT_A14, ADC_NONDIFFERENTIAL_INPUTS);
  ADC14_configureConversionMemory(ADC_MEM1, ADC_VREFPOS_AVCC_VREFNEG_VSS, ADC_INPUT_A13, ADC_NONDIFFERENTIAL_INPUTS);
  
  ADC14_enableInterrupt(ADC_INT2);

  // Enabling Interrupts
  Interrupt_enableInterrupt(INT_ADC14);
  Interrupt_enableMaster();

  // Setting up the sample timer to automatically step through the sequence convert
  ADC14_enableSampleTimer(ADC_AUTOMATIC_ITERATION);
  ADC14_setResolution(ADC_10BIT);

  // Triggering the start of the sample
  ADC14_enableConversion();
  ADC14_toggleConversionTrigger();
}

void setup() {
  WDT_A_hold(WDT_A_BASE);
  
  Serial.begin(115200);
  
  Serial.print("Attempting to connect to network named: ");
  Serial.print(ssid);
  WiFi.begin(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  
  Serial.println("\nNow connected to a network.\n");
  
  LCD_init();
  ADC_init();
}

void loop() {
  if (ms < publishTime) return;

  char str[5];

  while (!client.connected()) {
    Serial.print("Connection to Adafruit ");

    if (!client.connect("energiaClient", "ng8165", "aio_eqkc40ouIZih4PUlD3Jhu6UbnLeq")) {
      Serial.println("failed.");
    } else {
      Serial.println("succeeded.");
      break;
    }
  }

  sprintf(str, "%d", gestureFlag);
  if (client.publish("ng8165/feeds/iot-l-detection.gesture-detection", str)) {
    Serial.print("Gesture Detection succeeded. ");
    gestureFlag = 0;
  } else {
    Serial.print("Gesture Detection failed. ");
  }

  if (client.subscribe("ng8165/feeds/iot-l-detection.in-topic")) {
    Serial.print("In Topic successful. ");
    client.poll();
  } else {
    Serial.print("In Topic failed. ");
  }

  if (sendValue) {
    sprintf(str, "%d", accY);
    if (client.publish("ng8165/feeds/iot-l-detection.y-axis", str)) {
      Serial.println("Y-Axis succeeded.");
    } else {
      Serial.println("Y-Axis failed.");
    }
  } else {
    sprintf(str, "%d", accY);
    if (client.publish("ng8165/feeds/iot-l-detection.x-axis", str)) {
      Serial.println("X-Axis succeeded.");
    } else {
      Serial.println("X-Axis failed.");
    }
  }

  ms = 0;
}
