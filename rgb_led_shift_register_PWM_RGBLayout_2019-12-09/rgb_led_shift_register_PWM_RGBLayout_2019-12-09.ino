#include <TFT.h>

/*
  RGB LED shift register PWM test
  Ryan Wilson (c) 2017

  based on "74HC595 PWM SPI v1.3" by Joseph Francis
*/

// "https://github.com/PaulStoffregen/TimerOne"
#include <TimerOne.h>

// pin connected to ST_CP of 74HC595
// arduino nano SPI:SS pin
const int LATCH_PIN = 10;
// pin connected to SH_CP of 74HC595
// arduino nano SPI:SCK pin
const int CLOCK_PIN = 13;
// pin connected to DS of 74HC595
// arduino nano SPI:MOSI pin
const int DATA_PIN = 11;

// used for faster latching
const int LATCH_PIN_PORTB = LATCH_PIN - 8;

// number of colours per channel (8x8x8 = 512 colours)
//#define TICKER_PWM_LEVELS 8
//#define TICKER_STEP 8 / TICKER_PWM_LEVELS

// ** best balance of colour increments
// number of colours per channel (16x16x16 = 4096 colours)
#define TICKER_PWM_LEVELS 16
#define TICKER_STEP 16 / TICKER_PWM_LEVELS

// number of colours per channel (32x32x32 = 32768 colours)
//#define TICKER_PWM_LEVELS 32
//#define TICKER_STEP 32 / TICKER_PWM_LEVELS

const byte BLACK[3]       = {                          0,                           0,                           0};
const byte GREY50[3]      = {(TICKER_PWM_LEVELS / 2) - 1, (TICKER_PWM_LEVELS / 2) - 1, (TICKER_PWM_LEVELS / 2) - 1};
const byte WHITE[3]       = {      TICKER_PWM_LEVELS - 1,       TICKER_PWM_LEVELS - 1,       TICKER_PWM_LEVELS - 1};

const byte RED[3]         = {      TICKER_PWM_LEVELS - 1,                           0,                           0};
const byte ORANGE[3]      = {      TICKER_PWM_LEVELS - 1, (TICKER_PWM_LEVELS / 2) - 1,                           0};
const byte YELLOW[3]      = {      TICKER_PWM_LEVELS - 1,       TICKER_PWM_LEVELS - 1,                           0};
const byte CHARTREUSE[3]  = {(TICKER_PWM_LEVELS / 2) - 1,       TICKER_PWM_LEVELS - 1,                           0};
const byte GREEN[3]       = {                          0,       TICKER_PWM_LEVELS - 1,                           0};
const byte SPRINGGREEN[3] = {                          0,       TICKER_PWM_LEVELS - 1, (TICKER_PWM_LEVELS / 2) - 1};
const byte CYAN[3]        = {                          0,       TICKER_PWM_LEVELS - 1,       TICKER_PWM_LEVELS - 1};
const byte AZURE[3]       = {                          0, (TICKER_PWM_LEVELS / 2) - 1,       TICKER_PWM_LEVELS - 1};
const byte BLUE[3]        = {                          0,                           0,       TICKER_PWM_LEVELS - 1};
const byte VIOLET[3]      = {(TICKER_PWM_LEVELS / 2) - 1,                           0,       TICKER_PWM_LEVELS - 1};
const byte MAGENTA[3]     = {      TICKER_PWM_LEVELS - 1,                           0,       TICKER_PWM_LEVELS - 1};
const byte ROSE[3]        = {      TICKER_PWM_LEVELS - 1,                           0, (TICKER_PWM_LEVELS / 2) - 1};

// used in processInterrupt to control the software PWM cycle
int ticker = 0;

// TIMER_DELAY controls how fast this runs.
// As long as you don't see flicker ... a higher number (slower) is better to assure
// the loop has room to do stuff
#define TIMER_DELAY 500

#define SR_COUNT 1
#define SR_COUNT_LEVELS SR_COUNT * 8

// holds a 0 to 255 PWM value used to set the value of each SR pin
byte srPins[SR_COUNT_LEVELS];

// direct port access latching
void latchOn() {
  bitSet(PORTB, LATCH_PIN_PORTB);
}
void latchOff() {
  bitClear(PORTB, LATCH_PIN_PORTB);
}

