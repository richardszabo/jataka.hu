/*****************************************************************************/
/* File:         finder.c                                                    */
/* Version:      1.0                                                         */
/* Date:         19-Nov-97                                                   */
/* Description:  Simple example of using Khepera API with the panoramic      */
/*               turret.                                                     */
/* Author:       Olivier Michel Olivier.Michel@cyberbotics.com               */
/*                                                                           */
/* Copyright (c) 1998 Olivier Michel                                         */
/* Cyberbotics Ltd.                                                          */
/* Ch. de Valrose 1                                                          */
/* 1305 Penthalaz - Switzerland                                              */
/*****************************************************************************/

#include <Khepera.h>
#include <KheperaPanoramic.h>

#define FORWARD_SPEED 20	/* default forward speed */

int32	Interconn[16] = {-5,-15,-18,6,4,4,3,5,4,4,6,-18,-15,-5,5,3};	
	/* interconnection matrix between IR proximity sensors and motors */

main()
{
  int32 i,left_speed,right_speed,min_i,max_i;
  uint8 min;

  khepera_live();
  khepera_enable_proximity(IR_ALL_SENSORS);
  panoramic_enable_raw();
  for(;;)                                 /* Khepera never dies! */
  {
    left_speed  = 0;
    right_speed = 0;
    for(i=0; i<8; i++)                    /* Connections sensors-motors */
    {
      right_speed += khepera_get_proximity(i)*Interconn[i];
      left_speed  += khepera_get_proximity(i)*Interconn[8+i];
    }
    left_speed  /= 400;                   /* Global gain */
    right_speed /= 400;
    left_speed  += FORWARD_SPEED;         /* Offset */
    right_speed += FORWARD_SPEED;
    if (right_speed==left_speed)
    {
      min = 255;
      for(i=0;i<150;i++)
      {
        if (panoramic_get_raw(i)<min)
        {
          min = panoramic_get_raw(i);
          min_i = i;
          max_i = i;
        }
        else if (panoramic_get_raw(i)==min) max_i = i;
      }
      if (max_i - min_i < 10)
      {
        if (max_i+min_i < 140)
        {
          left_speed  = -10;
          right_speed =  10;
        }
        else if (max_i+min_i > 160)
        {
          left_speed  =  10;
          right_speed = -10;
        }
      }
    }
    khepera_set_speed(LEFT,left_speed);     /* Set the motor speeds */
    khepera_set_speed(RIGHT,right_speed);
    khepera_step(64);
  }
}
