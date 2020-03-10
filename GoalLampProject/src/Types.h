#ifndef TYPES_H
#define TYPES_H

typedef  NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod>  NeoPixelStrip;

class Team {
  public:
    int id;
    String name;
    int score;
    String toString(){
      return "TeamName- " + name + " Score " + String(score) + " id:" + id;
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
      return "\n GameState- " \
      + detailedState + "\r\n HomeTeam- " + home.toString() + "\r\n AwayTeam- " + away.toString();
    }
};

#endif 
