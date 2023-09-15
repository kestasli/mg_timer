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


#define TIMER_PIN 0
#define RESET_PIN = 14
#define COUNTER_DELAY 3000000

#define TIMER_RUN 0
#define TIMER_STOP 1
#define TIMER_RESET 2

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

  changeTimerState();
}

void loop() {

  unsigned int intervalEnd_prev;
  unsigned int intervalStart_prev;
  
  counter.fillSprite(TFT_BLACK);

  //if ((intervalEnd - intervalStart) < COUNTER_DELAY) timerState = true;
  //if ((intervalStart - intervalEnd) < COUNTER_DELAY) timerState = false;

  switch (timerState) {
    case TIMER_RUN:
      counter.setTextColor(TFT_WHITE);
      counter.drawFloat((float)(micros() - intervalStart) / 1000000, 2, SCREEN_W / 2, SCREEN_H / 2);
      break;
    case TIMER_STOP:
      counter.setTextColor(TFT_GREEN);
      counter.drawFloat((float)(intervalEnd - intervalStart) / 1000000, 2, SCREEN_W / 2, SCREEN_H / 2);
      break;
    case TIMER_RESET:
      counter.setTextColor(TFT_BLUE);
      counter.drawFloat(0.0, 2, SCREEN_W / 2, SCREEN_H / 2);
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
  if (((timePoint - timePointPrev) > COUNTER_DELAY) || timePoint == 0 || timerState == 2) {
    timerState = changeTimerState();
    if (timerState == 0) intervalStart = timePoint;
    if (timerState == 1) intervalEnd = timePoint;
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