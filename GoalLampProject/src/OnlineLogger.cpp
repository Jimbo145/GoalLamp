#include "Onlinelogger.h"

ESP8266WebServer server(80);

int OnlineLogger::init(){

  // Wait for connection

  while (WiFi.status() != WL_CONNECTED) {
        return -1;
    }

  if (MDNS.begin("GoalLamp")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", std::bind(&OnlineLogger::handleRoot, this) );

  server.onNotFound(std::bind(&OnlineLogger::handleNotFound, this));

  server.begin();
  log("LogServer Begun", DEBUG);
  return 0;
}

void OnlineLogger::handle(){
    server.handleClient();
    MDNS.update();
}

void OnlineLogger::handleRoot() {
  String table = "<html> <head> <link rel=\"stylesheet\" href=\"https://cdnjs.cloudflare.com/ajax/libs/foundation/6.6.0/css/foundation.min.css\"> </head> <body>";
    table += "<table style=\"\">";
    for(int i = 0; i<LOG_SIZE; i++ ){
      if(!logbook[i].time ){
        continue;
      }
      table += "<tr>";
      table += "<th>";
      switch(logbook[i].level) {
          case ERROR : 
            table += String("ERROR");
            break;
          case INFO : 
            table += String("INFO");
            break;
          case DEBUG : 
            table += String("DEBUG");
            break;
      }
      table += "</th>";
      //Column2
      table += "<th>";
      table += String(hour(logbook[i].time));
      table += ":";
      table += String(minute(logbook[i].time));
      table += ":";
      table += String(second(logbook[i].time));
      table += "</th>";
      //Column3
      table += "<th>";
      table += String(logbook[i].LogMessage);
      table += "</th>";
      

    }
  table += "</table>";
  table += "</body> </html>";

  server.send(200, "text/html", table);
}

void OnlineLogger::handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void OnlineLogger::log(String LogMessage, LogLevel level){
  if(logPos>=LOG_SIZE){
    logPos = 0;
  }
  logbook[logPos].level = level;
  logbook[logPos].LogMessage = LogMessage;
  logbook[logPos].time = now();

  logPos++;
}