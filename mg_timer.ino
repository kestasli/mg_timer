#include <TFT_eSPI.h>  // Hardware-specific library
#include <SPI.h>
#include "WiFi.h"
#include <WiFiManager.h>
#include <EEPROM.h>
#include "fonts/FreeSansBold56pt7b.h"
//#include "fonts/FreeMonoBold24pt7b.h"
#include <climits>

#define SCREEN_W 320
#define SCREEN_H 172
const int TIMER_PIN = 0;
const int RESET_PIN = 14;
const int COUNTER_DELAY = 3000000;
//ULONG_MAX

unsigned long intervalStart = 0;
unsigned long intervalEnd = 0;

unsigned long timePoint = 0;
unsigned long timePointPrev = 0;


unsigned int timerState = false;
bool intervalState = false;

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite counter = TFT_eSprite(&tft);

void setup() {
  // put your setup code here, to run once:

  tft.begin();
  tft.setRotation(1);
  counter.createSprite(SCREEN_W, SCREEN_H);
  counter.setFreeFont(&FreeSansBold56pt7b);
  //counter.setFreeFont(&FreeMonoBold24pt7b);
  counter.setTextColor(TFT_WHITE);
  counter.setTextDatum(CC_DATUM);

  pinMode(TIMER_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(TIMER_PIN), relayOn, FALLING);
}

void loop() {

  unsigned int intervalEnd_prev;
  unsigned int intervalStart_prev;

  //counter.fillSprite(TFT_BLACK);

  //if ((intervalEnd - intervalStart) < COUNTER_DELAY) timerState = true;
  //if ((intervalStart - intervalEnd) < COUNTER_DELAY) timerState = false;

  switch (timerState) {
    case 0:
      counter.setTextColor(TFT_WHITE);
      counter.drawFloat((float)(micros() - intervalStart) / 1000000, 2, SCREEN_W / 2, SCREEN_H / 2);
      break;
    case 1:
      counter.setTextColor(TFT_GREEN);
      counter.drawFloat((float)(intervalEnd - intervalStart) / 1000000, 2, SCREEN_W / 2, SCREEN_H / 2);
      break;
    case 2:
      // statements
      break;
  }
  /*
  if (timerState) {
    counter.setTextColor(TFT_WHITE);
    counter.drawFloat((float)(micros() - intervalStart) / 1000000, 2, SCREEN_W / 2, SCREEN_H / 2);
  } else {
    counter.setTextColor(TFT_GREEN);
    counter.drawFloat((float)(intervalEnd - intervalStart) / 1000000, 2, SCREEN_W / 2, SCREEN_H / 2);
  }
*/

  counter.pushSprite(0, 0);
  delay(10);
}

void relayOn() {

  timePoint = micros();
  if (((timePoint - timePointPrev) > COUNTER_DELAY) || timePoint == 0) {
    timerState = changeTimerState();
    if (timerState) intervalStart = timePoint;
    if (!timerState) intervalEnd = timePoint;
    timePointPrev = timePoint;
  }
}

unsigned int changeTimerState() {
  static unsigned int timerState = 0;
  if (timerState > 2) {
    timerState = 0;
  }
  return timerState++;
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