#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include "TimerOne.h"
//#include "TimerThree.h"

//#include "defines.h"

//State variables
int sensorstate=0;
int ledstate=0;
int musicstate=0;

//Input
#define distPin1 5 // Distance sensor analog in
#define distPin2 6
#define distPin3 7

// Output
#define redPin  3   // Red LED,   connected to digital pin 9
#define grnPin  5  // Green LED, connected to digital pin 10
#define bluPin  6  // Blue LED,  connected to digital pin 11


//Measured variables
volatile word s1_adc=0;
volatile word s2_adc=0;
volatile word s3_adc=0;
volatile word t_s1_adc[4]={0,0,0,0};
volatile word t_s2_adc[4]={0,0,0,0};
volatile word t_s3_adc[4]={0,0,0,0};
volatile word max_adc;
volatile int millis_elapsed=0;
volatile int millis_last=0;


// Color arrays
byte black[3]  = { 0, 0, 0 };
byte white[3]  = { 255, 255, 255 };
byte red[3]    = { 255, 0, 0 };
byte green[3]  = { 0, 255, 0 };
byte blue[3]   = { 0, 0, 255 };
byte yellow[3] = { 255, 255, 0 }; // wtf not valid { 40, 95, 0 };
byte dimWhite[3] = { 80, 80, 80 };
// etc.

// Set initial color
byte redVal = black[0];
byte grnVal = black[1]; 
byte bluVal = black[2];
bool valComputed = false;

int wait = 11;      // 10ms internal crossFade delay; increase for slower fades -- can be slower if the distance is more
int hold = 0;       // Optional hold when a color is complete, before the next crossFade
int DEBUG = 1;      // DEBUG counter; if set to 1, will write values back via serial
int loopCount = 200; // How often should DEBUG report?
int repeat = 0;     // How many times should we loop before stopping? (0 for no stop)
int j = 0;          // Loop counter for repeat

// Initialize color variables
byte prevR = redVal;
byte prevG = grnVal;
byte prevB = bluVal;

// Data variables
//double distance1=10; //in m, for sensor1
double sense_radius=30;
double dist_far_trig= 80;  //adc value
double dist_mid_trig= 250;  //
double dist_near_trig= 350;  //

//SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);
void DFRsetup();
void DFRloop();
bool getSensorVal();
void modifyFading();
void ledFade();
void sensorRead();

int lastread=0;
int avg_i=0;

void ADChandler()
{ //timer triggered
  
  lastread=(3==lastread)?(0):(lastread+1);
  
  t_s1_adc[lastread] = analogRead(distPin1);
  t_s2_adc[lastread] = analogRead(distPin2);
  t_s3_adc[lastread] = analogRead(distPin3);
  s1_adc=0;
  s2_adc=0;
  s3_adc=0;
  for(avg_i=0;avg_i<4;avg_i++)
  {
    s1_adc+=t_s1_adc[avg_i];
    s2_adc+=t_s2_adc[avg_i];
    s3_adc+=t_s3_adc[avg_i];
  }
  s1_adc=s1_adc>>2; //divide by 4
  s2_adc=s2_adc>>2; 
  s3_adc=s3_adc>>2;

  max_adc=max(s1_adc,max(s2_adc,s3_adc));
  sensorstate= (sense_radius < max_adc)?(true):(false);
  modifyFading();

  if(0==lastread){
  Serial.print("adc values:");
  Serial.print(s1_adc);
  Serial.print("\t");
  Serial.print(s2_adc);
  Serial.print("\t");
  Serial.print(s3_adc); 
  Serial.print("\t");
  Serial.print(max_adc);
  Serial.print("\t");
  Serial.println((0==sensorstate)?("true"):("false")); }
  return;
}

void setup()
{
  //initialize LEDs
  pinMode(redPin, OUTPUT);   // sets the pins as output
  pinMode(grnPin, OUTPUT);   
  pinMode(bluPin, OUTPUT); 

  //initialize sensors
  pinMode(distPin1, INPUT);
  pinMode(distPin2, INPUT);
  pinMode(distPin3, INPUT);

  //sensor ADC
  Timer1.initialize(500000);
  Timer1.attachInterrupt(ADChandler);

  //DFRsetup();
  //TODO: init the mp3 play, with library provided at https://github.com/DFRobot/DFRobotDFPlayerMini

  //
    Serial.begin(9600);  // ...set up the serial ouput 
  }

//Invoked by main function


//Modifies fading speed based on sensor data
void modifyFading() // change the color transition
{ 
  //set the number of steps according to distance
  if(sensorstate){            // valid value
      //tavol van: 0, kozel van: kb 600
     /*if (max_adc < dist_far_trig && max_adc > sense_radius) //kevesebb az adcval 150-nel (kozel van nagyon) de tobb 80-nal -- tavol=0
     {
     wait=50;                 // wait less if the object is closer
     //Timer3.setPeriod(50000);
     }
     else if (max_adc > dist_mid_trig)
     {
     //Timer3.setPeriod(10000);
     wait=10; 
     }
     else if (max_adc > dist_near_trig)
     {
     //Timer3.setPeriod(500);
     wait=0; 
     }
     return;*/
     wait=5;
    }
    else 
    {
      wait=50;
    }
  //TODO: more steps
//TODO: off transition

  
  //DEBUG
  if(DEBUG){
    Serial.print("wait: ");
    Serial.println(wait);
  }
  return;
}

