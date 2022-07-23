//Exercise 1: UART monitor

const int RED = 75;
const int GREEN = 76;
const int BLUE = 77;

void setup() {
  Serial.begin(115200);
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
}

void loop(){
  digitalWrite(RED, HIGH);
  Serial.println("RED");
  delay(500);
  digitalWrite(RED, LOW);

  digitalWrite(GREEN, HIGH);
  Serial.println("GREEN");
  delay(500);
  digitalWrite(GREEN, LOW);

  digitalWrite(BLUE, HIGH);
  Serial.println("BLUE");
  delay(500);
  digitalWrite(BLUE, LOW);
}
