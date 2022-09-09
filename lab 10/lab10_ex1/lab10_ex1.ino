// COSMOS Lab 10

#include <driverlib.h>
#include <SPI.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <stdlib.h>
#include <stdio.h>

// LCD Screen libraries
#include <LCD_screen.h>
#include <LCD_screen_font.h>
#include <LCD_utilities.h>
#include <Screen_HX8353E.h>
#include <Terminal12e.h>
#include <Terminal6e.h>
#include <Terminal8e.h>
Screen_HX8353E myScreen;

const uint16_t mic_high = 600;
const uint16_t mic_low = 512;
const uint16_t threshold = 50;
volatile uint16_t counter = 0;
volatile uint16_t micBuffer[5];
volatile uint8_t LED;

char ssid[] = "eec172"; // network name
char server[] = "io.adafruit.com"; // MQTT Server

WiFiClient wifiClient;
PubSubClient client(server, 1883, callback, wifiClient);

uint8_t mapToLED(uint16_t in) {
  uint8_t out;
  
  if (in < mic_low) {
    out = 0;
  } else if (in > mic_high) {
    out = 255;
  } else {
    out = (uint8_t) ((in - mic_low) * 255.0 / (mic_high - mic_low));
  }
  
  return out;
}

void ADC14_IRQHandler() {
  // Clear interrupt flags
  uint64_t status = ADC14_getEnabledInterruptStatus();
  ADC14_clearInterruptFlag(status);

  counter++;

  if (status & ADC_INT0 && counter <= threshold) {
    // shift micBuffer to the right
    for (int i=4; i>0; i--)
      micBuffer[i] = micBuffer[i-1];

    // place new value in empty spot
    micBuffer[0] = ADC14_getResult(ADC_MEM0);
    micBuffer[0] = abs(micBuffer[0] - 512) + 512;

    // average and display on LCD screen and LED
    uint32_t avg = 0;
    for (int i=0; i<5; i++)
      avg += micBuffer[i];

    uint16_t mic = avg/5;
    LED = mapToLED(mic);
    analogWrite(39, LED);
    
    myScreen.gText(40, 40, "Mic: " + String(mic) + "     ");
    myScreen.gText(40, 60, "LED: " + String(LED) + "     ");
    myScreen.gText(40, 80, "Cnt: " + String(counter) + "     ");
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Received message (topic: ");
  Serial.print(topic);
  Serial.print(", length: ");
  Serial.print(length);
  Serial.print("): \"");
  Serial.write(payload, length);
  Serial.println("\"");
}

void publishToBroker(uint8_t input) {
  while (!client.connected()) {
    Serial.print("Connecting... ");

    if (!client.connect("energiaClient", "ng8165", "aio_YOoT86vFIEE2dhRchNNUb47Fen7j")) {
      Serial.println("Connection failed.");
    } else {
      Serial.print("Connection succeeded. ");
      break;
    }
  }

  char str[5] = {0};
  sprintf(str, "%d", input);

  if (client.publish("ng8165/feeds/iot-microphone", str)) {
    Serial.println("Publish succeeded.");
  } else {
    Serial.println("Publish failed.");
  }
}

void ADC_init() {
  GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4, GPIO_PIN3, GPIO_TERTIARY_MODULE_FUNCTION);
  
  ADC14_registerInterrupt(ADC14_IRQHandler);

  // Initializing ADC (ADCOSC/64/8)
  ADC14_enableModule();
  ADC14_initModule(ADC_CLOCKSOURCE_ADCOSC, ADC_PREDIVIDER_64, ADC_DIVIDER_8, 0);

  // Configuring ADC Memory
  ADC14_configureSingleSampleMode(ADC_MEM0, true);
  ADC14_configureConversionMemory(ADC_MEM0, ADC_VREFPOS_AVCC_VREFNEG_VSS, ADC_INPUT_A10, false);

  // Enabling Interrupts
  ADC14_enableInterrupt(ADC_INT0);
  Interrupt_enableInterrupt(INT_ADC14);
  Interrupt_enableMaster();

  // Setting up the sample timer to automatically step through the sequence convert
  ADC14_enableSampleTimer(ADC_AUTOMATIC_ITERATION);
  ADC14_setResolution(ADC_10BIT);

  // Triggering the start of the sample
  ADC14_enableConversion();
  ADC14_toggleConversionTrigger();
}

void LCD_init() {
  myScreen.begin();
  myScreen.gText(20, 20, "Microphone ADC:");
}

void setup() {
  WDT_A_hold(WDT_A_BASE);

  Serial.begin(115200);

  // try to connect to network
  Serial.print("Attempting to connect to network named: ");
  Serial.print(ssid);
  WiFi.begin(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  
  Serial.println("\nNow connected to a network.\n");

  while (!client.connected()) {
    Serial.print("Connecting... ");

    if (!client.connect("energiaClient", "ng8165", "aio_YOoT86vFIEE2dhRchNNUb47Fen7j")) {
      Serial.println("Connection failed.");
    } else {
      Serial.println("Connection succeeded.\n");
      break;
    }
  }

  LCD_init();
  ADC_init();
}

void loop() {
  if (counter >= threshold) {
    Interrupt_disableInterrupt(INT_ADC14);
    publishToBroker(LED);
    counter = 0;
    Interrupt_enableInterrupt(INT_ADC14);
  }
}
