//Exercise 5: LED brightness control with analogWrite() and For loops int ext_LED = XX; //analog output pin

const int ext_LED = 38; //analog output pin

void setup() {
  pinMode(ext_LED, OUTPUT);
}

void loop() {
  for (int val=255; val>0; val--) {
    analogWrite(ext_LED, val);
    delay(10);
  }
  
  for (int val=0; val<255; val++) {
    analogWrite(ext_LED, val);
    delay(10);
  }
}
