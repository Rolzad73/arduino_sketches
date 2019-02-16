/*
  GPIO input to RGB LED shift register PWM
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

// this example uses 6 full shift registers, but the hardware tested on
// only drives 12 RGB LEDs and thereby only uses half of the 2nd set of 3
// 1st = first 8 RED
// 2nd = first 8 GREEN
// 3rd = first 8 BLUE
// 4th = next 4 RED
// 5th = next 4 GREEN
// 6th = next 4 BLUE
#define SR_COUNT 6
#define SR_COUNT_LEVELS SR_COUNT * 8

// holds a 0 to 255 PWM value used to set the value of each SR pin
byte srPins[SR_COUNT_LEVELS];

// direct port access latching
void latchOn() { bitSet(PORTB, LATCH_PIN_PORTB); }
void latchOff() { bitClear(PORTB, LATCH_PIN_PORTB); }

// this process is run by the timer and does the PWM control
void processInterrupt() {
  // create a temporary array of bytes to hold shift register values in
  byte srVals[SR_COUNT];
  // increment our ticker
  ticker++;
  // if our ticker level cycles, restart
  if(ticker > TICKER_PWM_LEVELS) { ticker = 0; }

  // get ticker as a 0 to 255 value, so we can always use the same data regardless of actual PWM levels
  int myPos = ticker * TICKER_STEP;

  // loop through all bits in the shift register (8 pin for the 595's)
  for(int i = 0 ; i < 8; i++) {
    int myLev = 0;
    // loop through all shift registers and set the bit on or off
    for(int iSR = 0 ; iSR < SR_COUNT; iSR++) {
      // start with the bit off
      myLev = 0;
      // if the value in the sr pin related to this SR/Byte is over the current pwm value
      // then turn the bit on
      if (srPins[i + (iSR * 8)] > myPos) { myLev = 1; }
      // write the bit into the SR byte array
      bitWrite(srVals[iSR], i, myLev);
    }
  }

  // run through all the temporary shift register values and send them (last one first)
  // latching in the process
  latchOff();
  for(int iSR = SR_COUNT - 1; iSR >= 0; iSR--) {
    spi_transfer(srVals[iSR]);
  }
  latchOn();
}

// used to setup SPI based on current pin setup
void setupSPI() {
  byte clr;
  SPCR |= ( (1<<SPE) | (1<<MSTR) ); // enable SPI as master
  SPCR &= ~( (1<<SPR1) | (1<<SPR0) ); // clear prescaler bits
  clr=SPSR; // clear SPI status reg
  clr=SPDR; // clear SPI data reg
  SPSR |= (1<<SPI2X); // set prescaler bits
  delay(10); 
}

// the really fast SPI version of shiftOut
void spi_transfer(byte data) {
  SPDR = data;
  loop_until_bit_is_set(SPSR, SPIF); 
}

// input pins
const int INPUT_1 = 8;
const int INPUT_2 = 7;
const int INPUT_3 = 6;
const int INPUT_4 = 5;

int val_1 = 0;
int val_2 = 0;
int val_3 = 0;
int val_4 = 0;

// set specific RGB LED to preset colour
void ledToColour(byte ledLocation, const byte colour[3]) {
  ledToColour(ledLocation, colour[0], colour[1], colour[2]);
}

// set specific RGB LED by colour component
void ledToColour(byte ledLocation, byte redVal, byte greenVal, byte blueVal) {
  if (ledLocation < 8) {
    // set first SR set
    srPins[ledLocation] = redVal;
    srPins[ledLocation + 8] = greenVal;
    srPins[ledLocation + 16] = blueVal;
  } else {
    // set first half of second SR set (we don't use 2nd nibble)
    srPins[ledLocation + 24] = redVal;
    srPins[ledLocation + 32] = greenVal;
    srPins[ledLocation + 40] = blueVal;
  }
}

// set all RGB LEDs to preset colour
void allToColour(const byte colour[3]) {
  allToColour(colour[0], colour[1], colour[2]);
}

// set all RGB LEDs by colour component
void allToColour(byte redVal, byte greenVal, byte blueVal) {
  for(int i = 0; i < (SR_COUNT); i++) {
    // set first SR set
    for(int j = 0; j < 8; j++) {
      srPins[j] = redVal;
      srPins[j + 8] = greenVal;
      srPins[j + 16] = blueVal;
    }
    
    // set first half of second SR set (we don't use 2nd nibble)
    for(int j = 0; j < 4; j++) {
      srPins[j + 24] = redVal;
      srPins[j + 32] = greenVal;
      srPins[j + 40] = blueVal;
    }     
  }
}

void fromColourToColour(const byte fromColour[3], const byte toColour[3], int stepCount, int stepDelay) {
  fromColourToColour(fromColour[0], fromColour[1], fromColour[2], toColour[0], toColour[1], toColour[2], stepCount, stepDelay);
}

void fromColourToColour(byte fromRedVal, byte fromGreenVal, byte fromBlueVal, byte toRedVal, byte toGreenVal, byte toBlueVal, int stepCount, int stepDelay) {
  // set from colour
  allToColour(fromRedVal, fromGreenVal, fromBlueVal);

  // incrementally set colour
  for(int i = 0; i < stepCount; i++) {
    allToColour(
      (((toRedVal - fromRedVal) / (float)stepCount) * (float)(i + 1) + fromRedVal),
      (((toGreenVal - fromGreenVal) / (float)stepCount) * (float)(i + 1) + fromGreenVal), 
      (((toBlueVal - fromBlueVal) / (float)stepCount) * (float)(i + 1) + fromBlueVal)
    );
    delay(stepDelay);
  }
}

void cycleRandomColours() {
  for(int i = 0; i < 20; i++) {
    for(int j = 0; j < 12; j++) {
      ledToColour(j, random(TICKER_PWM_LEVELS), random(TICKER_PWM_LEVELS), random(TICKER_PWM_LEVELS));
    }
    delay(100);
  }
}

// current DOD status
int currentStatus = -1;
int nextStatus = 0;
// States
// 0 no data           // black
// 1 DOD Queue = 0     // blue
// 2 DOD Queue = 1-5   // white
// 3 DOD Queue = 6-10  // yellow
// 4 DOD Queue = 11-15 // orange
// 5 DOD Queue = 15-20 // red
// 6 DOD Queue = 20+   // pink
// 7 DOD Queue contains BLOCKER // pulsing red to black
// 8 animation 1 (random)
// 9 animation 2 (rainbow cycle)
// 10 animation 3 (fire animation 1 particles up?)
// 11 animation 4 (spin animation)

// setup //////////////////////////////////////////////////////////////////////
void setup() {
  // for debug
  Serial.begin(9600);

  // set pins to output so you can control the shift register
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);

  pinMode(INPUT_1, INPUT);
  pinMode(INPUT_2, INPUT);
  pinMode(INPUT_3, INPUT);
  pinMode(INPUT_4, INPUT);

  // clear data
  digitalWrite(LATCH_PIN, LOW);
  digitalWrite(CLOCK_PIN, LOW);
  digitalWrite(DATA_PIN, LOW);

  // setup to run SPI
  setupSPI();

  // activate the PWM timer
  Timer1.initialize(TIMER_DELAY);
  Timer1.attachInterrupt(processInterrupt);

  // prepare random seed
  randomSeed(analogRead(0));

  // start all black
  allToColour(BLACK);
}

// loop ///////////////////////////////////////////////////////////////////////
void loop() {
  // read input
  val_1 = digitalRead(INPUT_1);
  val_2 = digitalRead(INPUT_2);
  val_3 = digitalRead(INPUT_3);
  val_4 = digitalRead(INPUT_4);
  // combine binary digits to integer
  nextStatus = val_1 + (val_2*2) + (val_3*4) + (val_4*8);
  Serial.println(nextStatus);

  // only change if status changes
  if (nextStatus != currentStatus) {
    currentStatus = nextStatus;

    // set lEDs
    switch (nextStatus) {
      case 0: // no data
        allToColour(BLACK);
      break;
      case 1: // DOD Queue = 0
        allToColour(BLUE);
      break;
      case 2: // DOD Queue = 1-5
        allToColour(WHITE);
      break;
      case 3: // DOD Queue = 6-10
        allToColour(YELLOW);
      break;
      case 4: // DOD Queue = 11-15
        allToColour(ORANGE);
      break;
      case 5: // DOD Queue = 15-20
        allToColour(RED);
      break;
      case 6: // DOD Queue = 20+
        allToColour(ROSE);
      break;
      case 7: // DOD Queue contains BLOCKER
        allToColour(GREEN);
      break;
      case 8:
        // TODO: animations
      break;
      default:
        // do nothing
      break;
    }

    currentStatus = nextStatus;
  }

  delay(1000);
}

