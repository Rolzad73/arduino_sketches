/*
  RGB LED test
  Ryan Wilson (c) 2017
 */

// Init the Pins used for PWM
const int R_PIN = 9;
const int G_PIN = 10;
const int B_PIN = 11;

const int R_IDX = 0;
const int G_IDX = 1;
const int B_IDX = 2;

const float R_MULT = 0.15;
const float G_MULT = 0.15;
const float B_MULT = 0.15;

const boolean SHOW_DEFINED_COLOURS = false;
const boolean SHOW_RANDOM_COLOURS = false;
const boolean SHOW_COLOUR_WHEEL = false;

// defined colours
const byte colours[][3] = {
  { 0, 0, 0 },        // black/off
  { 25, 25, 25 },     // 10% white
  { 63, 63, 63 },     // 25% white
  { 127, 127, 127 },  // 50% white
  { 191, 191, 191 },  // 75% white
  { 255, 255, 255 },  // 100% white

  { 25, 0, 0 },       // 10% red
  { 63, 0, 0 },       // 25% red
  { 127, 0, 0 },      // 50% red
  { 191, 0, 0 },      // 75% red
  { 255, 0, 0 },      // 100% red

  { 25, 25, 0 },      // 10% yellow
  { 63, 63, 0 },      // 25% yellow
  { 127, 127, 0 },    // 50% yellow
  { 191, 191, 0 },    // 75% yellow
  { 255, 255, 0 },    // 100% yellow

  { 0, 25, 0 },       // 10% green
  { 0, 63, 0 },       // 25% green
  { 0, 127, 0 },      // 50% green
  { 0, 191, 0 },      // 75% green
  { 0, 255, 0 },      // 100% green

  { 0, 25, 25 },      // 10% cyan
  { 0, 63, 63 },      // 25% cyan
  { 0, 127, 127 },    // 50% cyan
  { 0, 191, 191 },    // 75% cyan
  { 0, 255, 255 },    // 100% cyan

  { 0, 0, 25 },       // 10% blue
  { 0, 0, 63 },       // 25% blue
  { 0, 0, 127 },      // 50% blue
  { 0, 0, 191 },      // 75% blue
  { 0, 0, 255 },      // 100% blue

  { 25, 0, 25 },      // 10% white
  { 63, 0, 63 },      // 25% magenta
  { 127, 0, 127 },    // 50% magenta
  { 191, 0, 191 },    // 75% magenta
  { 255, 0, 255 }     // 100% magenta
};

// colour wheel component delta list
const int colour_wheel_delta[][3] = {
  // start at colour 255,0,0
  { 0, 1, 0 },  // increase green
  {-1, 0, 0 },  // reduce red
  { 0, 0, 1 },  // increase blue
  { 0,-1, 0 },  // reduce green
  { 1, 0, 0 },  // increase red
  { 0, 0,-1 },  // reduce blue
};

void setup() {
  pinMode(R_PIN, OUTPUT);
  pinMode(G_PIN, OUTPUT);
  pinMode(B_PIN, OUTPUT);
  
  randomSeed(analogRead(0));
}

void loop() {
  // go through list of defined colours
  if (SHOW_DEFINED_COLOURS) {
    int listSize = sizeof (colours) / sizeof (colours[0]);
    for (int i = 0; i < listSize; i++) {
      analogWrite(R_PIN, colours[i][R_IDX] * R_MULT);
      analogWrite(G_PIN, colours[i][G_IDX] * G_MULT);
      analogWrite(B_PIN, colours[i][B_IDX] * B_MULT);
      delay(500);
    }
  }

  // go through list of random colours
  if (SHOW_RANDOM_COLOURS) {
    for (int i = 0; i < 40; i++) {
      analogWrite(R_PIN, random(255) * R_MULT);
      analogWrite(G_PIN, random(255) * G_MULT);
      analogWrite(B_PIN, random(255) * B_MULT);
      delay(100);
    }
  }

  // colour wheel cycle
  if (SHOW_COLOUR_WHEEL) {
    int deltaSize = sizeof (colour_wheel_delta) / sizeof (colour_wheel_delta[0]);
    int R_VAL = 31;
    int G_VAL = 0;
    int B_VAL = 0;

    for (int i = 0; i < deltaSize; i++) {
      for (int j = 0; j < 31; j++) {
        analogWrite(R_PIN, R_VAL * 8 * R_MULT);
        analogWrite(G_PIN, G_VAL * 8 * G_MULT);
        analogWrite(B_PIN, B_VAL * 8 * B_MULT);
        delay(50);
        R_VAL += colour_wheel_delta[i][R_IDX];
        G_VAL += colour_wheel_delta[i][G_IDX];
        B_VAL += colour_wheel_delta[i][B_IDX];
      }
    }
  }
}

