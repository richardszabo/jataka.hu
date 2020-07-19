/*****************************************************************************/
/* File:         sticks_pulling.c                                            */
/* Version:      1.0                                                         */
/* Date:         07-Oct-98                                                   */
/* Description:  Khepera controller for stick pulling experiment             */
/* Author:       Auke Ijspeert ijspeert@lamisun1.epfl.ch                     */
/*****************************************************************************/
#include <stdio.h>
#include <Khepera.h>
#include <KheperaGripper.h>

#define M_PI                  3.1416
#ifndef random
#define random rand
#endif

#define TURN_AWAY_SPEED       10

#define ELEV_GRIP             205
#define ELEV_MIN              245
#define ELEV_MAX              150

#define IR_THRES_OBSTR        300
#define SENSIBILITY_THRESHOLD 2

#define GRIPPING_TIME         400
#define FORWARD_SPEED         10
#define BACKWARDS_SPEED       3
#define BACK_GRIP_STEPS       10
#define TURN_AWAY_STEPS       15
#define N_STEPS_OBST_AVOID    10 
#define WOBBLE_STEPS          60
#define WOBBLE_SPEED          3

#define BUFFER_SIZE           27

char BUFFER[BUFFER_SIZE];
int  STICK_FOUND;
int  VIRTUAL_IRDA_RECEIVE[4];
int  VIRTUAL_IRDA_SEND[4];
char MESSAGE,ANSWER;

void my_khepera_step();

/*******************************************************************************/
void khepera_get_virtual_IRDA()
{
  char message;
  int irda0,irda1,irda2,irda3;

  khepera_receive_serial(BUFFER_SIZE,BUFFER);
  sscanf(BUFFER,"%c| %d %d %d %d",&message,&irda0,&irda1,&irda2,&irda3);

  VIRTUAL_IRDA_RECEIVE[0]=irda0;
  VIRTUAL_IRDA_RECEIVE[1]=irda1;
  VIRTUAL_IRDA_RECEIVE[2]=irda2;
  VIRTUAL_IRDA_RECEIVE[3]=irda3;
}


/*******************************************************************************/
void turn_left(int v)
{
  int i;
  for(i=0;i<v;i++) {
    khepera_set_speed(LEFT,10);
    khepera_set_speed(RIGHT,-10);
    my_khepera_step();
  }
}

void turn_right(int v)
{
  int i;
  for(i=0;i<v;i++) {
    khepera_set_speed(LEFT,-10);
    khepera_set_speed(RIGHT,10);
    my_khepera_step();
  }
}


