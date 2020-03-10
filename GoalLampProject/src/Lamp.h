#ifndef LAMP_H
#define LAMP_H

#include <NeoPixelBus.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include <TimeLib.h> 
#include <TimeAlarms.h>

#include <ArduinoJson.h>

#include <math.h>
#include <Arduino.h>

#include "OnlineLogger.h"
#include "Constants.h"
#include "Draw.h"
#include "LampTime.h"
#include "Types.h"
#include "Private.h"

int CheckNHLScore();
void InitOta();
void AlarmCheckGame();
void BuildNHLPath();
void AlarmGameUpdate();
void testTrig();

#endif 
