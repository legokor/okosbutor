//    input variables nextSound
//                    sensorstate
//                    max_adc
//   output variables music               
//                    prevsensorstate
//                    timeoutCounter
//                    started
//                    finVol
//                    curVol
//                    
//
    
void inline sound(void){
    if(nextSound){                                         //change to next music
    myDFPlayer.play((music==3)?(0):(++music));
    }
    if(prevsensorstate<sensorstate){                       // rising edge
      digitalWrite(mutePin,0);  //unmute
      if(millis()>timeoutCounter+timeStopMillis)           //if there was a big low sensorstate
      { //stop timeout volt --> elorol kezd
            myDFPlayer.enableLoopAll();
      }
      else                                                 
      {
        myDFPlayer.start(); //must be in else branch!
        started=true;
      }
    }
    
    if(sensorstate){
      finVol=map(max_adc,0,511,0,15);
      timeoutCounter=millis();
    }
    else if (prevsensorstate>sensorstate)                 // falling edge
    {
      timeoutCounter=millis(); //redundant...
    }
    else if (millis()>timeoutCounter+timeoutMillis){               // timeout happened
      finVol=0;
    }

    
/////////////////////////////   SOUND FADING   ////////////////////////////////
  
    if(curVol<finVol){
      myDFPlayer.volume(curVol);
      curVol++;
      Serial.println("vol++");
    }
    else if(curVol>finVol){
      myDFPlayer.volume(curVol);
      curVol--;
      Serial.println("vol--");
    }
  
    if(0==curVol){
            myDFPlayer.pause();
            digitalWrite(mutePin,1);
            //paused=true;
    }


    prevsensorstate=sensorstate;
  


}

