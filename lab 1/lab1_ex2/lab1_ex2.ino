const int RED = 75;
const int GREEN = 76;
const int BLUE = 77;

void setup() { 
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
}

void blink(int LED) {
  digitalWrite(LED, HIGH);
  delay(500);
  digitalWrite(LED, LOW);
}

void blink2(int LED1, int LED2) {
  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, HIGH);
  delay(500);
  digitalWrite(LED2, LOW);
  digitalWrite(LED2, LOW);

}
 
void loop() { 
  blink(RED);
  blink(GREEN);
  blink(BLUE);
  blink2(RED, GREEN);
}
