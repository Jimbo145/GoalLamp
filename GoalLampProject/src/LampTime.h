#ifndef LAMP_TIME_H
#define LAMP_TIME_H

//#include <Lamp.h>
#include <Arduino.h>
#include <TimeLib.h> 
#include <TimeAlarms.h>
#include <WiFiUdp.h>

const int timeZone = -4;

const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message

void InitTime();
void sendNTPpacket();
time_t getNtpTime();
String addPreceding0(int digits);
time_t TimeStringToTime(String ts, int MinutesOffset);

#endif 
