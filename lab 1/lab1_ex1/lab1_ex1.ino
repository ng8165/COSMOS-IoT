//My first program 
 
int LED = 75; 
 
void setup() { 
  // put your setup code here, to run once: 
  pinMode(LED, OUTPUT); //variable LED (or pin 75 is set as an output) 
} 
 
void loop() { 
  //make it blink! 
  digitalWrite(LED, HIGH); //Turn the LED on 
  delay(1000); //wait 1000ms 
  digitalWrite(LED, LOW); //Turn the LED off 
  delay(1000); //wait 1000ms 
}
