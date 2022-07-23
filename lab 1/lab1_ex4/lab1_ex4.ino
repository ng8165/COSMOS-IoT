//Exercise 4: Toggle external LED with pushbutton and if/else statements

const int buttonPin = 73; //equivalently PUSH1 const int ext_LED = 10; //digital write pin
const int ext_LED = 10; //digital write pin

int buttonState = 0;

void setup() {
  pinMode(ext_LED, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
}

void loop() {
  buttonState = digitalRead(buttonPin);
  
  if (buttonState == HIGH) {
    digitalWrite(ext_LED, HIGH);
  } else {
    digitalWrite(ext_LED,LOW);
  }
}
