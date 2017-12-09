/*
  Input GPIO to RGB LED test
  Ryan Wilson (c) 2017
 */

// init the Pins used for PWM
const int R_PIN = 9;
const int G_PIN = 10;
const int B_PIN = 11;

const int R_IDX = 0;
const int G_IDX = 1;
const int B_IDX = 2;

const float R_MULT = 0.15;
const float G_MULT = 0.15;
const float B_MULT = 0.15;

// input pins
const int INPUT_1 = 8;
const int INPUT_2 = 7;
const int INPUT_3 = 6;
const int INPUT_4 = 5;

int val_1 = 0;
int val_2 = 0;
int val_3 = 0;
int val_4 = 0;

int state = 0;

void setup() {
  pinMode(INPUT_1, INPUT);
  pinMode(INPUT_2, INPUT);
  pinMode(INPUT_3, INPUT);
  pinMode(INPUT_4, INPUT);

  pinMode(R_PIN, OUTPUT);
  pinMode(G_PIN, OUTPUT);
  pinMode(B_PIN, OUTPUT);
  
  randomSeed(analogRead(0));

  // for debug
  Serial.begin(9600);
}

//States
//0 no data           // black
//1 DOD Queue = 0     // blue
//2 DOD Queue = 1-5   // white
//3 DOD Queue = 6-10  // yellow
//4 DOD Queue = 11-15 // orange
//5 DOD Queue = 15-20 // red
//6 DOD Queue = 20+   // pink
//7 DOD Queue contains BLOCKER // pulsing red to black
//8 animation 1 (random)
//9 animation 2 (rainbow cycle)
//10 animation 3 (fire animation 1 particles up?)
//11 animation 4 (spin animation)

void loop() {
  val_4 = digitalRead(INPUT_1);
  val_3 = digitalRead(INPUT_2);
  val_2 = digitalRead(INPUT_3);
  val_1 = digitalRead(INPUT_4);

//  Serial.print(val_1);
//  Serial.print(" ");
//  Serial.print(val_2);
//  Serial.print(" ");
//  Serial.print(val_3);
//  Serial.print(" ");
//  Serial.println(val_4);

  // combine binary digits to integer
  state = val_1 + (val_2*2) + (val_3*4) + (val_4*8);
//  Serial.println(state);

//States
//0 no data           // black
//1 DOD Queue = 0     // blue
//2 DOD Queue = 1-5   // white
//3 DOD Queue = 6-10  // yellow
//4 DOD Queue = 11-15 // orange
//5 DOD Queue = 15-20 // red
//6 DOD Queue = 20+   // pink
//7 DOD Queue contains BLOCKER // green (for now)
  switch (state) {
    case 0: // no data
      setLEDColour(0,0,0); // black
    break;
    case 1: // DOD Queue = 0
      setLEDColour(0,0,255); // blue
    break;
    case 2: // DOD Queue = 1-5
      setLEDColour(255,255,255); // white
    break;
    case 3: // DOD Queue = 6-10
      setLEDColour(255,255,0); // yellow
    break;
    case 4: // DOD Queue = 11-15
      setLEDColour(255,127,0); // orange
    break;
    case 5: // DOD Queue = 15-20
      setLEDColour(255,0,0); // red
    break;
    case 6: // DOD Queue = 20+
      setLEDColour(255,0,127); // rose
    break;
    case 7: // DOD Queue contains BLOCKER
      setLEDColour(0,255,0); // green (for now)
    break;
    case 8:
      // TODO: animations
    break;
    default:
      // do nothing
    break;
  }

  delay(500);
}

void setLEDColour(byte redVal, byte greenVal, byte blueVal) {
  analogWrite(R_PIN, redVal * R_MULT);
  analogWrite(G_PIN, greenVal * G_MULT);
  analogWrite(B_PIN, blueVal * B_MULT);
}

