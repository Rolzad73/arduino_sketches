/*
  RGB LED shift register test
  Ryan Wilson (c) 2017
 */

//Pin connected to ST_CP of 74HC595
int latchPin = 8;
//Pin connected to SH_CP of 74HC595
int clockPin = 12;
////Pin connected to DS of 74HC595
int dataPin = 11;

const byte PRESET_ALL_BLACK[6] =   {B00000000, B00000000, B00000000, B00000000, B00000000, B00000000};
const byte PRESET_ALL_WHITE[6] =   {B11111111, B11110000, B11111111, B11110000, B11111111, B11110000};
const byte PRESET_ALL_RED[6] =     {B11111111, B11110000, B00000000, B00000000, B00000000, B00000000};
const byte PRESET_ALL_YELLOW[6] =  {B11111111, B11110000, B11111111, B11110000, B00000000, B00000000};
const byte PRESET_ALL_GREEN[6] =   {B00000000, B00000000, B11111111, B11110000, B00000000, B00000000};
const byte PRESET_ALL_CYAN[6] =    {B00000000, B00000000, B11111111, B11110000, B11111111, B11110000};
const byte PRESET_ALL_BLUE[6] =    {B00000000, B00000000, B00000000, B00000000, B11111111, B11110000};
const byte PRESET_ALL_MAGENTA[6] = {B11111111, B11110000, B00000000, B00000000, B11111111, B11110000};

const byte PRESET_ALL_RAINBOW[6] =      {B11000111, B00010000, B01110001, B11000000, B00011100, B01110000};
const byte PRESET_ALL_RAINBOWcheck[6] = {B01000111, B00010000, B01010001, B11000000, B00010100, B01110000};

byte random_colours[6];

byte current_colour[6] = {B00000000, B00000000, B00000000, B00000000, B00000000, B00000000};

const boolean SHOW_DEFINED_COLOURS = true;
const boolean SHOW_RANDOM_COLOURS = true;
const boolean SHOW_COLOUR_SLIDE = true;

void setup() {
  //set pins to output so you can control the shift register
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  
  // for debug and input
  Serial.begin(9600);
  
  // set all to off
  set_colour(PRESET_ALL_BLACK);
}

void loop() {
  // preset colours
  if (SHOW_DEFINED_COLOURS) {
    set_colour(PRESET_ALL_BLACK);
    delay(1000);
    set_colour(PRESET_ALL_WHITE);
    delay(1000);
    set_colour(PRESET_ALL_RED);
    delay(1000);
    set_colour(PRESET_ALL_YELLOW);
    delay(1000);
    set_colour(PRESET_ALL_GREEN);
    delay(1000);
    set_colour(PRESET_ALL_CYAN);
    delay(1000);
    set_colour(PRESET_ALL_BLUE);
    delay(1000);
    set_colour(PRESET_ALL_MAGENTA);
    delay(1000);
  }
  
  // random colours
  if (SHOW_RANDOM_COLOURS) {
    for (int i = 0; i < 40; i++) {
      for (int j = 0; j < 6; j++) {
        random_colours[j] = random(255);
      }
      set_colour(random_colours);
      delay(250);
    }
  }
  
  if (SHOW_COLOUR_SLIDE) {
    slide_set_colour(PRESET_ALL_BLACK, true);
    slide_set_colour(PRESET_ALL_WHITE, true);
    slide_set_colour(PRESET_ALL_RED, true);
    slide_set_colour(PRESET_ALL_YELLOW, true);
    slide_set_colour(PRESET_ALL_GREEN, true);
    slide_set_colour(PRESET_ALL_CYAN, true);
    slide_set_colour(PRESET_ALL_BLUE, true);
    slide_set_colour(PRESET_ALL_MAGENTA, true);
    slide_set_colour(PRESET_ALL_RAINBOW, true);
    
    slide_set_colour(PRESET_ALL_BLACK, false);
    slide_set_colour(PRESET_ALL_WHITE, false);
    slide_set_colour(PRESET_ALL_RED, false);
    slide_set_colour(PRESET_ALL_YELLOW, false);
    slide_set_colour(PRESET_ALL_GREEN, false);
    slide_set_colour(PRESET_ALL_CYAN, false);
    slide_set_colour(PRESET_ALL_BLUE, false);
    slide_set_colour(PRESET_ALL_MAGENTA, false);
    slide_set_colour(PRESET_ALL_RAINBOW, false);
  }

  //exit(0);
}

// sets the colour
void set_colour(const byte colour[6]) {
  // blue_2
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, colour[5]);
  digitalWrite(latchPin, HIGH);
  // green_2
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, colour[3]);
  digitalWrite(latchPin, HIGH);
  // red_2
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, colour[1]);
  digitalWrite(latchPin, HIGH);

  // blue_1
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, colour[4]);
  digitalWrite(latchPin, HIGH);
  // green_1
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, colour[2]);
  digitalWrite(latchPin, HIGH);
  // red_1
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, colour[0]);
  digitalWrite(latchPin, HIGH);
}

