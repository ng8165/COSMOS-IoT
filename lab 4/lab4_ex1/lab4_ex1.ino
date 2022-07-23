#include "pitches.h";

#define ELEMENTS(x) (sizeof(x) / sizeof(x[0]))

int buzzerPin = 19; 
int S1 = 5;
int S2 = 6;
int S3 = 7;
int S4 = 8;
int S5 = 9;

void setup() {
  Serial.begin(115200);
  pinMode(buzzerPin, OUTPUT); // set buzzerPin to OUTPUT
  pinMode(S1, INPUT_PULLUP);
  pinMode(S2, INPUT_PULLUP);
  pinMode(S3, INPUT_PULLUP);
  pinMode(S4, INPUT_PULLUP);
}

int melody1[] = {NOTE_C5, NOTE_C5, NOTE_C5, NOTE_C5, NOTE_GS4, NOTE_AS4, NOTE_C5, NOTE_AS4, NOTE_C5};  

int tempo1[] = {10, 10, 10, 4, 4, 4, 5, 16, 1};

int melody2[] = {
  NOTE_E7, NOTE_E7, 0, NOTE_E7,
  0, NOTE_C7, NOTE_E7, 0,
  NOTE_G7, 0, 0,  0,
  NOTE_G6, 0, 0, 0,
 
  NOTE_C7, 0, 0, NOTE_G6,
  0, 0, NOTE_E6, 0,
  0, NOTE_A6, 0, NOTE_B6,
  0, NOTE_AS6, NOTE_A6, 0,
 
  NOTE_G6, NOTE_E7, NOTE_G7,
  NOTE_A7, 0, NOTE_F7, NOTE_G7,
  0, NOTE_E7, 0, NOTE_C7,
  NOTE_D7, NOTE_B6, 0, 0,
 
  NOTE_C7, 0, 0, NOTE_G6,
  0, 0, NOTE_E6, 0,
  0, NOTE_A6, 0, NOTE_B6,
  0, NOTE_AS6, NOTE_A6, 0,
 
  NOTE_G6, NOTE_E7, NOTE_G7,
  NOTE_A7, 0, NOTE_F7, NOTE_G7,
  0, NOTE_E7, 0, NOTE_C7,
  NOTE_D7, NOTE_B6, 0, 0
};

int tempo2[] = {
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
 
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
 
  9, 9, 9,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
 
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
 
  9, 9, 9,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
};

void playSong(int tunes[], int siz1, int durs[]) {  
  for (int thisNote = 0; thisNote < siz1; thisNote++) {
    int noteDuration = 1000/durs[thisNote];
    tone(buzzerPin, tunes[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration + 50; // delay between pulse
    delay(pauseBetweenNotes);
    noTone(buzzerPin); // stop the tone playing
  }
}

void beep(int note, int duration) {
  tone(buzzerPin, note, duration/2);
  delay(duration/2);
  noTone(buzzerPin);
  delay(duration/2 + 20);  
}

void song3() {
  beep(note_a, 500);
  beep(note_a, 500);
  beep(note_a, 500);
  beep(note_ff, 350);
  beep(note_cH, 150);  
  beep(note_a, 500);
  beep(note_ff, 350);
  beep(note_cH, 150);
  beep(note_a, 650);
  delay(150);
  beep(note_eH, 500);
  beep(note_eH, 500);
  beep(note_eH, 500);   
  beep(note_fH, 350);
  beep(note_cH, 150);
  beep(note_gS, 500);
  beep(note_ff, 350);
  beep(note_cH, 150);
  beep(note_a, 650);
  delay(150);
  beep(note_aH, 500);
  beep(note_a, 300);
  beep(note_a, 150);
  beep(note_aH, 400);
  beep(note_gSH, 200);
  beep(note_gH, 200); 
  beep(note_fSH, 125);
  beep(note_fH, 125);    
  beep(note_fSH, 250);
  delay(250);
  beep(note_aS, 250); 
  beep(note_dSH, 400); 
  beep(note_dH, 200);  
  beep(note_cSH, 200);  
  beep(note_cH, 125);  
  beep(note_b, 125);  
  beep(note_cH, 250);  
  delay(250);
  beep(note_ff, 125);  
  beep(note_gS, 500);  
  beep(note_ff, 375);  
  beep(note_a, 125);
  beep(note_cH, 500);
  beep(note_a, 375);  
  beep(note_cH, 125);
  beep(note_eH, 650);
  beep(note_aH, 500);
  beep(note_a, 300);
  beep(note_a, 150);
  beep(note_aH, 400);
  beep(note_gSH, 200);
  beep(note_gH, 200);
  beep(note_fSH, 125);
  beep(note_fH, 125);    
  beep(note_fSH, 250);
  delay(250);
  beep(note_aS, 250);  
  beep(note_dSH, 400);  
  beep(note_dH, 200);  
  beep(note_cSH, 200);  
  beep(note_cH, 125);  
  beep(note_b, 125);  
  beep(note_cH, 250);      
  delay(250);
  beep(note_ff, 250);  
  beep(note_gS, 500);  
  beep(note_ff, 375);  
  beep(note_cH, 125);
  beep(note_a, 500);   
  beep(note_ff, 375);   
  beep(note_cH, 125); 
  beep(note_a, 650);
}

int melody4[] = {
  NOTE_FS5, NOTE_FS5, NOTE_D5, NOTE_B4, NOTE_B4, NOTE_E5, 
  NOTE_E5, NOTE_E5, NOTE_GS5, NOTE_GS5, NOTE_A5, NOTE_B5, 
  NOTE_A5, NOTE_A5, NOTE_A5, NOTE_E5, NOTE_D5, NOTE_FS5, 
  NOTE_FS5, NOTE_FS5, NOTE_E5, NOTE_E5, NOTE_FS5, NOTE_E5
};

int tempo4[] = {
  8, 8, 8, 4, 4, 4, 
  4, 5, 8, 8, 8, 8, 
  8, 8, 8, 4, 4, 4, 
  4, 5, 8, 8, 8, 8
};

int melody5[] = {
  NOTE_C5
};

int tempo5[] = {
  8
};

void loop() {
  int SV1 = digitalRead(S1);
  int SV2 = digitalRead(S2);
  int SV3 = digitalRead(S3);
  int SV4 = digitalRead(S4);
  int SV5 = digitalRead(S5);

  if (SV1 == LOW) {
    playSong(melody1, ELEMENTS(melody1), tempo1);
  }
  
  if (SV2 == LOW) {
    playSong(melody2, ELEMENTS(melody2), tempo2);
  }

  if (SV3 == LOW) {
    song3();
  }

  if (SV4 == LOW) {
    playSong(melody4, ELEMENTS(melody4), tempo4);
  }

  if (SV5 == LOW) {
    playSong(melody5, ELEMENTS(melody5), tempo5);
  }
}
