/*****************************************************************************/
/* File:         predator.c                                                  */
/* Version:      1.0                                                         */
/* Date:         07-Oct-98                                                   */
/* Description:  Predator in the prey - predator experiment.                 */
/*               This program was adapted from Dario Floreano's program      */
/* Author:       Olivier.Michel@cyberbotics.com                              */
/*                                                                           */
/* Copyright (c) 1998 Cyberbotics -  www.cyberbotics.com                     */
/*****************************************************************************/

#include <Khepera.h>
#include <KheperaK213.h>
#include <gui.h>
#include <stdio.h>
#include <math.h>

#define SYNAPSE_LENGTH    5 /* binary length for one synapse */
#define PROXIMITY         8
#define CAMERA	 	  5 /* 5 photo-sensors on a linear camera */
#define HIDDEN            2
#define INPUT             ((PROXIMITY)+(CAMERA)+(HIDDEN))
#define N_SYNAPSES        ((INPUT)*(HIDDEN)+(HIDDEN))
#define CHROMOSOME_SIZE   ((SYNAPSE_LENGTH)*(N_SYNAPSES))
#define	SENSCALE          1024.0
#define	MOTSCALE          20.0

int    chromosome[CHROMOSOME_SIZE];
int    w1sign[(INPUT)*(HIDDEN)];
float  w1[(INPUT)*(HIDDEN)];
int    bhsign[HIDDEN];
float  bh[HIDDEN];
float  inp[INPUT];
float  hid[HIDDEN];
int    distance[PROXIMITY];
uint8 *camera;
int    left_speed;
int    right_speed;
bool   braiten=false;
int    generation = 0;
int    seed = 1;

gui_window    window=NULL;
gui_textfield file_number_tf=NULL;
gui_textfield gen_number_tf=NULL;
gui_button    reload=NULL;

/* load a chromosome from a file */
void load()
{
  char filename[256];
  int i,j;
  FILE *file;
  
  sprintf(filename,"best_P.%d",seed);
  file = fopen(filename,"r");
  if (file==NULL) fprintf(stderr,"predator: %s file not found\n",filename);
  for(i=0;i<=generation;i++)
  {
    fscanf(file,"%s",filename); // skip the generation number
    for(j=0;j<CHROMOSOME_SIZE;j++) fscanf(file,"%d",&chromosome[j]);
  }
  fclose(file);
}

/**********************************************************************/
/* It returns the decoded value of a binary string p  starting at     */
/* position init for length n                                         */
/**********************************************************************/
int decode(int *p, int init, int n)
{
  int j,l=0,decoded=0,power;

  for(j=init;j<(init+n); j++)
  { 
    if (l==0) power=1; else power=1<<l;
    decoded+=p[j]*power;
    l++;
  }
  return decoded;
}

void reset_network()
{
  int i;
  
  for(i=0;i<INPUT;i++)  inp[i]=0.0;
  for(i=0;i<HIDDEN;i++) hid[i]=0.0;
}

void print_network()
{
  int i;
  
  for(i=0;i<INPUT;i++)
  {
    printf("I%d[%g]: ",i,inp[i]);
    if (w1sign[2*i]==-1) printf("-");
    printf("%g ",w1[2*i]);
    
    if (w1sign[2*i+1]==-1) printf("-");
    printf("%g\n",w1[2*i+1]);
  }
  printf("B: ");
  if (bhsign[0]==-1) printf("-");
  printf("%g ",bh[0]);
  if (bhsign[1]==-1) printf("-");
  printf("%g\n",bh[1]);
  printf("H0[%g]\n",hid[0]);
  printf("H1[%g]\n",hid[1]);
}

void create_network()
{
  int i,j=0;
  for(i=0;i<(HIDDEN*INPUT);i++)
  {
    w1sign[i] = decode(chromosome,j*SYNAPSE_LENGTH,1)*2-1;
    w1[i]     = (float)(decode(chromosome,j*SYNAPSE_LENGTH+1,4))/16.0;
    j++;
  }
  for(i=0;i<HIDDEN;i++)
  {
    bhsign[i] = decode(chromosome,(j*SYNAPSE_LENGTH),1)*2-1;
    bh[i]     = (float)(decode(chromosome,(j*SYNAPSE_LENGTH)+1,4))/16.0;
    j++;
  }
  reset_network();
}