// this process is run by the timer and does the PWM control
void processInterrupt() {
  // create a temporary array of bytes to hold shift register values in
  byte srVals[SR_COUNT];
  // increment our ticker
  ticker++;
  // if our ticker level cycles, restart
  if (ticker > TICKER_PWM_LEVELS) {
    ticker = 0;
  }

  // get ticker as a 0 to 255 value, so we can always use the same data regardless of actual PWM levels
  int myPos = ticker * TICKER_STEP;

  // loop through all bits in the shift register (8 pin for the 595's)
  for (int i = 0 ; i < 8; i++) {
    int myLev = 0;
    // loop through all shift registers and set the bit on or off
    for (int iSR = 0 ; iSR < SR_COUNT; iSR++) {
      // start with the bit off
      myLev = 0;
      // if the value in the sr pin related to this SR/Byte is over the current pwm value
      // then turn the bit on
      if (srPins[i + (iSR * 8)] > myPos) {
        myLev = 1;
      }
      // write the bit into the SR byte array
      bitWrite(srVals[iSR], i, myLev);
    }
  }

  // run through all the temporary shift register values and send them (last one first)
  // latching in the process
  latchOff();
  for (int iSR = SR_COUNT - 1; iSR >= 0; iSR--) {
    spi_transfer(srVals[iSR]);
  }
  latchOn();
}

// used to setup SPI based on current pin setup
void setupSPI() {
  byte clr;
  SPCR |= ( (1 << SPE) | (1 << MSTR) ); // enable SPI as master
  SPCR &= ~( (1 << SPR1) | (1 << SPR0) ); // clear prescaler bits
  clr = SPSR; // clear SPI status reg
  clr = SPDR; // clear SPI data reg
  SPSR |= (1 << SPI2X); // set prescaler bits
  delay(10);
}

// the really fast SPI version of shiftOut
void spi_transfer(byte data) {
  SPDR = data;
  loop_until_bit_is_set(SPSR, SPIF);
}

// set specific RGB LED to preset colour
void ledToColour(byte ledLocation, const byte colour[3]) {
  ledToColour(ledLocation, colour[0], colour[1], colour[2]);
}

// set specific RGB LED by colour component
void ledToColour(byte ledLocation, byte redVal, byte greenVal, byte blueVal) {
  srPins[(ledLocation * 3)] = redVal;
  srPins[(ledLocation * 3) + 1] = greenVal;
  srPins[(ledLocation * 3) + 2] = blueVal;
}

// set all RGB LEDs to preset colour
void allToColour(const byte colour[3]) {
  allToColour(colour[0], colour[1], colour[2]);
}

// set all RGB LEDs by colour component
void allToColour(byte redVal, byte greenVal, byte blueVal) {
  for (int i = 0; i < (SR_COUNT_LEVELS); i += 3) {
//    Serial.print(redVal);
//    Serial.print("\n");
//    Serial.print(greenVal);
//    Serial.print("\n");
//    Serial.print(blueVal);
//    Serial.print("\n");
    srPins[i] = redVal;
    if ((i + 1) < SR_COUNT_LEVELS ) srPins[i + 1] = greenVal;
    if ((i + 2) < SR_COUNT_LEVELS ) srPins[i + 2] = blueVal;
  }

  //  for (int i = 0; i < (SR_COUNT); i++) {
  //    // set first SR set
  //    for (int j = 0; j < 8; j += 3) {
  //      srPins[j] = redVal;
  //      srPins[j + 1] = greenVal;
  //      srPins[j + 2] = blueVal;
  //    }
  //
  //    //    // set first half of second SR set (we don't use 2nd nibble)
  //    //    for(int j = 0; j < 4; j++) {
  //    //      srPins[j + 24] = redVal;
  //    //      srPins[j + 32] = greenVal;
  //    //      srPins[j + 40] = blueVal;
  //    //    }
  //  }
}

// setup //////////////////////////////////////////////////////////////////////
void setup() {
  // for debug
  Serial.begin(9600);
  Serial.print("setup\n");

  // set pins to output so you can control the shift register
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);

  // clear data
  digitalWrite(LATCH_PIN, LOW);
  digitalWrite(CLOCK_PIN, LOW);
  digitalWrite(DATA_PIN, LOW);

  // setup to run SPI
  setupSPI();

  // activate the PWM timer
  Timer1.initialize(TIMER_DELAY);
  Timer1.attachInterrupt(processInterrupt);
}

