#include <SoftwareSerial.h>

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  pinMode(6, OUTPUT); //B
  pinMode(5, OUTPUT); //G
  pinMode(3, OUTPUT); //R
  TCCR0A = _BV(COM0A1) | _BV(COM0B1) | _BV(WGM00);
//WGM0[2:0]=001b PWM, Phase Correct TOP=0xFF update of OCR0x at TOP and TOV flag Set on BOTTOM(BOTTOM=0x00)
//COM0A[1:0]=10b Clear OC0A on Compare Match when up-counting. Set OC0A on Compare Match when down-counting.
//com0B[1:0]=10b Clear OC0B on Compare Match when up-counting. Set OC0B on Compare Match when down-counting.
  
  TCCR0B = _BV(CS00);
//CS0[2:0]=001b clk i/o / 4^cs0=256
 
  


  TCCR2A = _BV(COM2B1) | _BV(WGM20);
  TCCR2B = _BV(CS20);
  OCR2B = 125; //R
  OCR0A = 50;   //B   //compare values
  OCR0B = 100; //G
}

int timer=millis();
void loop() {
  // put your main code here, to run repeatedly:
/*  while(OCR0B){
    timer=millis();
    OCR0A++;
    OCR0B--;
    Serial.println(millis());
    while(millis()-timer<100) 
    {}
  }*/
}