float logistic(float b)
{
  return (float)(1.0/(1.0 + exp((double)-b)));
}

void propagate()
{
  int i,j;
  float buffer;

  for(i=0;i<HIDDEN;i++)
  {
    buffer=0.0;
    for(j=0;j<INPUT;j++) buffer+=inp[j]*w1[INPUT*i+j]*w1sign[INPUT*i+j];
    buffer   += bh[i]*bhsign[i];
    hid[i]    = logistic(buffer);
  }
}

void init_input()
{
  int i,j;
  
  for(i=0;i<(PROXIMITY);i++) inp[i]=(float)(distance[i])/SENSCALE;
  for(i=PROXIMITY;i<PROXIMITY+CAMERA;i++) inp[i]=0.0;                  // set to 0 every camera neuron
  for(i=0;i<K213_WIDTH;i++) if (camera[i]<100) { inp[PROXIMITY+i/13]=1.0; break; }// set to 1 if a dark thing is detected
//  for(i=PROXIMITY;i<PROXIMITY+CAMERA;i++) if (inp[i]==0.0) printf("."); else printf("X");
//  printf("\n");
  j=0;
  for(i=INPUT-HIDDEN;i<INPUT;i++) // two memory neurons
  {
    inp[i]=hid[j];
    j++;
  }
}

void init_output()
{
  left_speed  = (MOTSCALE*((hid[0]-0.5)*2));
  right_speed = (MOTSCALE*((hid[1]-0.5)*2));
}

void gui_callback()
{
  if (gui_event_get_info()==GUI_WIDGET)
   if (gui_event_get_widget()==reload)
   {
     sscanf(gui_textfield_get_text(file_number_tf),"%d",&seed);
     sscanf(gui_textfield_get_text(gen_number_tf),"%d",&generation);
     load();
     create_network();
     gui_button_desactivate(reload);
   }
}

void create_gui()
{
  window = gui_window_new("Predator",30,30,160,100);
  gui_label_new(window,20,25,"Seed:");
  gui_label_new(window,20,50,"Generation:");
  file_number_tf = gui_textfield_new(window,100,10,"1",3);
  gen_number_tf  = gui_textfield_new(window,100,35,"0",3);
  reload         = gui_button_new(window,55,70,"reload");
  gui_event_callback(gui_callback);
  gui_window_show(window);
}

void do_step()
{
  char serial[32];
  
  khepera_step(64);
  if (khepera_receive_serial(32,serial))
  {
    switch(serial[0])
    {
      case 'b': braiten = true;
                break;
      case 's': sscanf(&serial[1],"%d",&seed);
                if (file_number_tf)
		 gui_textfield_set_text(file_number_tf,&serial[1]);
                break;
      case 'g': sscanf(&serial[1],"%d",&generation);
                if (gen_number_tf)
		 gui_textfield_set_text(gen_number_tf,&serial[1]);
                load();
		create_network();
		braiten = false;
		break;
    }
  }
}

void do_braiten()
{
  static const int32 Interconn[16] = {-5,-15,-18,6,4,4,3,5,4,4,6,-18,-15,-5,5,3};	
  int i;
  
  left_speed  = 0;
  right_speed = 0;
  for(i=0; i<8; i++)
  {                    /* Connections sensors-motors */
    right_speed += distance[i] * Interconn[i];
    left_speed  += distance[i] * Interconn[8+i];
  }
  left_speed  /= 400;                     /* Global gain */
  right_speed /= 400;
  left_speed  += 10;                      /* Offset */
  right_speed += 10;
}

int main()
{
  int32 i;
  
  khepera_live();
  // create_gui(); /* you can uncomment this to add a GUI interface */
  load();
  create_network();
  khepera_enable_proximity(IR_ALL_SENSORS); /* enable proximity measures */
  k213_enable();                            /* enable linear camera */
  for(;;)                                   /* Khepera never dies! */
  {
    do_step();
    camera = k213_get_image();
    for(i=0; i<8; i++) distance[i] = khepera_get_proximity(i);
    if (braiten) do_braiten();
    else
    {
      init_input();
      propagate();
      init_output();
    }
    khepera_set_speed(LEFT,left_speed);     /* Set the motor speeds */
    khepera_set_speed(RIGHT,right_speed);
    do_step(); // doing two Khepera step for one cycle => 128 ms
  }
}