/*******************************************************************************/
void search_stick() /* return true and sets STICK_FOUND to 1 when a stick 
		       is found */
{
  void stick_check();
  void obstacle_avoidance(int n_steps);
  int32 r,l,left_speed,right_speed,count_damping;

  MESSAGE='-';
  STICK_FOUND=0;
  gripper_set_grip(0);

  for(;;) {
    khepera_get_virtual_IRDA();
    l = khepera_get_proximity(0)+khepera_get_proximity(1)+khepera_get_proximity(2);
    r = khepera_get_proximity(3)+khepera_get_proximity(4)+khepera_get_proximity(5);


    if (r+l < 120) {
      /* Nothing visible with proximity IR */
      count_damping=0;
      if (VIRTUAL_IRDA_RECEIVE[1]+
	  VIRTUAL_IRDA_RECEIVE[3] <20) { /* No lateral IRDA either */
	left_speed  = FORWARD_SPEED;
	right_speed = FORWARD_SPEED;
      }
      else {
	/* perform phototaxis to IRDA emission */
	left_speed  = 2-(VIRTUAL_IRDA_RECEIVE[1]-VIRTUAL_IRDA_RECEIVE[3])/200;
	right_speed = 2+(VIRTUAL_IRDA_RECEIVE[1]-VIRTUAL_IRDA_RECEIVE[3])/200;
      }
    } 
    else {
      count_damping++;
      left_speed  = -(l-r)/(10+5*count_damping);
      right_speed =  (l-r)/(10+5*count_damping);
      /*      left_speed  = -(l-r)/20;
	      right_speed =  (l-r)/20;*/
    }

    if (left_speed > FORWARD_SPEED) left_speed = FORWARD_SPEED;
    else if (left_speed < -FORWARD_SPEED) left_speed = -FORWARD_SPEED;
    if (right_speed > FORWARD_SPEED) right_speed = FORWARD_SPEED;
    else if (right_speed < -FORWARD_SPEED) right_speed = -FORWARD_SPEED;

    if ((left_speed==0) && (right_speed==0)) {
      if ((khepera_get_proximity(2)>600)&&(khepera_get_proximity(3)>600)) {
	/* stick presumably found */
	stick_check();
	if (!STICK_FOUND) {
	  turn_right(TURN_AWAY_STEPS+(random()%10)-5); 
	  obstacle_avoidance(N_STEPS_OBST_AVOID);
	  continue;}
	else break;
      }
      else if ((khepera_get_proximity(1)<900)&&(khepera_get_proximity(2)<800)&&(khepera_get_proximity(3)<800)&&(khepera_get_proximity(4)<900)) {
	left_speed  = 1; /* approach */
	right_speed = 1;
      }
      else {
	/* Go a little bit backwards with a little turn if already too close */
	left_speed  = -1; 
	right_speed = -2;
	count_damping = 0;
      }
    }
    khepera_set_speed(LEFT,left_speed);     /* Set the motor speeds */
    khepera_set_speed(RIGHT,right_speed);
    my_khepera_step(); 
  }
}

/*******************************************************************************/
void stick_check() 
{
  int32 left_speed,right_speed;
  int i,j,sensors,large;

  /* Check whether a stick has been found: wobble */
  STICK_FOUND = 1;

  large=0;
  for (i=0;i<WOBBLE_STEPS;i++) {
    if (i<WOBBLE_STEPS/4) {
      /* Turn left */
      left_speed  = -WOBBLE_SPEED;
      right_speed = WOBBLE_SPEED;
    }
    else if (i>=WOBBLE_STEPS/4 && i<3*WOBBLE_STEPS/4) { 
      /* Turn right */
      left_speed  = WOBBLE_SPEED;
      right_speed = -WOBBLE_SPEED;
    }
    else if (i>=3*WOBBLE_STEPS/4) {  
      /* Turn left */
      left_speed  = -WOBBLE_SPEED;
      right_speed = WOBBLE_SPEED;
    }
    sensors=0;
    for (j=0;j<6;j++) 
      if (khepera_get_proximity(j)>IR_THRES_OBSTR) sensors++;
    if (sensors>2) large++; /* obstacle seen by three sensors */
    else large = 0;

    if (large>SENSIBILITY_THRESHOLD) {
      STICK_FOUND = 0;      /* mistake, there was no (single) stick */
      break; 
    }
    khepera_set_speed(LEFT,left_speed);    
    khepera_set_speed(RIGHT,right_speed);
    my_khepera_step(); 
  }
}


