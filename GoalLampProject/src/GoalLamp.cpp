#include "Lamp.h"
#include "Private.h"
#include <Arduino.h>



NhlGame game;


/*Neopixels*/
NeoPixelStrip strip(PIXEL_COUNT, PIXEL_PIN_1);


AnimationEngine AE(&game, &strip, 100);


void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  WiFi.mode(WIFI_STA);

  
  Animation ani(lightSpinningLamp, &strip, &game, RgbColor(255,0,0), 10);
  Animation ani2(fillWithColor, &strip, &game, RgbColor(0,255,0), 1000);
  //Animation ani3(fillWithColor, &strip, &game, RgbColor(0,0,255), 100);
  AE.AddAnimation(ani);
  

  //AE.AddAnimation(ani2);
  //AE.AddAnimation(ani3);

  AE.StartAnimation();



  //Init lightstrip
  strip.Begin();
  strip.Show();

  //Connect to internet
  bool ff = false;
  while ( WiFi.status() != WL_CONNECTED ) {
    
    delay ( 500 );
    Serial.println(String(ff) + " ");
    //flash bottom row of pixels when connecting
    if(ff){
      ClearLamp(&strip);
      ff = false;
    }else{
        lightLine(0, 0, 0, 8, RgbColor(0,0,255), &strip);
        ff = true;
    }
  }
  //turn OTA on
  InitOta();

  //Init UDP for NTP
  InitTime();


  //create path on startup
  //AlarmCheckGame();  
  //BuildNHLPath();

  //Alarm.timerOnce(10, AlarmGameUpdate);

  //print("ATL", RgbColor(0,225,0), &strip,game.home, lightLamp);

  //Serial.println(String(Id));

  //at 8:30AM create Game
  //Alarm.alarmRepeat(8,30,0, AlarmCheckGame);


}


void loop() {
  
  AE.handleAnimation();
  
  ArduinoOTA.handle();
  Alarm.delay(10);
   
}

void AlarmCheckGame(){
  BuildNHLPath();
  Alarm.alarmOnce(game.time - 600, AlarmGameUpdate);
}

void AlarmGameUpdate(){

  switch(CheckNHLScore()) {
    case 0: 
      //no Score update
      showPixelScore(game, &strip);
      break;
    case 1: 
      //Home Score Update
      //lightSpinningLamp(5, &strip);
      showScoreNumbers(game, &strip);
      break;
    case 2: 
      //Away Score Update
      //lightLamp( &strip);
      showScoreNumbers(game, &strip);
      break;
  }
  Serial.println(game.toString());

  //Scheduled
  //Pre-Game
  //
  if(!game.detailedState.equals("Final"))
  {
    Alarm.timerOnce(10, AlarmGameUpdate);
  }else{
    ClearLamp(&strip);
  }
}



//builds nhlPath
void BuildNHLPath(){
    Serial.print("GET /api/v1/schedule?startDate=");
    Serial.print(String(month()) + "/" + String(day()) + "/" + String(year()));
    Serial.print("&endDate=");
    Serial.print(String(month()) + "/" + String(day()) + "/" + String(year()));
    Serial.println("&site=en_nhl&teamId=7 HTTP/1.1");

    WiFiClient client;

    int gamePk = 0;
    int season = 0;
 
    if (client.connect("statsapi.web.nhl.com", 80))
    {
      client.print("GET /api/v1/schedule?startDate=");
      client.print(String(month()) + "/" + String(day()) + "/" + String(year()));//05/25/2019
      //client.print("10/11/2019");
      client.print("&endDate=");
      client.print(String(month()) + "/" + String(day()) + "/" + String(year()));
      //client.print("10/11/2019");
      client.println("&site=en_nhl&teamId=7 HTTP/1.1");
      client.println("Host: statsapi.web.nhl.com");
      client.println("Connection: keep-alive");
      client.println("Cache-Control: max-age=0");
      client.println("Upgrade-Insecure-Requests: 1");
      client.println("User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/74.0.3729.169 Safari/537.36");
      client.println("DNT: 1");
      client.println("Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3");
      client.println("Accept-Language: en-US,en;q=0.9");
      client.println();
  
      while (client.connected())
      {
        if (client.available()) {

          /*
            teamIdH = root["dates"][0]["games"][0]["teams"]["home"]["team"]["id"];
           teamIdA = root["dates"][0]["games"][0]["teams"]["away"]["team"]["id"];
           gameStatus = atoi(root["dates"][0]["games"][0]["status"]["codedGameState"]);
         */

          client.findUntil("\"gamePk\" :", "\0"); 

          gamePk = client.readStringUntil(',').toInt();
  
          client.findUntil("\"season\" : \"", "\0"); 

          season = client.readStringUntil('"').toInt();

          client.findUntil("\"gameDate\" : \"", "\0");

          game.time = TimeStringToTime(client.readStringUntil('"'),0);

          client.findUntil("\"detailedState\" : \"", "\0"); 
          game.detailedState = client.readStringUntil('"');

          client.findUntil("\"id\" :", "\0"); 
          game.away.id = client.readStringUntil(',').toInt();
          
          client.findUntil("\"id\" :", "\0"); 
          game.home.id = client.readStringUntil(',').toInt();

          Serial.println("GameTime- " + String(game.time));
          break;
        }
      }

      if(gamePk && season){
       sprintf(game.path,"/GameData/%i/%i/gc/gcsb.jsonp", season ,gamePk);
       //Serial.println(game.path);
      }else{
       Serial.println("ERROR Unable To Build Path");
      }
      client.stop();
    }
}



/*
    returns: 0 (no score update)
             1 (home Score update)
             2 (away Score update)
*/
int CheckNHLScore(){


    WiFiClient client;

    String awayTeamName;
    String homeTeamName;
    int newAwayScore = 0;
    int newHomeScore = 0;
    int scoreUpdateStatus = 0;

    Serial.print("GET live.nhle.com");
    Serial.println(game.path);
 
    if (client.connect("live.nhle.com", 80))
    {
      client.print("GET ");
      client.print(game.path);
      client.println(" HTTP/1.1");
      client.println("Host: live.nhle.com");
      client.println("Connection: keep-alive");
      client.println("Cache-Control: max-age=0");
      client.println("Upgrade-Insecure-Requests: 1");
      client.println("User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/77.0.3865.90 Safari/537.36");
      client.println("DNT: 1");
      client.println("Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3");
      client.println("Accept-Encoding: gzip, deflate");
      client.println("Accept-Language: en-US,en;q=0.9");
      client.println();
  
      while (client.connected())
      {
        if (client.available()) {;

          client.findUntil("\"ab\":\"", "\0"); 

          game.away.name = client.readStringUntil('"');

          client.findUntil("\"tot\":", "\0"); 
          client.findUntil("\"g\":", "\0"); 

          newAwayScore = client.readStringUntil(',').toInt();

          client.findUntil("\"ab\":\"", "\0"); 

          game.home.name = client.readStringUntil('"');

          client.findUntil("\"tot\":", "\0"); 
          client.findUntil("\"g\":", "\0"); 

          newHomeScore = client.readStringUntil(',').toInt();
  
          
          break;

        }
      }

      client.stop();
    }

    if(game.home.score != newHomeScore){
        game.home.score = newHomeScore;
        scoreUpdateStatus = 1;
    }
    if(game.away.score != newAwayScore){
        game.away.score = newAwayScore;
        scoreUpdateStatus = 2;
    }
    Serial.println("score Update Status " + String(scoreUpdateStatus));
    return scoreUpdateStatus;
}


void InitOta(){
  
      ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
}
