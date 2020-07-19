/*****************************************************************************/
/* File:         can.c                                                       */
/* Version:      1.0                                                         */
/* Date:         09-Apr-98                                                   */
/* Description:  Simple example of using Khepera API with the gripper module */
/* Author:       Olivier.Michel@cyberbotics.com                              */
/*                                                                           */
/* Copyright (c) 1998 Cyberbotics - www.cyberbotics.com                      */
/*****************************************************************************/

#include <Khepera.h>
#include <KheperaGripper.h>

void turn_left(int v)
{
  int i;
  for(i=0;i<v;i++)
  {
    khepera_set_speed(LEFT,8);
    khepera_set_speed(RIGHT,-8);
    khepera_step(64);
  }
}

void turn_right(int v)
{
  int i;
  for(i=0;i<v;i++)
  {
    khepera_set_speed(LEFT,-8);
    khepera_set_speed(RIGHT,8);
    khepera_step(64);
  }
}

void search_can() /* return true when a can is found */
{
  int32 r,l,left_speed,right_speed;
  
  for(;;)
  {
    l = khepera_get_proximity(0)
      + khepera_get_proximity(1)
      + khepera_get_proximity(2);
    r = khepera_get_proximity(3)
      + khepera_get_proximity(4)
      + khepera_get_proximity(5);

    if (r+l < 200)
    {
      left_speed  = 20;
      right_speed = 20;
    }
    else
    {
      if (l>r)
      {
        left_speed  = -(l-r)/30;
        right_speed =  (l-r)/30;
      }
      else
      {
        left_speed  =  (r-l)/30;
        right_speed = -(r-l)/30;
      }
      if (left_speed > 20) left_speed = 20;
      else if (left_speed < -20) left_speed = -20;
      if (right_speed > 20) right_speed = 20;
      else if (right_speed < -20) right_speed = -20;
    }
    if ((left_speed==0) && (right_speed==0))
    {
      if ((khepera_get_proximity(1)<10)&&(khepera_get_proximity(4)<10))
      {
        if ((khepera_get_proximity(2)>600)&&(khepera_get_proximity(3)>600))
         break;
	 /* can found */
        else
        {
          left_speed  = 3; /* approach */
          right_speed = 3;
        }
      }
      else if (l>r) /* obstacle on the left, turn right */
      {
        turn_right(15);
        continue;
      }
      else /* obstacle on the right, turn left */
      {
        turn_left(15);
        continue;
      }
    }
    khepera_set_speed(LEFT,left_speed);     /* Set the motor speeds */
    khepera_set_speed(RIGHT,right_speed);
    khepera_step(8);
  }
}

void grip_can()
{
  uint16 i;
  
  khepera_set_speed(LEFT,-10);   /* move a little bit backwards from the can */
  khepera_set_speed(RIGHT,-10);
  for(i=0;i<4;i++) khepera_step(64);
  
  khepera_set_speed(LEFT,0);     /* stop */
  khepera_set_speed(RIGHT,0);
  
  for(i=0;i<45;i++)
  {
    if (i==0)       gripper_set_arm(255);  /* arm down */
    else if (i==15) gripper_set_grip(1);   /* grip the can */
    else if (i==30) gripper_set_arm(152);  /* arm up */
    khepera_step(64);
  }
}

void release_can()
{
  uint16 i;
  
  turn_left(10);                   /* put it on the left */
  khepera_set_speed(LEFT,0);     /* stop */
  khepera_set_speed(RIGHT,0);
  for(i=0;i<45;i++)
  {
    if (i==0)       gripper_set_arm(255);  /* arm down */
    else if (i==15) gripper_set_grip(0);   /* release the can */
    else if (i==30) gripper_set_arm(152);  /* arm up */
    khepera_step(64);
  }
  turn_right(10);                  /* come back to initial orientation */
}

int main()
{
  khepera_live();
  khepera_enable_proximity(IR_ALL_SENSORS);
  gripper_enable_presence();
  search_can();                             /* find a first can       */
  grip_can();                               /* grasp it               */
  for(;;)
  {                                         /* Khepera never dies!    */
    search_can();                           /* search for another can */
    release_can();                          /* release the can held   */
    grip_can();                             /* grasp the other can    */
  }
  return 0; /* this statement is never reached */
}
