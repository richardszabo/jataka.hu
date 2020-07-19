/*
 * Artu based on 
 * Arduino Controlled Servo Robot (SERB) - Test Program
 * For more details visit: http://www.oomlout.com/serb 
 * 
 * Behaviour: Go forward, when obstacle is less than 20 cm ahead, then it turns right.
 *
 * Wiring: Right Servo Signal - pin 9
 *         Left Servo Signal - pin 10 
 *         Ultrasonic Signal - pin 7  
 *
 * License: This work is licenced under the Creative Commons 
 *          Attribution-Share Alike 3.0 Unported License. To 
 *          view a copy of this licence, visit 
 *          http://creativecommons.org/licenses/by-sa/3.0/ 
 *          or send a letter to Creative Commons, 171 Second 
 *          Street, Suite 300, San Francisco, California 94105, 
 *          USA.
 *         
*/ 
 
//---------------------------------------------------------------------------------
#include <Servo.h>

#define LEFTSERVOPIN  10
#define RIGHTSERVOPIN  9
#define SONARPIN  7

Servo leftServo; 
Servo rightServo; 

int speed = 100; //sets the speed of the robot (both servos) a percentage between 0 and 100

//---------------------------------------------------------------------------------


/*
 * sets pins to appropriate states and attaches servos. Then pauses
 * for 1 second before the program starts
*/ 
void setup()                  
{
  serbSetup();                       //sets the state of all neccesary pins and adds servos to your sketch
  delay(1000);
  Serial.begin(9600);
}

/*
 * 
*/
void loop()                     
{
   long cm;
   
   cm = ping();
   Serial.print(cm);
   Serial.println();
   if( cm < 20 ) {
     goRight();
     delay(1000);                     //turns for 1 seconds
   } else {
     goForward();
     delay(500);                     //goes straight for 0.5 seconds
   }
}

/*
 * turns the robot randomly left or right for a random time period between
 * minTime (milliseconds) and maxTime (milliseconds)
 */
void turnRandom(int minTime, int maxTime){
  int choice = random(2);                     //Random number to decide between left (1) and right (0)
  int turnTime = random(minTime,maxTime);     //Random number for the pause time
  if(choice == 1){ goLeft();}                 //If random number = 1 then turn left
  else {goRight();}                           //If random number = 0 then turn right
  delay(turnTime);                            //delay for random time                         
}

//------------------------------------------------------------------------------------------------------------
//START OF ARDUINO CONTROLLED SERVO ROBOT (SERB) ROUTINES

/*
 * sets up your arduino to address your SERB using the included routines
*/
void serbSetup(){
  setSpeed(speed);
  pinMode(LEFTSERVOPIN, OUTPUT);     //sets the left servo signal pin to output
  pinMode(RIGHTSERVOPIN, OUTPUT);    //sets the right servo signal pin to output
  leftServo.attach(LEFTSERVOPIN);    //attaches left servo
  rightServo.attach(RIGHTSERVOPIN);  //attaches right servo
  goStop();
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

/*
 * sends the robot forwards
 */
void goForward(){
 leftServo.write(90 + speed);
 rightServo.write(90 - speed);
}
  
/*
 * sends the robot right
 */
void goRight(){
 leftServo.write(90 + speed);
 rightServo.write(90 + speed);
}

/*
 * sends the robot left
 */
void goLeft(){
 leftServo.write(90 - speed);
 rightServo.write(90 - speed);
}

/*
 * stops the robot
 */
void goStop(){
 leftServo.write(90);
 rightServo.write(90);
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
  duration = pulseIn(SONARPIN, HIGH);

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