// slides new colour over current colour. true = right, false  = left
void slide_set_colour(const byte new_colour[6], boolean direction_right) {
  if (direction_right) {
    // first slide in first colour byte with first half of last new colour byte
    for (int i = 0; i < 4; i++) {
      current_colour[1] = current_colour[1] >> 1;
      current_colour[1] |= current_colour[0] << 7;
      current_colour[0] = current_colour[0] >> 1;
      
      current_colour[3] = current_colour[3] >> 1;
      current_colour[3] |= current_colour[2] << 7;
      current_colour[2] = current_colour[2] >> 1;
      
      current_colour[5] = current_colour[5] >> 1;
      current_colour[5] |= current_colour[4] << 7;
      current_colour[4] = current_colour[4] >> 1;

      current_colour[0] |= new_colour[1] << (3 - i);
      current_colour[2] |= new_colour[3] << (3 - i);
      current_colour[4] |= new_colour[5] << (3 - i);
      
      // set the new colour
      set_colour(current_colour);
      delay(100);
    }
    
    // next slide in the 2nd byte colour
    for (int i = 0; i < 8; i++) {
      current_colour[1] = current_colour[1] >> 1;
      current_colour[1] |= current_colour[0] << 7;
      current_colour[1] &= B11110000;
      current_colour[0] = current_colour[0] >> 1;
      
      current_colour[3] = current_colour[3] >> 1;
      current_colour[3] |= current_colour[2] << 7;
      current_colour[3] &= B11110000;
      current_colour[2] = current_colour[2] >> 1;
      
      current_colour[5] = current_colour[5] >> 1;
      current_colour[5] |= current_colour[4] << 7;
      current_colour[5] &= B11110000;
      current_colour[4] = current_colour[4] >> 1;

      current_colour[0] |= new_colour[0] << (7 - i);
      current_colour[2] |= new_colour[2] << (7 - i);
      current_colour[4] |= new_colour[4] << (7 - i);
      
      // set the new colour
      set_colour(current_colour);
      delay(100);
    }
  } else {
    // first slide in 2nd half colour byte with first half of last new colour byte
    for (int i = 0; i < 4; i++) {
      current_colour[0] = current_colour[0] << 1;
      current_colour[0] |= current_colour[1] >> 7;
      current_colour[1] = current_colour[1] << 1;
      
      current_colour[2] = current_colour[2] << 1;
      current_colour[2] |= current_colour[3] >> 7;
      current_colour[3] = current_colour[3] << 1;
      
      current_colour[4] = current_colour[4] << 1;
      current_colour[4] |= current_colour[5] >> 7;
      current_colour[5] = current_colour[5] << 1;

      current_colour[1] |= new_colour[0] >> (3 - i);
      current_colour[3] |= new_colour[2] >> (3 - i);
      current_colour[5] |= new_colour[4] >> (3 - i);
      
      // set the new colour
      set_colour(current_colour);
      delay(100);
    }
    
    // next slide in the 2nd byte colour
    for (int i = 0; i < 8; i++) {
      current_colour[0] = current_colour[0] << 1;
      current_colour[0] |= current_colour[1] >> 7;
      current_colour[1] = current_colour[1] << 1;
      
      current_colour[2] = current_colour[2] << 1;
      current_colour[2] |= current_colour[3] >> 7;
      current_colour[3] = current_colour[3] << 1;
      
      current_colour[4] = current_colour[4] << 1;
      current_colour[4] |= current_colour[5] >> 7;
      current_colour[5] = current_colour[5] << 1;

      if (i < 4) {
        current_colour[1] |= new_colour[1] >> (7 - i);
        current_colour[3] |= new_colour[3] >> (7 - i);
        current_colour[5] |= new_colour[5] >> (7 - i);
      }
      
      // set the new colour
      set_colour(current_colour);
      delay(100);
    }
  }
}

// sets the bytes R, R, G, G, B, B
void set_bytes(byte red_1, byte red_2, byte green_1, byte green_2, byte blue_1, byte blue_2) {
  // blue_2
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, blue_2);
  digitalWrite(latchPin, HIGH);
  // green_2
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, green_2);
  digitalWrite(latchPin, HIGH);
  // red_2
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, red_2);
  digitalWrite(latchPin, HIGH);

  // blue_1
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, blue_1);
  digitalWrite(latchPin, HIGH);
  // green_1
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, green_1);
  digitalWrite(latchPin, HIGH);
  // red_1
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, red_1);
  digitalWrite(latchPin, HIGH);
}

// NOTE: registers work farthest to closest order
void all_red() {
  // 1st byte 00000000
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, 0);
  digitalWrite(latchPin, HIGH);
  // 2nd byte 00000000
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, 0);
  digitalWrite(latchPin, HIGH);
  // 3rd byte 11111111
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, 255);
  digitalWrite(latchPin, HIGH);
  
  // 4th byte 00000000
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, 0);
  digitalWrite(latchPin, HIGH);
  // 5th byte 00000000
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, 0);
  digitalWrite(latchPin, HIGH);
  // 6th byte 11111111
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, 255);
  digitalWrite(latchPin, HIGH);
}
