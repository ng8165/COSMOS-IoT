#include <driverlib.h>
#include <SPI.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <stdio.h>
#include <string.h>

uint8_t count = 0;
uint8_t IO_button;

char ssid[] = "eec172";
char server[] = "io.adafruit.com";

void callback(char* topic, byte* payload, unsigned int length) {
  char* str = (char*) payload;
  IO_button = (str[1] == 'N') ? 1 : 0;
}

WiFiClient wifiClient;
PubSubClient client(server, 1883, callback, wifiClient);

void printWifiStatus() {
  // print the network name
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength
  long rssi = WiFi.RSSI();
  Serial.print("Strength: ");
  Serial.print(rssi);
  Serial.println(" dBm");
}

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

void pollBroker() {
  while (!client.connected()) {
    Serial.print("Connecting... ");

    if (!client.connect("energiaClient", "ng8165", "aio_YOoT86vFIEE2dhRchNNUb47Fen7j")) {
      Serial.println("Connection failed.");
    } else {
      Serial.print("Connection successful. ");
      
      if (client.subscribe("ng8165/feeds/button1")) {
        Serial.println("Subscription successful.");
        break;
      } else {
        Serial.println("Subscription failed.");
      }
    }

    delay(1000);
  }

  client.poll();
  // Serial.print("IO button: ");
  // Serial.println(IO_button);
  
  delay(100);
}

void setup() {
  WDT_A_hold(WDT_A_BASE);

  // set output and input pins
  GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2);
  GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2);
  GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);

  Serial.begin(115200);

  // try to connect to network
  Serial.print("Attempting to connect to network named: ");
  Serial.print(ssid);
  WiFi.begin(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }

  // wait for IP address
  Serial.println("\n\nNow connected to a network.");
  Serial.print("Waiting for an IP address");
  while (WiFi.localIP() == INADDR_NONE) {
    Serial.print(".");
    delay(300);
  }

  // print wifi status
  Serial.println("\n\nIP Address obtained.");
  printWifiStatus();
  Serial.println();
}

void loop() {
  do {
    pollBroker();
  } while (GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN1) == GPIO_INPUT_PIN_HIGH && IO_button == 0);
  
  count = (count+1)%8;
  RGB_output(count);
  
  delay(100);
  
  do {
    pollBroker();
  } while (GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN1) == GPIO_INPUT_PIN_LOW || IO_button == 1);
  
  delay(100);
}
