/*****************************************************************************/
/* File:         jumper.c                                                    */
/* Version:      1.0                                                         */
/* Date:         28-Apr-98                                                   */
/* Description:  Very Simple example of using Khepera API.                   */
/*               This program reads the jumpers of the Khepera and turn on   */
/*               and off the LEDs of the robot.                              */
/* Author:       Olivier.Michel@cyberbotics.com                              */
/*                                                                           */
/* Copyright (c) 1998 Cyberbotics -  www.cyberbotics.com                     */
/*****************************************************************************/

#include <Khepera.h>

int main()
{
  uint8 j;
  bool left_on  = false;
  bool right_on = false;

  khepera_live();
  khepera_enable_jumper();
  for(;;)                                   /* Khepera never dies! */
  {
    j = khepera_get_jumper();
    if (j&MIDDLE_JUMPER)
    {
      if (left_on==false)
      {
        khepera_set_led(LEFT,ON);
        left_on = true;
      }
    }
    else if (left_on==true)
    {
      khepera_set_led(LEFT,OFF);
      left_on = false;
    }
    if (j&OUTER_JUMPER)
    {
      if (right_on==false)
      {
        khepera_set_led(RIGHT,ON);
        right_on = true;
      }
    }
    else if (right_on==true)
    {
      khepera_set_led(RIGHT,OFF);
      right_on = false;
    }
    khepera_step(64);
  }
  return 0;
}
