/*****************************************************************************/
/* File:         phototaxy.c                                                 */
/* Version:      2.0                                                         */
/* Date:         21-Apr-99                                                   */
/* Description:  Simple example of using Khepera API with the infrared light */
/*               measurements: like some insects, the robot is attracted by  */
/*               lights.                                                     */
/* Author:       Olivier.Michel@cyberbotics.com                              */
/*                                                                           */
/* Copyright (c) 1998 Cyberbotics - www.cyberbotics.com                      */
/*****************************************************************************/

#include <Khepera.h>

#define FORWARD_SPEED 20

int32	Interconn[16] = {-5,-15,-18,6,4,4,3,5,4,4,6,-18,-15,-5,5,3};	
	/* interconnection matrix between IR proximity sensors and motors */

main()
{
  int32 i,left_speed,right_speed;

  khepera_live();
  khepera_enable_proximity(IR_ALL_SENSORS);
  khepera_enable_light(IR_ALL_SENSORS);
  for(;;)
  {                                         /* Khepera never dies! */
    left_speed  = 0;
    right_speed = 0;                        /* Braitenberg obstacle avoidance */
    for(i=0; i<8; i++)
    {                                       /* Connections sensors-motors */
      right_speed += khepera_get_proximity(i) * Interconn[i];
      left_speed  += khepera_get_proximity(i) * Interconn[8+i];
    }
    left_speed  /= 400;                     /* Global gain */
    right_speed /= 400;
    left_speed  += FORWARD_SPEED;           /* Offset */
    right_speed += FORWARD_SPEED;

    if ((right_speed>=15)&&(left_speed>=15))
    {
      for(i=0;i<8;i++)
      {           /* here, we assume no obstacle are in front of the robot */
        right_speed += khepera_get_light(i) * Interconn[i];
        left_speed  += khepera_get_light(i) * Interconn[i+8];
      }
      left_speed  /= 800;                     /* Global gain */
      right_speed /= 800;
      left_speed  += FORWARD_SPEED;           /* Offset */
      right_speed += FORWARD_SPEED;
    }

    khepera_set_speed(LEFT,left_speed);     /* Set the motor speeds */
    khepera_set_speed(RIGHT,right_speed);
    khepera_step(64);                       /* run one step */
  }
}