/*******************************************************************************/
void obstacle_avoidance(int n_steps)
{
  int i;
  int32 r,l,left_speed,right_speed;

  for(i=0;i<n_steps;i++) {
    khepera_get_virtual_IRDA();
    l = khepera_get_proximity(0)+khepera_get_proximity(1)+2*khepera_get_proximity(2);
    r = 2*khepera_get_proximity(3)+khepera_get_proximity(4)+khepera_get_proximity(5);

    if (r+l < 200) {
      /* Nothing visible with proximity IR */
      if ((VIRTUAL_IRDA_RECEIVE[1]+
	  VIRTUAL_IRDA_RECEIVE[3] <20) | (i<n_steps/4)) { /* No lateral IRDA either */
	left_speed  = FORWARD_SPEED;
	right_speed = FORWARD_SPEED;
      }
      else {
	/* perform phototaxis to IRDA emission */
	left_speed  = 2-(VIRTUAL_IRDA_RECEIVE[1]-VIRTUAL_IRDA_RECEIVE[3])/200;
	right_speed = 2+(VIRTUAL_IRDA_RECEIVE[1]-VIRTUAL_IRDA_RECEIVE[3])/200;
      }
    } 
    else {
      if (l>r) {
        left_speed  =  (l-r)/30;
        right_speed = -(l-r)/30;
      } 
      else {
        left_speed  = -(r-l)/30;
        right_speed =  (r-l)/30;
      }  

      if (left_speed > FORWARD_SPEED) left_speed = FORWARD_SPEED;
      else if (left_speed < -FORWARD_SPEED) left_speed = -FORWARD_SPEED;
      if (right_speed > FORWARD_SPEED) right_speed = FORWARD_SPEED;
      else if (right_speed < -FORWARD_SPEED) right_speed = -FORWARD_SPEED;
    }
    khepera_set_speed(LEFT,left_speed);     /* Set the motor speeds */
    khepera_set_speed(RIGHT,right_speed);
    my_khepera_step(); 
  }
}