// loop ///////////////////////////////////////////////////////////////////////
void loop() {
  Serial.print("-= loop start =-\n");
  allToColour(BLACK);
  // flash RGB to show start of pattern
  for (int i = 0; i < 3; i++) {
    allToColour(RED);
    delay(100);
    allToColour(BLUE);
    delay(100);
    allToColour(GREEN);
    delay(100);
    allToColour(BLACK);
    delay(1000);
  }
  delay(1000);
  
//  allToColour(RED);
//  delay(500);
//  allToColour(ORANGE);
//  delay(500);
//  allToColour(YELLOW);
//  delay(500);
//  allToColour(CHARTREUSE);
//  delay(500);
//  allToColour(GREEN);
//  delay(500);
//  allToColour(SPRINGGREEN);
//  delay(500);
//  allToColour(CYAN);
//  delay(500);
//  allToColour(AZURE);
//  delay(500);
//  allToColour(BLUE);
//  delay(500);
//  allToColour(VIOLET);
//  delay(500);
//  allToColour(MAGENTA);
//  delay(500);
//  allToColour(ROSE);
//  delay(500);
//  allToColour(GREY50);
//  delay(500);
//  allToColour(WHITE);
//  delay(2000);
//
//  // colour transitions (from black)
//  int stepCount = 16;
//  int stepDelay = 50;
//  fromColourToColour(BLACK, RED, stepCount, stepDelay);
//  fromColourToColour(BLACK, ORANGE, stepCount, stepDelay);
//  fromColourToColour(BLACK, YELLOW, stepCount, stepDelay);
//  fromColourToColour(BLACK, CHARTREUSE, stepCount, stepDelay);
//  fromColourToColour(BLACK, GREEN, stepCount, stepDelay);
//  fromColourToColour(BLACK, SPRINGGREEN, stepCount, stepDelay);
//  fromColourToColour(BLACK, CYAN, stepCount, stepDelay);
//  fromColourToColour(BLACK, AZURE, stepCount, stepDelay);
//  fromColourToColour(BLACK, BLUE, stepCount, stepDelay);
//  fromColourToColour(BLACK, VIOLET, stepCount, stepDelay);
//  fromColourToColour(BLACK, MAGENTA, stepCount, stepDelay);
//  fromColourToColour(BLACK, ROSE, stepCount, stepDelay);
//
//  delay(1000);
//
//  fromColourToColour(RED, ORANGE, stepCount, stepDelay);
//  fromColourToColour(ORANGE, YELLOW, stepCount, stepDelay);
//  fromColourToColour(YELLOW, CHARTREUSE, stepCount, stepDelay);
//  fromColourToColour(CHARTREUSE, GREEN, stepCount, stepDelay);
//  fromColourToColour(GREEN, SPRINGGREEN, stepCount, stepDelay);
//  fromColourToColour(SPRINGGREEN, CYAN, stepCount, stepDelay);
//  fromColourToColour(CYAN, AZURE, stepCount, stepDelay);
//  fromColourToColour(AZURE, BLUE, stepCount, stepDelay);
//  fromColourToColour(BLUE, VIOLET, stepCount, stepDelay);
//  fromColourToColour(VIOLET, MAGENTA, stepCount, stepDelay);
//  fromColourToColour(MAGENTA, ROSE, stepCount, stepDelay);
//  fromColourToColour(ROSE, RED, stepCount, stepDelay);

  //  delay(1000);
  //
  //  ledToColour(0, RED);
  //  delay(100);
  //  ledToColour(1, ORANGE);
  //  delay(100);
  //  ledToColour(2, YELLOW);
  //  delay(100);
  //  ledToColour(3, CHARTREUSE);
  //  delay(100);
  //  ledToColour(4, GREEN);
  //  delay(100);
  //  ledToColour(5, SPRINGGREEN);
  //  delay(100);
  //  ledToColour(6, CYAN);
  //  delay(100);
  //  ledToColour(7, AZURE);
  //  delay(100);
  //  ledToColour(8, BLUE);
  //  delay(100);
  //  ledToColour(9, VIOLET);
  //  delay(100);
  //  ledToColour(10, MAGENTA);
  //  delay(100);
  //  ledToColour(11, ROSE);

  delay(1000);
  
  cycleRandomColours();
  Serial.print("-= loop end =-\n");
}

void fromColourToColour(const byte fromColour[3], const byte toColour[3], int stepCount, int stepDelay) {
  fromColourToColour(fromColour[0], fromColour[1], fromColour[2], toColour[0], toColour[1], toColour[2], stepCount, stepDelay);
}

void fromColourToColour(byte fromRedVal, byte fromGreenVal, byte fromBlueVal, byte toRedVal, byte toGreenVal, byte toBlueVal, int stepCount, int stepDelay) {
  // set from colour
  allToColour(fromRedVal, fromGreenVal, fromBlueVal);

  // incrementally set colour
  for (int i = 0; i < stepCount; i++) {
    allToColour(
      (((toRedVal - fromRedVal) / (float)stepCount) * (float)(i + 1) + fromRedVal),
      (((toGreenVal - fromGreenVal) / (float)stepCount) * (float)(i + 1) + fromGreenVal),
      (((toBlueVal - fromBlueVal) / (float)stepCount) * (float)(i + 1) + fromBlueVal)
    );
    delay(stepDelay);
  }
}

void cycleRandomColours() {
  for (int i = 0; i < 20; i++) {
    for (int j = 0; j < 2; j++) {
      ledToColour(j, random(TICKER_PWM_LEVELS), random(TICKER_PWM_LEVELS), random(TICKER_PWM_LEVELS));
    }
    delay(100);
  }
}
