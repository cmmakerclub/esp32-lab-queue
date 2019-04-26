#ifndef CMMC_RTC_H
#define CMMC_RTC_H

#include <Arduino.h>
#include <CMMC_Module.h>

#include <Wire.h>
#include "RTClib.h"

class CMMC_RTC : public CMMC_Module
{
protected:
  RTC_DS1307 *rtc;
  char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
  char dateTimeCharArrray[100];
  char dateCharArrray[50];
  char timeCharArrray[50];
  uint32_t prev;
  bool _ready = false;
  bool _rtc_locked = false;
  DateTime _now;

public:
  CMMC_RTC();
  void setup();
  void loop();
  void configLoop();
  void configSetup();
  String getDateTimeString();
  String getDateString();
  String getTimeString();
  void adjust(const DateTime &dt);
  uint32_t getCurrentTimestamp();
};

#endif
