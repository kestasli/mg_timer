//#include <TFT_eSPI.h>  // Hardware-specific library
//#include <SPI.h>
//#include "WiFi.h"
//#include <WiFiManager.h>
//#include <EEPROM.h>
//#include "fonts/3X5_____52pt7b.h"
//#include <climits>

#include "LedController.hpp"

//the pin where the chip select is connected to
#define CS 10
#define Segments 5


//#define SCREEN_W 320
//#define SCREEN_H 172

#define RELAY_PIN 2
#define COUNTER_DELAY 3000000

#define TIMER_RUN 0
#define TIMER_STOP 1
#define TIMER_RESET 2

unsigned long counter = 0;
//ULONG_MAX

LedController<Segments, 1> lc;

unsigned long intervalStart = 0;
unsigned long intervalEnd = 0;

unsigned long timePoint = 0;
unsigned long timePointPrev = 0;

unsigned int timerState = false;
bool intervalState = false;

ByteBlock digits[10] = {
  { 0b00000000,
    0b00000000,
    0b00000000,
    0b01111100,
    0b10100010,
    0b10010010,
    0b10001010,
    0b01111100 },
  { 0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000010,
    0b11111110,
    0b01000010,
    0b00000000 },
  { 0b00000000,
    0b00000000,
    0b00000000,
    0b01100010,
    0b10010010,
    0b10001010,
    0b10000110,
    0b01000010 },
  { 0b00000000,
    0b00000000,
    0b00000000,
    0b01101100,
    0b10010010,
    0b10010010,
    0b10010010,
    0b01000100 },
  { 0b00000000,
    0b00000000,
    0b00000000,
    0b11111110,
    0b00010000,
    0b00010000,
    0b00010000,
    0b11110000 },
  { 0b00000000,
    0b00000000,
    0b00000000,
    0b10001100,
    0b10010010,
    0b10010010,
    0b10010010,
    0b11110010 },
  { 0b00000000,
    0b00000000,
    0b00000000,
    0b00001100,
    0b10010010,
    0b10010010,
    0b10010010,
    0b01111100 },
  { 0b00000000,
    0b00000000,
    0b00000000,
    0b11100000,
    0b10010000,
    0b10001110,
    0b10000000,
    0b10000000 },
  { 0b00000000,
    0b00000000,
    0b00000000,
    0b01101100,
    0b10010010,
    0b10010010,
    0b10010010,
    0b01101100 },
  { 0b00000000,
    0b00000000,
    0b00000000,
    0b01111100,
    0b10010010,
    0b10010010,
    0b10010010,
    0b01100100 }
};


void setup() {

  lc = LedController<Segments, 1>(CS);
  pinMode(13, OUTPUT);
  for (unsigned int i = 0; i < 10; i++) {
    //void;
    digits[i] = digits[i].rotate180();
    lc.clearMatrix();
    lc.setIntensity(15);
  }

  //pinMode(TIMER_PIN, INPUT_PULLUP);
  pinMode(RELAY_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(RELAY_PIN), relayOn, FALLING);
  //attachInterrupt(digitalPinToInterrupt(TIMER_PIN), relayOn, FALLING);

  timerState = 2;
}

void loop() {
  //showTime(counter);
  
  switch (timerState) {
    case TIMER_RUN:
      showTime(micros() - intervalStart);
      //counter.setTextColor(TFT_WHITE);
      //counter.drawString(formatTime(micros() - intervalStart), 0, SCREEN_H / 2);
      break;
    case TIMER_STOP:
      showTime(intervalEnd - intervalStart);
      //counter.setTextColor(TFT_GREEN);
      //counter.drawString(formatTime(intervalEnd - intervalStart), 0, SCREEN_H / 2);
      break;
    case TIMER_RESET:
      showTime(0);
      //counter.setTextColor(TFT_WHITE);
      //counter.drawString("0'00,00", 0, SCREEN_H / 2);
      break;
  }
  
  //counter = counter + 10000;
  delay(10);
}

void relayOn() {
  timePoint = micros();
  if (((timePoint - timePointPrev) > COUNTER_DELAY) || timePoint == 0) {
    timerState = changeTimerState();
    if (timerState == 0) intervalStart = timePoint;
    if (timerState == 1) intervalEnd = timePoint;
    timePointPrev = timePoint;
  }
}

unsigned int changeTimerState() {
  static unsigned int timerState = 0;
  if (timerState > 1) {
    timerState = 0;
  }
  return timerState++;
}

void showTime(unsigned long interval) {
  unsigned long min = interval / 1000000 / 60;
  unsigned long minRemainder = interval - min * 1000000 * 60;
  unsigned long sec = minRemainder / 1000000;
  unsigned long secReminder = minRemainder - sec * 1000000 + 5000;
  unsigned long ms = secReminder / 10000;

  lc.displayOnSegment(0, digits[min]);

  lc.displayOnSegment(1, digits[sec / 10]);
  lc.displayOnSegment(2, digits[sec - 10 * (sec / 10)]);

  lc.displayOnSegment(3, digits[ms / 10]);
  lc.displayOnSegment(4, digits[ms - 10 * (ms / 10)]);

  lc.setLed(0, 6, 5, true);
  lc.setLed(0, 6, 2, true);
  lc.setLed(2, 6, 6, true);
  lc.setLed(2, 6, 7, true);
}




//parameter is microseconds
char* formatTime(unsigned long interval) {
  static char timerDisplay[9];
  char secondsString[6];
  unsigned long intervalMinutes;
  float intervalSeconds;

  intervalMinutes = interval / 1000 / 1000 / 60;
  intervalSeconds = (interval - intervalMinutes * 60 * 1000 * 1000) / 1000000.0;
  dtostrf(intervalSeconds, 4, 2, secondsString);
  if (intervalSeconds < 10) {
    sprintf(timerDisplay, "%u'0%s", intervalMinutes, secondsString);
  } else {
    sprintf(timerDisplay, "%u'%s", intervalMinutes, secondsString);
  }
  return timerDisplay;
}




/*
unsigned long getInterval(unsigned long start, unsigned long end) {
  unsigned long diff = start - end;
  if (start <= end) {
    return abs(diff);
  } else {
    return abs(diff - ULONG_MAX);
  }
}
*/