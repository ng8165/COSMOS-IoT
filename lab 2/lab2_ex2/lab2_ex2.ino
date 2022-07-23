// Exercise 2: Measure a Voltage and Monitoring it with the UART

void setup() {
  Serial.begin(115200);
}

void loop() {
  int sensorValue = analogRead(A3); // print out the digital equivalent of the value read
  Serial.println(sensorValue);
  
  delay(1000); // 1 second delay between readings
}
