#include "LampTime.h"

WiFiUDP ntpUDP;
time_t t;
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

void InitTime(){
  ntpUDP.begin(1337);
  setSyncProvider(getNtpTime); 
  
}

time_t TimeStringToTime(String ts, int MinutesOffset){
  int YYYY;
  int MM;
  int DD;
  int hh;
  int mm;
  int ss;

  //Serial.println(ts);

  ss = ts.substring(17, 19).toInt();
  mm = ts.substring(14, 16).toInt();  
  hh = ts.substring(11, 13).toInt();
  DD = ts.substring(8, 10).toInt();
  MM = ts.substring(5, 7).toInt();
  YYYY = ts.substring(0, 4).toInt();

  mm += MinutesOffset;
  if(mm < 0){
    mm = 60 + mm;
    hh--;
  }
  hh = hh + timeZone;
  if(hh< 0){
    hh = 24 + hh;
    DD--;
  }
  
  
  //Serial.println(String(YYYY) + " " + String(MM) + " " + String(DD) + " " + String(hh) + " " + String(mm) + " " + String(ss));
  //2019-05-26T18:15:00Z
  tmElements_t tmSet;
  tmSet.Year = YYYY - 1970;
  tmSet.Month = MM ;
  tmSet.Day = DD;
  
  tmSet.Hour = hh;
  tmSet.Second = ss;
  return makeTime(tmSet);         //convert to time_t
}

String addPreceding0(int digits){
  // utility for digital clock display: prints preceding colon and leading 0
  String retString = "";
  if (digits < 10){
    retString = "0";
  }
  retString += String(digits);
  return retString;
}

time_t getNtpTime(){
  while (ntpUDP.parsePacket() > 0) ; // discard any previously received packets
  Serial.println("Transmit NTP Request");
  sendNTPpacket();
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = ntpUDP.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println("Receive NTP Response");
      ntpUDP.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      time_t retTime = secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
      setTime(retTime);
      return retTime;
    }
  }
  Serial.println("No NTP Response :-(");
  //ESP.restart();
  return 0; // return 0 if unable to get the time
}
// send an NTP request to the time server at the given address
void sendNTPpacket(){
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:                 
  ntpUDP.beginPacket("pool.ntp.org", 123); //NTP requests are to port 123
  ntpUDP.write(packetBuffer, NTP_PACKET_SIZE);
  ntpUDP.endPacket();
}
