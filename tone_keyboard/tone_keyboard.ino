// Simple 8 note organ (C major key)
// created 15 May 2017
// by Ryan Paul Wilson

#include "pitches.h"

int buttonState = 0;
const int buttonStartPin = 2;
const int buttonCount = 8;
const int speakerPin = 12;

int notesOctave1[] = {
  NOTE_C4,
  NOTE_D4,
  NOTE_E4,
  NOTE_F4,
  NOTE_G4,
  NOTE_A4,
  NOTE_B4,
  NOTE_C5
};

void setup() {
  // button pins
  for (int input = buttonStartPin; input < buttonStartPin + buttonCount; input++) {
    pinMode(input, INPUT_PULLUP);
  }

  // speaker pin
  pinMode(speakerPin, OUTPUT);
}

void loop() {
  boolean keyPressed = false;
  for (int input = buttonStartPin; input < buttonStartPin + buttonCount; input++) {
    // get a sensor reading
    buttonState = digitalRead(input);

    // play sound for key
    if (buttonState == LOW && keyPressed == false) {
      keyPressed = true;
      tone(speakerPin, notesOctave1[input-buttonStartPin]);
    }
  }
  
  // silence speaker if no button pressed
  if (!keyPressed) {
      noTone(speakerPin);
  }
  
  // stabalizing delay
  delay(1);
}