/*******************************************************************************/
void grip_stick()
{
  uint16 i,j,up,cooperation;


  cooperation=0;

  /**** Move backwards ****/
  khepera_set_speed(LEFT,-BACKWARDS_SPEED);   
  khepera_set_speed(RIGHT,-BACKWARDS_SPEED);
  for (i=0;i<BACK_GRIP_STEPS;i++) { 
    if ((khepera_get_proximity(6)>600) | (khepera_get_proximity(7)>600)) {
      turn_right(TURN_AWAY_STEPS+(random()%10)-5);
      obstacle_avoidance(N_STEPS_OBST_AVOID);
      return; /* If obstacle behind: exit this routine, go back to searching */
    }
    my_khepera_step();
  }
  khepera_set_speed(LEFT,0);    
  khepera_set_speed(RIGHT,0);

  /**** Start gripping ****/
  gripper_set_arm(ELEV_MIN);  /* arm down */
  for(i=0;i<33;i++) my_khepera_step();
  /* Query: ask supervisor the status of the stick in front of the robot */
  MESSAGE='Q';
  my_khepera_step();
  khepera_receive_serial(BUFFER_SIZE,BUFFER);
  ANSWER=BUFFER[0];

  /*************** Grip 1 ***************/
  if ((ANSWER=='0') | (ANSWER=='1')) { /* ANSWER =='0' if another robot 
					    has just released the stick */
    /**** Grip1: virtual grip ****/
    for(i=0;i<66;i++) {
      if (i==0) {
	gripper_disable_gripping(); 
	gripper_set_grip(1);
      }
      if (i==32) {
	MESSAGE='G';
	VIRTUAL_IRDA_SEND[0]=100;
	/* Tell the supervisor that the stick is gripped */
      }
      else if (i==33)
      {
        gripper_set_arm(ELEV_GRIP);  /* arm up */
      }	
      my_khepera_step();
    }

    /**** Grip1: wait GRIPPING_TIME before releasing ****/
    for (i=0;i<GRIPPING_TIME;i++) {
      khepera_get_virtual_IRDA();  
      /* Query: ask supervisor the status of the stick */
      MESSAGE='Q';
      my_khepera_step();
      khepera_receive_serial(BUFFER_SIZE,BUFFER);
      ANSWER=BUFFER[0];

      /* Stop gripping if a second robot has gripped the same stick */ 
      if (ANSWER=='3') break;

      my_khepera_step();  
    }

    /**** Grip1: Release ****/
    /* Tell supervisor that a stick is released (virtual release, end of grip1) */
    gripper_set_grip(0);  
    MESSAGE='R';
    VIRTUAL_IRDA_SEND[0]=0;
    my_khepera_step();
    for(i=0;i<16;i++) {
      if (i==0) gripper_set_arm(ELEV_MAX);   /* arm up*/
      my_khepera_step();
    }
  }
  /************** Grip 2 ****************/
  else if (ANSWER=='2') {
    /**** Grip2: real grip ****/
    for(i=0;i<33;i++) {
      if (i==0) {
	/* virtual grip for one step to give time to the other robot
	to open its gripper*/
	gripper_disable_gripping();   
	gripper_set_grip(1); 
      } 
      if (i==31) {
	/* Tell the supervisor that the stick is gripped */
	MESSAGE='G';
      }
      if (i==32) {
	/* IRDA signalling: emit in the frontal direction */
	VIRTUAL_IRDA_SEND[0]=100; 
	gripper_enable_gripping();  
	gripper_set_grip(1);   /* grip the stick */
      }
      my_khepera_step();
    }
    
    /**** Grip2: wait GRIPPING_TIME before releasing ****/
    up=0;
    for (i=0;i<GRIPPING_TIME;i++) {
      my_khepera_step();  
      if (up==0) {
	/* Query: ask supervisor the status of the stick */
	MESSAGE='Q';
	my_khepera_step();
	khepera_receive_serial(BUFFER_SIZE,BUFFER);
	ANSWER=BUFFER[0];

	/* Lift completely, if grip 1 has ceased */
	if ((ANSWER=='4') | (ANSWER=='1')) { /* ANSWER=='1' if stick just released by the other robot  */
	  up=1;
	  /* arm up, SUCCESSFUL COOPERATION */
	  for(j=0;j<20;j++) {
	    if (j==0) gripper_set_arm(ELEV_MAX-10);   
	    my_khepera_step();
	  }	
	  cooperation=1;
	  break;    /* do not wait anymore */
	}
      }
    }
    /**** Grip2: if cooperation, make success dance ***/
    if (cooperation) {
      VIRTUAL_IRDA_SEND[0]=0;
      for(j=0;j<100;j++) {
	if (j==20) gripper_set_arm(ELEV_MAX+20);  
	if (j==40) gripper_set_arm(ELEV_MAX-10);  
	if (j==60) gripper_set_arm(ELEV_MAX+20);  
	if (j==80) gripper_set_arm(ELEV_MAX-10);  
	my_khepera_step();
      }	
    }

    /**** Grip2: release ****/
    for(i=0;i<45;i++) {
      if (i==0)       gripper_set_arm(ELEV_GRIP+10);  /* arm partially down */
      else if (i==15) {
	gripper_set_grip(0);   /* release the stick */
	/* Tell supervisor that a stick is released (end of grip2)*/
	MESSAGE='R';
	VIRTUAL_IRDA_SEND[0]=0;
      }
      else if (i==30) gripper_set_arm(ELEV_MAX);  /* arm up */

      my_khepera_step();
    }
  }
  else {
    /**** Not correct message from supervisor, abandon ****/
    gripper_set_arm(ELEV_MAX); 
  }


  /**** After both grips:  turn away and obst. avoidance for a while ****/
  turn_right(TURN_AWAY_STEPS+(random()%10)-5);
  obstacle_avoidance(N_STEPS_OBST_AVOID);
}

/*********************************************************************************/
void my_khepera_step(){

  sprintf(BUFFER,"%c| %5d %5d %5d %5d",MESSAGE,VIRTUAL_IRDA_SEND[0],VIRTUAL_IRDA_SEND[1],VIRTUAL_IRDA_SEND[2],VIRTUAL_IRDA_SEND[3]);

  khepera_send_serial(BUFFER_SIZE,BUFFER);

  khepera_step(64);
 
  MESSAGE='-';
}


/*********************************************************************************/
void main()
{
  int k;
  khepera_live();
  khepera_enable_proximity(IR_ALL_SENSORS);
  gripper_disable_gripping();
  gripper_enable_presence();

  for (k=0;k<4;k++) VIRTUAL_IRDA_SEND[k]=0;

  for(;;) {                                
    search_stick();                           /* Search for stick */
    if (STICK_FOUND) grip_stick();
  }
}
