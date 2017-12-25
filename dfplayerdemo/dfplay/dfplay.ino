//recsegés tesztelése időzítés alapon
//(észrevehetetlen már az ereje)

#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include "TimerOne.h"

#define mutePin 13

DFRobotDFPlayerMini myDFPlayer;
SoftwareSerial mySoftwareSerial(7, 4); // RX, TX 7 4  10  11

void setup() {
   pinMode(mutePin, OUTPUT);
  digitalWrite(mutePin,1);
  
  Serial.begin(9600);
  Serial.println("ena mute");
   mySoftwareSerial.begin(9600); // maybe higher baud leads to less noise?!
   Serial.println(F("Initializing, DFPlayer"));
   if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to connect"));
    while(true);
   }
   // DELAY 3 SEC & UNMUTE HERE:
   delay(3000);
   myDFPlayer.volume(5);
   myDFPlayer.loop(2);
   digitalWrite(mutePin,0);
   Serial.println("disable mute");
   Serial.println(F("online."));
}

void loop() {
  delay(400);
  myDFPlayer.volume(10);
  //Serial.println(millis());
  
  //Serial.println(millis());
  
}
