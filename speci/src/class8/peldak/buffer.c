/*****************************************************************************/
/* File:         buffer.c                                                    */
/* Version:      1.0                                                         */
/* Date:         01-Jun-99                                                   */
/* Description:  Simple example of using Khepera API to communicate with the */
/*               supervisor API.                                             */
/* Author:       Olivier.Michel@cyberbotics.com                              */
/*                                                                           */
/* Copyright (c) 1999 Cyberbotics -  www.cyberbotics.com                     */
/*****************************************************************************/

#include <Khepera.h>
#include <stdio.h>

int main()
{
  char  message[40];
  char  answer[10];
  uint32 l;
  uint16 p;

  khepera_live();
  khepera_enable_proximity(3); /* enable the front right sensor */
  for(;;)                                   /* Khepera never dies! */
  {
    khepera_step(64);                       /* run one step */
    l = khepera_receive_serial(10,answer);  /* wait until a message from the supervisor is received */
    if (l>0)
    {
      switch(answer[0])
      {
        case '1':
          khepera_set_led(LEFT,ON);
          break;
        case '0':
          khepera_set_led(LEFT,OFF);
          break;
      }
    }
    p = khepera_get_proximity(3);           /* get the proximity value for the sensor 3 */
    sprintf(message,"%d",p);
    khepera_send_serial(strlen(message)+1,message); /* send it to the supervisor */
  }
  return 0;
}
