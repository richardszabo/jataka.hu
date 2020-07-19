/*
 * BegR based on 
 * Arduino Controlled Servo Robot (SERB) - Test Program
 * For more details visit: http://www.oomlout.com/serb 
 * 
 * Wiring: Right Servo Signal - pin 9
 *         Left Servo Signal - pin 10 
 *         Ultrasonic Signal - pin 7  
 *         Cup Servo Signal - pin 6 
 *         Head Servo Signal - pin 5
 * 
*/ 
 
//---------------------------------------------------------------------------------
#include <Servo.h>

#define LEFTSERVOPIN  10
#define RIGHTSERVOPIN  9
#define CUPSERVOPIN  6
#define HEADSERVOPIN  5
#define SONARPIN  7

Servo leftServo; 
Servo rightServo; 
Servo cupServo; 
Servo headServo; 

int speed = 100; //sets the speed of the robot (both servos) a percentage between 0 and 100
int turnDirection = 0;
int inByte = 0;         // incoming serial byte

#define NO_PINGS 3
long cms[NO_PINGS] = {-1,-1,-1};

int automove = 0;
#define MOVE 0
#define TURN 1
int state;
unsigned long actionStart;
#define TURNTIME 500
#define MOVEMIN 3000
#define MOVEMAX 7000
int moveTime;


//---------------------------------------------------------------------------------


/*
 * sets pins to appropriate states and attaches servos. Then pauses
 * for 1 second before the program starts
*/ 
void setup()                  
{
  setSpeed(speed);
  pinMode(LEFTSERVOPIN, OUTPUT);     //sets the servos signal pin to output
  pinMode(RIGHTSERVOPIN, OUTPUT);    
  pinMode(CUPSERVOPIN, OUTPUT);      
  pinMode(HEADSERVOPIN, OUTPUT);      
  leftServo.attach(LEFTSERVOPIN);    //attaches thes servos
  rightServo.attach(RIGHTSERVOPIN);  
  cupServo.attach(CUPSERVOPIN);      
  headServo.attach(HEADSERVOPIN);      
  cupServo.write(90);
  headServo.write(90);
  goStop();
  delay(1000);
  Serial.begin(9600);
  state = MOVE;
  // automove switched on when there is no PC connection
  automove = 1;
}

/*
 * 
*/
void loop()                     
{     
  long cm;
  
  if (Serial.available() > 0) {
     // get incoming byte:
     inByte = Serial.read();   
     switch(inByte) {
       case 'p':
         cm = ping();
         Serial.print(cm);
         Serial.println();
       break;
       case 'w':
         goForward();        //goes straight for 0.5 seconds
         delay(500);
       break;
       case 'a':
         turn(-1);
         delay(150);
       break;
       case 's':
         turn(1);
         delay(150);
       break;
       case 'z':
         goStop();
         automove = 0;
         clearSenses();
       break;
       case 'm':
         automove = 1;
        break; 
       case 'c':
         shake();
         //delay(1000);                     //waits for 1 seconds
       break;
       case 'h':
         turnHead();
         //delay(1000);                     //waits for 1 seconds
       break;
     }
  }
   if( automove == 1 ) {
       beg();
   }    
}

//------------------------------------------------------------------------------------------------------------

void beg() {
       if( state == TURN ) {
         if( millis() - actionStart < TURNTIME ) {
           turn(turnDirection);         
           return;
         } else {
           startMove();
           Serial.print("startMove:");       
           Serial.print(moveTime);       
           Serial.println();
         }
       }
       // finding obstacles
       if( cms[0] != 0 ) {  // moving sensing queue forward
         for( int i = NO_PINGS -1; i > 0; --i) {
           cms[i] = cms[i-1]; 
         }
       }
       cms[0] = ping(); // new sensation
       Serial.print(cms[0]);
       Serial.println();
       int isWallClose = 0;
       for( int i = 0; i < NO_PINGS; ++i) { // finding obstacle
           if( cms[i] < 30 && cms[i] > 0 ) {
             isWallClose = 1;             
           }
       }
       
       if( isWallClose ) {     // wall/person found
           goStop();
           shakeAndStartTurn();
           Serial.print("***************************************");       
           Serial.println();
           Serial.print("New turndirection is ");       
           Serial.print(turnDirection);       
           Serial.println();
       } else {
           Serial.print(millis());
           Serial.print(":");
           Serial.print(actionStart);
           Serial.println();         
         if( millis() - actionStart < moveTime ) {
           Serial.print("goForward");       
           Serial.println();         
           goForward();
         } else {
           Serial.print("turn");       
           Serial.println();         
           goStop();
           turnHead();
           shakeAndStartTurn();
         }        
       }       
}

void clearSenses() {
         for( int i = 0; i < NO_PINGS; ++i) {
             cms[i] = -1;
         }
}

void startMove() {
         state = MOVE;
         moveTime = random(MOVEMAX-MOVEMIN) + MOVEMIN;
         actionStart = millis();           
}

void shakeAndStartTurn() {
         state = TURN;      // robot turns
         clearSenses();     // cleaning up to pings because we will turn for a while
         shake();         // shake, may be the obstacle is a person
         delay(1000);                     //waits for 1 seconds
         turnDirection = random(2)*2-1; // setting turning direction
         actionStart = millis();
}

/*
 * sets the speed of the robot between 0-(stopped) and 100-(full speed)
 * NOTE: speed will not change the current speed you must change speed 
 * then call one of the go methods before changes occur.
*/ 
void setSpeed(int newSpeed){
  if(newSpeed >= 100) {newSpeed = 100;}     //if speed is greater than 100 make it 100
  if(newSpeed <= 0) {newSpeed = 0;}         //if speed is less than 0 make it 0 
  speed = newSpeed * 0.9;                   //scales the speed to be between 0 and 90
}

void shakeOnce(){
  /*cupServo.write(0);
 delay(1000);
  cupServo.write(180);
 delay(1000);*/
 cupServo.write(100);
 delay(100);
 cupServo.write(80);
 delay(100);
}

/*
 * shakes the cup
 */
void shake(){
  int i;
 for( i = 0; i < 3; ++i)  {
   shakeOnce();
 }
 cupServo.write(90);
}

/*
 * stops the robot
 */
void goStop(){
 leftServo.write(90);
 rightServo.write(90);
} 

/*
 * sends the robot forwards
 */
void goForward(){
 leftServo.write(90 + speed);
 rightServo.write(90 - speed);
}

/*
 * turns the robot
 */
void turn(int turnDirection){
 leftServo.write(90 + turnDirection * speed);
 rightServo.write(90 + turnDirection * speed);
}

/*
 * turns the robot head
 */
void turnHead(){
 headServo.write(150);
 delay(300);
 headServo.write(30);
 delay(600);
 headServo.write(90);
}

/**
 * Returns distance in centimeters.
 */
long ping() 
{
  long duration;

  // The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
  // We give a short LOW pulse beforehand to ensure a clean HIGH pulse.
  pinMode(SONARPIN, OUTPUT);
  digitalWrite(SONARPIN, LOW);
  delayMicroseconds(2);
  digitalWrite(SONARPIN, HIGH);
  delayMicroseconds(5);
  digitalWrite(SONARPIN, LOW);

  // The same pin is used to read the signal from the PING))): a HIGH
  // pulse whose duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(SONARPIN, INPUT);
  duration = pulseIn(SONARPIN, HIGH,50000);

  // convert the time into a distance
  return microsecondsToCentimeters(duration);  
}

long microsecondsToCentimeters(long microseconds)
{
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29 / 2;
}

//------------------------------------------------------------------------------------------------------------