//Functions below: 
//      RGB hexa value change in one iteration (of the 1020)
//      for the next i the analog value on pins

int calculateStep(int prevValue, int endValue) {
  int step = endValue - prevValue; // What's the overall gap?
  if (step) {                      // If its non-zero, 
    step = 1020/step;              //   divide by 1020
  } 
  return step;
}

int calculateVal(int step, int val, int i) {
  if ((step) && i % step == 0) { // If step is non-zero and its time to change a value,
    if (step > 0) { val += 1; }  //   increment the value if step is positive...         
    else if (step < 0) { val -= 1;   }             //   ...or decrement it if step is negative
          // novel vagy csokkent a brightnessen
  }
  //szaturacio:
  val=(val<255)?(val):(255);
  val=(val<0)?(0):(val);

  /*if (DEBUG) { // If we want serial output, print it at the 
      if (i == 0 or i % loopCount == 0) { // beginning, and every loopCount times
        //Serial.print("Loop/12VBRG: #");
        //Serial.print(i);
        Serial.print(" |calcval ");
        Serial.print(step);
        Serial.print(" / ");
        Serial.print(val);
        Serial.print(" / ");  
        //Serial.print(stepB); 
        Serial.print(" adcval ");
        Serial.println(s1_adc);
      } 
      DEBUG += 1;
      }*/
  return val;
}

//Functions above used by crossfade, it is a transition to next color.

void crossFade(byte color[3]) {
  // Convert to 0-255 not necess
  //Serial.println(" color ");
  //millis_last=millis();
   if (DEBUG) {
        Serial.print(" initial: ");
        Serial.print(color[0]);
        Serial.print(" | ");
        Serial.print(color[1]);
        Serial.print(" / ");
        Serial.println(color[2]); 
      DEBUG += 1;
  }
  /*if(!sensorstate)
  {
    color=black;
  }*/
  
  int R = color[0];
  int G = color[1];
  int B = color[2];
  
  if (DEBUG) {
        Serial.print(" color ");
        Serial.print(R);
        Serial.print(" | ");
        Serial.print(G);
        Serial.print(" / ");
        Serial.println(B); 
      DEBUG += 1;
  }
  int stepR = calculateStep(prevR, R);
  int stepG = calculateStep(prevG, G); 
  int stepB = calculateStep(prevB, B);

  Serial.print(" for cycle time: ");
  Serial.println(stepR);
  for (int i = 0; i <= 1020; i++) { //modified from 1020
      //TODO: loop
      //Serial.print(" one loop: ");
      //Serial.println(millis());
      redVal = calculateVal(stepR, redVal, i);
      grnVal = calculateVal(stepG, grnVal, i);
      bluVal = calculateVal(stepB, bluVal, i);
      
      /*if (i == 0 or i % loopCount*5 == 0) {
      modifyFading();
      }*/
      //ha letelt az idő:
      valComputed= true; //ready to be written out 
      //writeColor();
      delay(wait);  

      if (DEBUG) { // If we want serial output, print it at the 
      if (i == 0 or i % loopCount == 0) { // beginning, and every loopCount times
        Serial.print("Loop/12VBRG: #");
        //Serial.print(i);
        Serial.print(" | ");
        Serial.print(stepR);
        Serial.print(" / ");
        Serial.print(stepG);
        Serial.print(" / ");  
        Serial.print(stepB); 
        Serial.print(" adcval ");
        Serial.println(s1_adc);
      } 
      DEBUG += 1;
      }
      writeColor();
  }
  // Update current values for next loop
  prevR = redVal; 
  prevG = grnVal; 
  prevB = bluVal;
  delay(hold); // Pause for optional 'wait' milliseconds before resuming the loop
}

void writeColor()
{
    //TODO: csak akkor irjon ha....
    //
    /// for loop hasából, globális változót használva
    if(max_adc <sense_radius){
      redVal=redVal>>1;
      bluVal=bluVal>>1;
      grnVal=grnVal>>1;

    }
            Serial.print(redVal>>1);
            Serial.print("\t");

    
    analogWrite(redPin, redVal);   // Write current values to LED pins
    analogWrite(grnPin, grnVal);      
    analogWrite(bluPin, bluVal); 
   
    /*if (DEBUG) { // If we want serial output, print it at the 
      //if (i == 0 or i % loopCount == 0) { // beginning, and every loopCount times
        Serial.print("Loop/12VBRG: #");
        //Serial.print(i);
        Serial.print(" | ");
        Serial.print(bluVal);
        Serial.print(" / ");
        Serial.print(redVal);
        Serial.print(" / ");  
        Serial.print(grnVal); 
        Serial.print(" adcval ");
        Serial.println(s1_adc);
      //} 
      DEBUG += 1;
    }*/
    //valCompleted=false;
}   

void ledFade(){
  crossFade(red);
  crossFade(green);
  crossFade(blue);
  crossFade(yellow);
  crossFade(black);
}

// Main program: list the order of crossfades
void loop()
{
  
    ledFade();
    //crossFade(red);
    //crossFade(white);
    /*analogWrite(redPin, redVal);   // Write current values to LED pins
    analogWrite(grnPin, grnVal);      
    analogWrite(bluPin, bluVal); */


  // DFRloop();
  
  
}
