/*****************************************************************************/
/* File:         alice.c                                                     */
/* Version:      1.0                                                         */
/* Date:         19-Mar-99                                                   */
/* Description:  Supervisor for the Alice robots                             */
/* Author:       Olivier.Michel@cyberbotics.com                              */
/*****************************************************************************/

#include <stdio.h>
#include <Supervisor.h>
#include <Alice.h>
#include <eai.h>
#include <gui.h>
#include <fcntl.h>
#include <math.h>

#define N_ROBOT 3

eai_node alice[N_ROBOT];

gui_window     window;
gui_button     forwardButton;
gui_button     backwardButton;
gui_button     rightButton;
gui_button     leftButton;
gui_button     sensorsOffButton;
gui_button     sensorsOnButton;
gui_button     stopButton;
gui_button     followRightTurnButton;
gui_button     followLeftTurnButton;
gui_button     followRightButton;
gui_button     followLeftButton;
gui_button     stepRightMotorButton;
gui_button     stepLeftMotorButton;
gui_textfield  stepRightMotorTextfield;
gui_textfield  stepLeftMotorTextfield;
gui_popup      robotIdPopup;
gui_button     activeButton;

void gui_callback()
{
  gui_widget w;
  if (gui_event_get_info()==GUI_WIDGET)
  {
    w = gui_event_get_widget();
    if (w!=robotIdPopup) activeButton = w;
  }
}

void create_window()
{
  static char *popupId[]
   = {"0","1","2","3","4","5","6","7"};

  window = gui_window_new("Alice",30,30,150,150);
  gui_label_new(window,10,20,"Robot identifier:");
  robotIdPopup     = gui_popup_new(window,100,5,N_ROBOT,popupId,0);
  forwardButton    = gui_button_new(window,10,30,"Forward");
  backwardButton   = gui_button_new(window,10,50,"Backward");
  rightButton      = gui_button_new(window,10,70,"Right");
  leftButton       = gui_button_new(window,10,90,"Left");
  stopButton       = gui_button_new(window,10,110,"Stop");
  gui_button_set_width(forwardButton,70);
  gui_button_set_width(backwardButton,70);
  gui_button_set_width(rightButton,70);
  gui_button_set_width(leftButton,70);
  gui_button_set_width(stopButton,70);

  gui_event_callback(gui_callback);
  gui_window_show(window);
}

int8 getCommand()
{
  if (activeButton==forwardButton)    return 1;
  if (activeButton==backwardButton)   return 2;
  if (activeButton==rightButton)      return 3;
  if (activeButton==leftButton)       return 4;
  if (activeButton==sensorsOffButton) return 5;
  if (activeButton==sensorsOnButton)  return 6;
  if (activeButton==stopButton)       return 8;
  return 0;
}

main()
{
  char string[16];
  int  i,id;
  uint8 n=N_ROBOT;
  uint8 r,c,p,l,*a;
  float32 x,y,theta;

  supervisor_live();
  create_window();
  /* Get the identifiers of the robots */
  for(i=0;i<N_ROBOT;i++)
  {
    sprintf(string,"Alice%d",i);
    alice[i] = eai_get_node(string);
  }
  for(;;)
  {
    if (activeButton)
    {
      id = gui_popup_get_selection(robotIdPopup);
      string[0]=(id<<4)|getCommand();
      string[1]=0; // parameter
      eai_write_stream(alice[id],string,1);
      gui_button_desactivate(activeButton);
      activeButton = NULL;
    }
    supervisor_step(64);
  }
}
