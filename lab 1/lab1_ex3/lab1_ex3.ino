int ext_LED = 10; //digital write pin

void setup() {
  pinMode(ext_LED, OUTPUT);
}
void loop() {
  digitalWrite(ext_LED, HIGH);
  delay(1000);
  digitalWrite(ext_LED, LOW);
  delay(1000);
}
