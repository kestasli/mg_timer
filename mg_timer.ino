#include "LedController.hpp"

/*
 The master has three significant pins: SCLK(Serial Clock), MOSI(Master Out Slave In), SS(Slave Select).
 The slave also has tree needed pins (and two for power(GND and VCC)): CLK(Clock), DIN(Data In), CS(Ship Select).
 SCLK has to be connected to CLK and MOSI to DIN.
 These signals can be shared by multiple SPI slaves even if they have nothing to do with each other.
 SS has to be connected to CS and each Slave has its own SS pin(which can be any free pin).
 */

//the pin where the chip select is connected to
#define CS 1 //esp32
//#define DIN 11
//#define CS 10 //Arduino Pro Mini
//#define CLK 13


//pin 12 is connected to the DataIn 
//pin 11 is connected to the CLK 
//pin 10 is connected to LOAD 

//CS0  10
//SCLK 12
//MISO 13
//MOSI 11

#define Segments 5

#define RELAY_PIN 2
#define COUNTER_DELAY 2000000

//ULONG_MAX

LedController<Segments, 1> lc = LedController<Segments, 1>();

unsigned long intervalStart = 0;
unsigned long intervalEnd = 1;
unsigned long timePoint = 0;
unsigned long timePointPrev = 0;
bool timerState = false;  //true- time counting, false- stopped


ByteBlock digits[10] = {
  { 0b00111000,
    0b01000100,
    0b01000100,
    0b01000100,
    0b01000100,
    0b01000100,
    0b01000100,
    0b00111000 },
  { 0b00010000,
    0b00110000,
    0b00010000,
    0b00010000,
    0b00010000,
    0b00010000,
    0b00010000,
    0b00111000 },
  { 0b00111000,
    0b01000100,
    0b00000100,
    0b00000100,
    0b00001000,
    0b00010000,
    0b00100000,
    0b01111100 },
  { 0b00111000,
    0b01000100,
    0b00000100,
    0b00011000,
    0b00000100,
    0b00000100,
    0b01000100,
    0b00111000 },
  { 0b00000100,
    0b00001100,
    0b00010100,
    0b00100100,
    0b01000100,
    0b01111100,
    0b00000100,
    0b00000100 },
  { 0b01111100,
    0b01000000,
    0b01000000,
    0b01111000,
    0b00000100,
    0b00000100,
    0b01000100,
    0b00111000 },
  { 0b00111000,
    0b01000100,
    0b01000000,
    0b01111000,
    0b01000100,
    0b01000100,
    0b01000100,
    0b00111000 },
  { 0b01111100,
    0b00000100,
    0b00000100,
    0b00001000,
    0b00010000,
    0b00100000,
    0b00100000,
    0b00100000 },
  { 0b00111000,
    0b01000100,
    0b01000100,
    0b00111000,
    0b01000100,
    0b01000100,
    0b01000100,
    0b00111000 },
  { 0b00111000,
    0b01000100,
    0b01000100,
    0b01000100,
    0b00111100,
    0b00000100,
    0b01000100,
    0b00111000 }
};

ByteBlock digits_c[10] = {
  { 0b00111000,
    0b01000100,
    0b01000100,
    0b01000100,
    0b01000100,
    0b01000100,
    0b01000101,
    0b00111001 },
  { 0b00010000,
    0b00110000,
    0b00010000,
    0b00010000,
    0b00010000,
    0b00010000,
    0b00010001,
    0b00111001 },
  { 0b00111000,
    0b01000100,
    0b00000100,
    0b00000100,
    0b00001000,
    0b00010000,
    0b00100001,
    0b01111101 },
  { 0b00111000,
    0b01000100,
    0b00000100,
    0b00011000,
    0b00000100,
    0b00000100,
    0b01000101,
    0b00111001 },
  { 0b00000100,
    0b00001100,
    0b00010100,
    0b00100100,
    0b01000100,
    0b01111100,
    0b00000101,
    0b00000101 },
  { 0b01111100,
    0b01000000,
    0b01000000,
    0b01111000,
    0b00000100,
    0b00000100,
    0b01000101,
    0b00111001 },
  { 0b00111000,
    0b01000100,
    0b01000000,
    0b01111000,
    0b01000100,
    0b01000100,
    0b01000101,
    0b00111001 },
  { 0b01111100,
    0b00000100,
    0b00000100,
    0b00001000,
    0b00010000,
    0b00100000,
    0b00100001,
    0b00100001 },
  { 0b00111000,
    0b01000100,
    0b01000100,
    0b00111000,
    0b01000100,
    0b01000100,
    0b01000101,
    0b00111001 },
  { 0b00111000,
    0b01000100,
    0b01000100,
    0b01000100,
    0b00111100,
    0b00000100,
    0b01000101,
    0b00111001 }
};


void setup() {

  controller_configuration<Segments, 1> conf;
  //use the specified CS pin
  conf.SPI_CS = CS;
  //set the transfer speed to the highest stable value
  //conf.spiTransferSpeed = 10000000;
  conf.onlySendOnChange = true;
  //enable hardware spi
  conf.useHardwareSpi = true;
  //init the controller from the configuration
  lc.init(conf);
  //set the brightness
  lc.setIntensity(10);

  for (unsigned int i = 0; i < 10; i++) {
    //void;
    digits[i] = digits[i].rotate180();
    digits_c[i] = digits_c[i].rotate180();
    lc.clearMatrix();
  }

  pinMode(RELAY_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(RELAY_PIN), relayOn, FALLING);

}

void loop() {

  //if (((timePoint - timePointPrev) > COUNTER_DELAY) || timePoint == 0) {
  if ((timePoint - timePointPrev) > COUNTER_DELAY) {

    timerState = !timerState;
    if (timerState) intervalStart = timePoint;
    if (!timerState) intervalEnd = timePoint;
    timePointPrev = timePoint;
  }

  if (timerState) showTime(micros() - intervalStart);
  if (!timerState) showTime(intervalEnd - intervalStart);
  delay(10);
}


void relayOn() {
  //only grab time when relay is ON
  timePoint = micros();
}

void showTime(unsigned long interval) {
  //Max measurement interval is 9min, 59s, 99ms
  //Only update display if interval is less that capacity of display
  if (interval < 600000000) {
    //this will round up to the nearest 100ts of miliseconds
    unsigned long interval_rounded = interval + 5000;
    unsigned long minutes = interval_rounded / 1000000 / 60;
    unsigned long interval_nominutes = interval_rounded - minutes * 1000000 * 60;

    lc.displayOnSegment(0, digits_c[minutes]);
    lc.displayOnSegment(1, digits[interval_nominutes / 10000000 % 10]);
    lc.displayOnSegment(2, digits_c[interval_nominutes / 1000000 % 10]);
    lc.displayOnSegment(3, digits[interval_nominutes / 100000 % 10]);
    lc.displayOnSegment(4, digits[interval_nominutes / 10000 % 10]);
    //display semicolon and comma
    //lc.setLed(0, 1, 7, true);
    //lc.setLed(0, 0, 7, true);
    //lc.setLed(2, 1, 7, true);
    //lc.setLed(2, 0, 7, true);

  } else {
    lc.displayOnSegment(0, digits_c[9]);
    lc.displayOnSegment(1, digits[5]);
    lc.displayOnSegment(2, digits_c[9]);
    lc.displayOnSegment(3, digits[9]);
    lc.displayOnSegment(4, digits[9]);
  }
}