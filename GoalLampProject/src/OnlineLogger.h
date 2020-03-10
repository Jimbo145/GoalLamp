#ifndef ONLINELOGGER_H
#define ONLINELOGGER_H

/* Logging Webserver
* 
*   Standalone implementation of ESP8266WebServer that allows for Serial entries to be written to the ESP IP address
*
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <TimeLib.h>


enum LogLevel { ERROR, INFO, DEBUG};

struct LogEntry{
    String LogMessage;
    LogLevel level;
    time_t time;
};


class OnlineLogger {
    public:
        int init(void);
        void handle(void);
        void log(String LogMessage, LogLevel level = DEBUG);
        void save(void);

    private:
        static const uint8_t LOG_SIZE = 20;
        void handleNotFound(void);
        void handleRoot(void);
        LogEntry logbook[LOG_SIZE];
        uint8_t logPos = 0;
};



#endif 
