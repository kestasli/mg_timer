#include <TFT_eSPI.h>  // Hardware-specific library
#include <SPI.h>
//#include "WiFi.h"
//#include <WiFiManager.h>
//#include <EEPROM.h>
#include "fonts/3X5_____52pt7b.h"
#include <climits>

#define SCREEN_W 320
#define SCREEN_H 172

#define TIMER_PIN 0
#define RESET_PIN 14
#define RELAY_PIN 16
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

  tft.begin();
  tft.setRotation(1);
  counter.createSprite(SCREEN_W, SCREEN_H);
  counter.setFreeFont(&f3X5_____52pt7b);
  
  counter.setTextColor(TFT_WHITE);
  counter.setTextDatum(CL_DATUM);

  pinMode(TIMER_PIN, INPUT_PULLUP);
  pinMode(RELAY_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(RELAY_PIN), relayOn, FALLING);
  attachInterrupt(digitalPinToInterrupt(TIMER_PIN), relayOn, FALLING);

  timerState = 2;
}

void loop() {

  counter.fillSprite(TFT_BLACK);

  switch (timerState) {
    case TIMER_RUN:
      counter.setTextColor(TFT_WHITE);
      counter.drawString(formatTime(micros() - intervalStart), 0, SCREEN_H/2);
      break;
    case TIMER_STOP:
      counter.setTextColor(TFT_GREEN);
      counter.drawString(formatTime(intervalEnd - intervalStart), 0, SCREEN_H/2);
      break;
    case TIMER_RESET:
      counter.setTextColor(TFT_WHITE);
      counter.drawString("0'00,00", 0, SCREEN_H/2);
      break;
  }

  counter.pushSprite(0, 0);
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

//parameter is microseconds
char* formatTime(unsigned long interval){
  static char timerDisplay[9];
  char secondsString[6];
  unsigned long intervalMinutes;
  float intervalSeconds;

  intervalMinutes = interval/1000/1000/60;
  intervalSeconds = (interval - intervalMinutes * 60*1000*1000)/1000000.0;
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