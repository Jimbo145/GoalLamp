#include "Lamp.h"



NhlGame game;


/*Neopixels*/
NeoPixelStrip strip(PIXEL_COUNT, PIXEL_PIN_1);


OnlineLogger logger;

AnimationEngine AE(&game, &strip, 100);

bool waitingForTime = false;



void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  WiFi.mode(WIFI_STA);
  WiFi.hostname("GoalLamp");

 
  //turn OTA on
  

  //Test Game
  //game.home.id = 7;
  //game.home.name = "BUF";
  //game.home.score = 3;
  //game.away.id = 6;
  //game.away.name = "BOS";
  //game.away.score = 1;
  //game.detailedState = "Pre-Game";


  AnimationExtra extra;
  extra.uInteger = 1;
  extra.minFrameNum = 1000;
  
  //Animation ani(sideBySideTeamColor, &strip, extra, &game);
  //Animation ani(sideBySideTeamColor, &strip, &game, RgbColor(255,0,0), 10, );
  //Animation ani2(fillWithColor, &strip, &game, RgbColor(0,255,0), 1000);
  //Animation ani3(fillWithColor, &strip, &game, RgbColor(0,0,255), 100);
  //Animation ani(sideBySideTeamColor, &strip, extra, &game);
  //AE.AddAnimation(ani);

  

  AE.StartAnimation();



  //Init lightstrip
  strip.Begin();
  strip.Show();

  //Connect to internet
  bool ff = false;
  while ( WiFi.status() != WL_CONNECTED ) {
    
    delay ( 500 );
    //flash bottom row of pixels when connecting
    if(ff){
      ClearLamp(&strip);
      ff = false;
    }else{
        lightLine(0, 0, 0, 8, RgbColor(0,0,255), &strip);
        ff = true;
    }
    strip.Show();
  }

  ClearLamp(&strip);
  strip.Show();

  //internet connected... lets go
  logger.init(); 
  InitOta(); 


  //Init UDP for NTP
  if (!InitTime()){
    logger.log("InitTime Failed");
  }



 
  //BuildNHLPath();

   

  //at 8:30AM create Game
  Alarm.alarmRepeat(8,30,0, AlarmCheckGame);
  
  //create path on startup
  AlarmCheckGame();

}


void loop() {
  
  AE.handleAnimation();
  
  ArduinoOTA.handle();
  Alarm.delay(10);
  logger.handle();
   
}

void AlarmCheckGame(){
  if(timeStatus() == timeNotSet || timeStatus() == timeNeedsSync){
    logger.log("Waiting for time server");
    Serial.println("waiting for time server");
    Alarm.timerOnce(10, AlarmCheckGame);
  } else{
    BuildNHLPath();
    Serial.print(String(game.gameToday));
    Serial.println(" we made it out of the buld path");
    if(game.gameToday){
      Alarm.alarmOnce(game.time - 600, AlarmGameUpdate);
    }
  };
  
}

void AlarmGameUpdate(){
  
  //Show Pixel Score
  AnimationExtra extShowPix;
  Animation aniShowPix(showPixelScore, &strip, extShowPix, &game);

  //Light The Lamp for the Good Guys
  AnimationExtra extLightSpinning;
  extLightSpinning.uInteger = 20;
  Animation aniLightSpinning(lightSpinningLamp, &strip, extLightSpinning, &game);

  //Light the Lamp for the Bad Guys
  AnimationExtra extLight;
  extLight.minFrameNum = 20;
  extLight.color = RgbColor(255,0,0);
  Animation aniLight(fillWithColor, &strip, extLight, &game);

  //Clear Fill
  //AnimationExtra extClearLight;
  //extClearLight.minFrameNum = 20;
  //extClearLight.color = RgbColor(0,0,0);
  //Animation aniLightClear(fillWithColor, &strip, extClearLight, &game);

  AnimationExtra extTeamColor;
  extTeamColor.minFrameNum = 10;
  Animation aniTeamColor(evenOddTeamColor, &strip, extLight, &game);

  
  //logger.log("game Update");
  switch(CheckNHLScore()) {
    case 0: 
      //no Score update
      
      AE.AddAnimation(aniShowPix);
      
      break;
    case 1:       
      //Home Score Update
      extTeamColor.uInteger = 0;
      if(game.home.id == 7){
        AE.AddAnimation(aniLight);
      }else{
        AE.AddAnimation(aniLightSpinning);
      }
      AE.AddAnimation(aniTeamColor);
      AE.AddAnimation(aniShowPix);
      break;
    case 2: 
      //Away Score Update
      extTeamColor.uInteger = 1;
      if(game.home.id == 7){
        AE.AddAnimation(aniLight);
      }else{
        AE.AddAnimation(aniLightSpinning);
      }
      AE.AddAnimation(aniTeamColor);
      AE.AddAnimation(aniShowPix);
      break;
  }
  //logger.log(game.toString());

  //Scheduled
  //Pre-Game
  //
  if(!game.detailedState.equals("Final"))
  {
    Alarm.timerOnce(10, AlarmGameUpdate);
  }else{
    //AE.AddAnimation(aniLightClear);
    //ClearLamp(&strip);
  }
}



//builds nhlPath
void BuildNHLPath(){
  
    /*logger.log("BUILDNHLPATH GET /api/v1/schedule?startDate=" \
    + String(month()) + "/" + String(day()) + "/" + String(year()) \
    + "&endDate=" \
    + String(month()) + "/" + String(day()) + "/" + String(year()) \
    + "&site=en_nhl&teamId=7 HTTP/1.1");
    */

    

    WiFiClient client;
    int numberOfGamesToday = -1;

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

          client.findUntil("\"totalGames\" : ", "\0");
          numberOfGamesToday = client.readStringUntil(',').toInt();
          if(numberOfGamesToday == 0){
            game.gameToday = false;
            break;
          }else{
            game.gameToday = true;
          }

          Serial.println(String(numberOfGamesToday));

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

          break;
        }
      }

      if(gamePk && season){
        Serial.println("try to generate game pagth");
        sprintf(game.path,"/GameData/%i/%i/gc/gcsb.jsonp", season ,gamePk);

        if(game.detailedState.equals("In Progress") || game.detailedState.equals("In Progress - Critical") ||game.detailedState.equals("Final")){
        AlarmGameUpdate();
        }
        client.stop();
      }else if(!game.gameToday){
        
       logger.log("No Games Today");
      }else{
          
       logger.log("ERROR Unable To Build Path", ERROR);
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

    logger.log("GET live.nhle.com " + String(game.path));
 
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
    logger.log("score Update Status " + String(scoreUpdateStatus));
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
