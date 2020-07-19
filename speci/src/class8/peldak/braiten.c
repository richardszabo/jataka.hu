/*****************************************************************************/
/* File:         braiten.c                                                   */
/* Version:      1.0                                                         */
/* Date:         07-Oct-98                                                   */
/* Description:  Simple example of using Khepera API.                        */
/*               This program was adapted from the Braitenberg program       */
/*               written by Francesco Mondada (Francesco.Mondada@di.epfl.ch) */
/*               for the Khepera BIOS.                                       */
/*               This program implements a simple Braitenberg vehicle able   */
/*               to move around while avoiding obstacles.                    */
/* References:   Valentino Braitenberg. Vehicles: Experiments in Synthetic   */
/*               Psychology. MIT Press, Cambridge, 1984.                     */
/* Author:       Olivier.Michel@cyberbotics.com                              */
/*                                                                           */
/* Copyright (c) 1998 Cyberbotics -  www.cyberbotics.com                     */
/*****************************************************************************/

#include <Khepera.h>
#include "braiten.h"

/* interconnection matrix between IR proximity sensors and motors */
int32 Interconn[16] = {-5,-15,-18,6,4,4,3,5,4,4,6,-18,-15,-5,5,3};	

int main()
{
  int32 i,left_speed,right_speed;

  khepera_live();
  khepera_enable_proximity(IR_ALL_SENSORS); /* enable proximity measures */
  for(;;)                                   /* Khepera never dies! */
  {
    left_speed  = 0;
    right_speed = 0;
    for(i=0; i<8; i++) {                    /* Connections sensors-motors */
      right_speed += khepera_get_proximity(i) * Interconn[i];
      left_speed  += khepera_get_proximity(i) * Interconn[8+i];
    }

    left_speed  /= 400;                     /* Global gain */
    right_speed /= 400;
    left_speed  += FORWARD_SPEED;           /* Offset */
    right_speed += FORWARD_SPEED;

    khepera_set_speed(LEFT,left_speed);     /* Set the motor speeds */
    khepera_set_speed(RIGHT,right_speed);
    khepera_step(64);                       /* run one step */
  }
  return 0;
}
