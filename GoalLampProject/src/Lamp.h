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


typedef  NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod>  NeoPixelStrip;


//ESP8266 Output Pin
const int boardLed = D7;

#define NUMBER_OF_STRANDS 8
#define PIXEL_PER 7
#define PIXEL_PIN_1 5
#define PIXEL_PIN_0 15
#define PIXEL_COUNT 57
#define TOP_PIXEL 7
#define PIXEL_TYPE (NEO_GRB + NEO_KHZ800)
#define PIXEL_COUNT_BOTTOM 5



const int matrix[8][7] = {
    {56,55,54,53,52,51,50},
    {43,44,45,46,47,48,49},
    {42,41,40,39,38,37,36},
    {29,30,31,32,33,34,35},
    {28,27,26,25,24,23,22},
    {15,16,17,18,19,20,21},
    {14,13,12,11,10,9,8},
    {0,1,2,3,4,5,6}  
};


class Team {
  public:
    int id;
    String name;
    int score;
    String toString(){
      return "TeamName- " + name + "Score " + String(score);
    }
};

class NhlGame {
  public:
    char path[45];
    time_t time;
    String detailedState;
    Team home;
    Team away;
    String toString(){
      return "\nGameState- " \
      + detailedState + "\r\nHomeTeam- " + home.toString() + "\r\nAwayTeam- " + away.toString();
    }
};

int CheckNHLScore();
void InitOta();
void AlarmCheckGame();
void BuildNHLPath();
void AlarmGameUpdate();
void testTrig();


#include "LookupTables.h"
#include "Draw.h"
#include "LampTime.h"

#undef _TIME_T_
#define _TIME_T_ long unsigned int

#endif 
