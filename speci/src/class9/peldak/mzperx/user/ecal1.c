/*****************************************************************************/
/*****************************************************************************/
/*
    modifications:
    //- turn_to_feeder disappeared and because of the permutation of the action-selection
    //  branches wall_following replaced this module
    - search_for_feeder is smarter it takes into account the direction of the last-seen feeder
    - search_for_feeder takes place after every wall_following and feeder_approach so as to 
      keep the feeder in sight 
    - feeder_approach turns harder if the feeder is far

    - starting module is search_for_feeder
    - closer middle-wall-following with higher threshold
    - modified feeder_approach 
    - modified middle-wall-following  
    - search_for_feeder starting direction is random

*/

#include <Khepera.h>
#include <KheperaK6300.h>
#include <gui.h>
#include <math.h>

//#include "ecal1.h"

#define EPSILON 0.00001;

/* object types on screen */
#define NILL     0
#define FOOD     1

/* object colors on screen */
int16 food[3]={0,255,0};


/* module names */
#define MODULE_EMPTY		 0
#define MODULE_NORMAL_MOVE	 1
#define MODULE_FEEDER_APPROACH	 2
//#define MODULE_TURN_TO_FEEDER 	 3
#define MODULE_SEARCH_FOR_FEEDER 4
#define MODULE_WALL_FOLLOWING	 5
#define MODULE_ESCAPE		 6

int module_still_active = MODULE_SEARCH_FOR_FEEDER;   // starting module

/* obstacle avoidance vector */
int32	Interconn[16] = {-5,-15,-18,6,4,4,3,5,4,4,6,-18,-15,-5,5,3};	
	/* interconnection matrix between IR proximity sensors and motors */

//int32	Interconn_food[10] = {-10,-15,-18,-18,-15,-5};	
//	/* interconnection matrix between food sensors and motors*/

/* elapsed time from last feeder chasing */
int last_feeder_seen = 0;
int last_feeder_seen_after_others = 0;

/* actual speed of the simulation according to khepera_fair_step() */
double sim_speed = 64.0;

bool trapped = false;

/* last direction where feeder was seen */
int last_feeder_dir = LEFT;
/* last direction where feeder was seen close */
int last_close_feeder_dir = LEFT_AND_RIGHT;  // no normal default value 

/* speed names */
int32 MAX_FORWARD_SPEED		= 20;
int32 TURNING_SPEED 		= 10;
int32 WALL_FOLLOWING_SPEED 	= 15;
int32 FULL_TURN  		= 21;

#define IMAGE_SCALE 2
#define TOLERANCE 50              //tolerance on color detection

/* gui definitions */
gui_window window1,window2;
gui_rectangle picture1[K6300_WIDTH][K6300_HEIGHT];
gui_rectangle picture2[K6300_WIDTH];
gui_checkbox imageCheckBox;

/* results of the image processing is in this array */
uint8 myImage[K6300_WIDTH];

/* filtered sensor values */
int32 left_bumper_stimuli,right_bumper_stimuli;
int32 left_food_stimuli,right_food_stimuli;

/* actual motor values */
int32 left_speed,right_speed;

/* constants */
static const int FOOD_THRESHOLD = 20;
static const int HIGH_BUMPER_THRESHOLD = 1200;
static const int BUMPER_THRESHOLD = 700;
static const int LOW_BUMPER_THRESHOLD = 300;
static const int LOW_BUMPER_THRESHOLD_BIAS = 200;
static const int FEEDER_SEARCH_START_THRESHOLD = 3000;
static const int FEEDER_SEARCH_START_AFTER_OTHERS_THRESHOLD = 200;
static const int WALL_FOLLOWING_START_THRESHOLD = 1500;
static const int WALL_FOLLOWING_THRESHOLD = 1200;
static const int WALL_FOLLOWING_TURNS = 3;


/*******************************************************************************************/
/****************************************************************************************/

#define min(a,b) (a<b)?a:b
#define max(a,b) (a>b)?a:b

float  drand (){ return((float)rand()/(float)RAND_MAX); }
/****************************************************************************************/
/*******************************************************************************************/
// unfortunatelly the returning value is not signed long but unsigned int
// so I convert it to signed int which is a subset of signed long
int32 my_khepera_get_position(uint8 side) {
    int32 value = khepera_get_position(side);
    if( value > 32768 ) {
	return value - 65536;
    } else {
	return value;
    }
}

/****************************************************************************************/
/*******************************************************************************************/
// image creation procedures, not related to the behaviour of the webot
void create_camera_image()
{
  uint16 x,y,x_rectangle, y_rectangle;
  uint32 color = gui_color_get(0,0,0);
  
  for(x = 0 ; x < K6300_WIDTH ; x++)
  {
    for(y = 0 ; y < K6300_HEIGHT ; y++)
    {
      x_rectangle = 30 + IMAGE_SCALE * x;
      y_rectangle = 30 + IMAGE_SCALE * y;
      picture1[x][y] = gui_rectangle_new(window1,x_rectangle,y_rectangle,IMAGE_SCALE,IMAGE_SCALE,color,1);
    }
    y_rectangle = 30 + IMAGE_SCALE * K6300_WIDTH/2;
    picture2[x] = gui_rectangle_new(window2,x_rectangle,y_rectangle,IMAGE_SCALE,IMAGE_SCALE,color,1);
  }
}

/*******************************************************************************************/

void paint_black_camera_image()
{
  uint16 x,y;
  for(y = 0 ; y < K6300_HEIGHT ; y++) for(x = 0 ; x < K6300_WIDTH ; x++)
   gui_rectangle_change_color(picture1[x][y],GUI_BLACK);
}

/*******************************************************************************************/

void gui_callback()
{
  gui_widget w;
  if (gui_event_get_info()==GUI_WIDGET)
  {
    w = gui_event_get_widget();
    if ((w == imageCheckBox)&&(gui_checkbox_is_active(imageCheckBox)==false)) paint_black_camera_image();
  }
}

/*******************************************************************************************/

void create_gui()
{
  int i;

  window1   = gui_window_new("Eye interface",30,30,220,180);
  window2   = gui_window_new("Eye filtering",30,30,220,180);
  create_camera_image();
  imageCheckBox = gui_checkbox_new(window1,30,10,"show image",true);
  gui_event_callback(gui_callback);
  gui_window_show(window1);
  gui_window_show(window2);
}

/*******************************************************************************************/

void change_camera_image()
{
  uint8 red,green,blue;
  uint16 x,y;
  uint32 color;
  uint8 *image = k6300_get_image();

  for(y = 0 ; y < K6300_HEIGHT ; y++)
  {
    for(x = 0 ; x < K6300_WIDTH ; x++)
    {
      red = k6300_get_red(image,x,y);
      green = k6300_get_green(image,x,y);
      blue = k6300_get_blue(image,x,y);
      color = gui_color_get(red,green,blue);
      gui_rectangle_change_color(picture1[x][y],color);
    }
  }
}

/*******************************************************************************************/

void update_filter_camera()
{
  uint16 x,y;
  uint32 color;

    for(x = 0 ; x < K6300_WIDTH ; x++)
    {
     if (myImage[x]==FOOD) color = gui_color_get(food[0],food[1],food[2]);
     else color = gui_color_get(0,0,0);
     gui_rectangle_change_color(picture2[x],color);
    }
}

/****************************************************************************************/
/*******************************************************************************************/

float propRGB(int16 red, int16 green, int16 blue, int16 *goal)
{
  float prop[3], propG[3];
  float simil, aux;
 
  if (green > 0 || blue > 0) prop[0] = red / (float)(green + blue); else prop[0] = red/255;
  if (red > 0 || blue > 0) prop[1] = green / (float)(red + blue); else prop[1] = green/255;
  if (red > 0 || green > 0) prop[2] = blue / (float)(red + green); else prop[2] = blue/255;
  aux = max(prop[0],prop[1]); aux = max(aux,prop[2]);
  if (aux>0) { prop[0]/=aux; prop[1]/=aux; prop[2]/=aux; }
  else prop[0] = prop[1] = prop[2] = 1;
  
  if (goal[1] > 0 || goal[2] > 0) propG[0] = goal[0] / (float)(goal[1] + goal[2]); else propG[0] = goal[0]/255;
  if (goal[0] > 0 || goal[2] > 0) propG[1] = goal[1] / (float)(goal[0] + goal[2]); else propG[1] = goal[1]/255;
  if (goal[0] > 0 || goal[1] > 0) propG[2] = goal[2] / (float)(goal[0] + goal[1]); else propG[2] = goal[2]/255;
  aux = max(propG[0],propG[1]); aux = max(aux,propG[2]);
  if (aux>0) { propG[0]/=aux; propG[1]/=aux; propG[2]/=aux; }
  else propG[0] = propG[1] = propG[2] = 1;

   simil = 1-(fabs(prop[0]-propG[0]) + fabs(prop[1]-propG[1]) + fabs(prop[2]-propG[2]))/3;    // scaled between (0,1)

  return(simil);
}

/*******************************************************************************************/

bool colorDetected(int16 red, int16 green, int16 blue, int8 target)
{
  bool resp;
  float margin,diff,simil;
  int16 *goal; 
  
  switch (target){
    case FOOD:   	goal = food; break;
  } 
  
  simil = propRGB(red,green,blue,goal);
  diff = min(100,fabs(red - green) + fabs(red - blue) + fabs(green - blue));
  margin = TOLERANCE * ((2 - diff/100) + simil);        // adding some robustness to light intensity 
  resp = (fabs(red - goal[0])<margin) && (fabs(green - goal[1])<margin) && (fabs(blue - goal[2])<margin); // printf("Margin: %2.2f diffs[%2.2f,%2.2f,%2.2f]  ",margin, fabs(red - goal[0]),fabs(green - goal[1]),fabs(blue - goal[2]));
  return(resp);
}

/*******************************************************************************************/

void trackImage()
{
  uint8 red,green,blue;
  uint16 x;
  uint8 *image = k6300_get_image();
  
    // It does not necessary to scan the whole image. 
    // If there is a feeder in sight then it can be seen from
    // the middle line of the image because the feeder is taller than the webot.
    for(x=0;x<K6300_WIDTH;x++)
    {
      red   = k6300_get_red  (image,x,K6300_HEIGHT/2);
      green = k6300_get_green(image,x,K6300_HEIGHT/2);
      blue  = k6300_get_blue (image,x,K6300_HEIGHT/2);       
      if( colorDetected(red,green,blue,FOOD) ) {
        myImage[x] = FOOD;
      } else {
        myImage[x] = NILL;
      }
    }
}

/*******************************************************************************************/
/*******************************************************************************************/

/* global module selection variables are used because this way is more efficient */

int set_active_module(void) {
    uint16 x,y;

    bool food_in_sight;
    bool food_near;
    bool obstacle_near;

    /* these two variables are used to generate wall_following and search_for_feeder sequences */
    static int wall_following_turn = 0;
    static int last_module_active = MODULE_EMPTY;

    left_food_stimuli    = 0;
    left_bumper_stimuli  = 0;
    right_food_stimuli   = 0;
    right_bumper_stimuli = 0;

    /* stimuli processing */ 
    // if the food is on the side it is more important to focus on it
    for(x=0;x<K6300_WIDTH/2;x++) {
        left_food_stimuli += (myImage[x] == FOOD);
    }	
    for(x=K6300_WIDTH/2;x<K6300_WIDTH;x++) {
        right_food_stimuli += (myImage[x] == FOOD);
    }
    for(x=0;x<3;x++) {
        left_bumper_stimuli += khepera_get_proximity(x);
    }
    for(x=3;x<6;x++) {
	right_bumper_stimuli += khepera_get_proximity(x);
    }

    printf("bumpers:%d,%d\n",left_bumper_stimuli,right_bumper_stimuli);    
    food_in_sight = (left_food_stimuli + right_food_stimuli > 0);  
    food_near = (left_food_stimuli + right_food_stimuli > FOOD_THRESHOLD);  
    obstacle_near = (left_bumper_stimuli + right_bumper_stimuli > BUMPER_THRESHOLD);
    trapped = (left_bumper_stimuli > HIGH_BUMPER_THRESHOLD && 
               right_bumper_stimuli > HIGH_BUMPER_THRESHOLD);

    ++last_feeder_seen;

    // changing when food is in sight
    if( left_food_stimuli > right_food_stimuli ) {
	last_feeder_dir = LEFT;
	if( food_near ) {
	    last_close_feeder_dir = LEFT;	    
	}
    } else if( right_food_stimuli > 0) {
	last_feeder_dir = RIGHT;
	if( food_near ) {
	    last_close_feeder_dir = RIGHT;	    
	}
    }

    //printf("last:%d",last_close_feeder_dir);

    if( last_feeder_seen_after_others ) {
	// WALL_FOLLOWING and FEEDER_APPROACH sets this value to one
	// here it is incremented and after reaching FEEDER_SEARCH_START_AFTER_OTHERS_THRESHOLD
	// search_for_feeder turns on and resets the counter. The incremention is also switched off.
	// With this search_for_feeder can follow the other two module after a few steps.
	++last_feeder_seen_after_others;
    }
    //printf("l:%d,%lf\n",last_feeder_seen_after_others,sim_speed);
    //printf("w:%d,o:%d,l:%d\n",wall_following_turn,obstacle_near,last_module_active);

    /* module selection */
    if( module_still_active == MODULE_EMPTY ) {
	if( trapped ){
	    return MODULE_ESCAPE;
	} else if( food_in_sight && !obstacle_near ) {
	    last_module_active = MODULE_FEEDER_APPROACH;
	    return MODULE_FEEDER_APPROACH;
	} else if( food_in_sight && obstacle_near ||
		   obstacle_near && last_feeder_seen > WALL_FOLLOWING_START_THRESHOLD / sim_speed ||
		   (left_bumper_stimuli > LOW_BUMPER_THRESHOLD ||
		    right_bumper_stimuli > LOW_BUMPER_THRESHOLD) && 
		    last_module_active == MODULE_SEARCH_FOR_FEEDER && 
		    wall_following_turn < WALL_FOLLOWING_TURNS ) {
	    if( wall_following_turn >= WALL_FOLLOWING_TURNS ) {
		wall_following_turn = 0;
	    }
	    last_module_active = MODULE_WALL_FOLLOWING;
	    return MODULE_WALL_FOLLOWING;
	} else if( last_feeder_seen > FEEDER_SEARCH_START_THRESHOLD / sim_speed ||
	           last_feeder_seen_after_others > FEEDER_SEARCH_START_AFTER_OTHERS_THRESHOLD / sim_speed  ) {
	    ++wall_following_turn;
	    last_module_active = MODULE_SEARCH_FOR_FEEDER;
	    return MODULE_SEARCH_FOR_FEEDER;	 
	} else {
	    printf("l:%d,r:%d,wft:%d\n",left_bumper_stimuli,right_bumper_stimuli,wall_following_turn);
	    last_module_active = MODULE_NORMAL_MOVE;
	    return MODULE_NORMAL_MOVE;
	}

    } else {
	return module_still_active;
    }   
}

/*******************************************************************************************/

bool normal_move(void) {
    int32 sensor,i;

    printf("normal_move\n");
    left_speed  = 0;
    right_speed = 0;
    for(i=0; i<8; i++)                      /* Connections sensors-motors */
    {
	sensor = khepera_get_proximity(i);
	right_speed += sensor * Interconn[i];
	left_speed  += sensor * Interconn[8+i];
    }
    left_speed  /= 400;                     /* Global gain */
    right_speed /= 400;
    left_speed  += MAX_FORWARD_SPEED;           /* Offset */
    right_speed += MAX_FORWARD_SPEED;
    khepera_set_speed(LEFT,left_speed);     /* Set the motor speeds */
    khepera_set_speed(RIGHT,right_speed);

    return false;
}

/*******************************************************************************************/

bool feeder_approach(void) {
    uint16 x,y;
  
    printf("feeder_approach\n");
    left_speed = 0;
    right_speed = 0;
    last_feeder_seen = 0;
    last_feeder_seen_after_others = 1;
    if( left_food_stimuli != right_food_stimuli ) {
	if( left_food_stimuli ) {
	    for(x=0;x<K6300_WIDTH/2;x++) {
        	left_speed -= (K6300_WIDTH/2 - x)/5 * ( myImage[x] == FOOD);
	    }
	}
	if( right_food_stimuli ) {
	    for(x=K6300_WIDTH/2;x<K6300_WIDTH;x++) {
        	right_speed -= (x - K6300_WIDTH/2)/5 * ( myImage[x] == FOOD);
	    }
	}
	/*
	//if( left_food_stimuli < 5 && !(left_food_stimuli < right_food_stimuli) ) {
    	//    left_speed = -left_food_stimuli * 5;
	//}
	//else 

	if( left_food_stimuli < 10 && !(left_food_stimuli < right_food_stimuli) ) {
    	    left_speed = -left_food_stimuli * 3;
	} else {
    	    left_speed = -left_food_stimuli/1.5;
	}

	//if( right_food_stimuli < 5 && !(right_food_stimuli > left_food_stimuli) ) {
	//    right_speed = -right_food_stimuli * 5;    
	//}
	//else 

	if( right_food_stimuli < 10 && !(right_food_stimuli > left_food_stimuli) ) {
	    right_speed = -right_food_stimuli * 3;    
	} else {
	    right_speed = -right_food_stimuli/1.5;    
	}*/

    }
    left_speed  += MAX_FORWARD_SPEED; //TURNING_OFFSET;          
    right_speed += MAX_FORWARD_SPEED; //TURNING_OFFSET;
    khepera_set_speed(LEFT,left_speed);
    khepera_set_speed(RIGHT,right_speed);

    return false;
}

/*******************************************************************************************/

bool search_for_feeder(void) 
{
    static int32 start_pos;
    static bool start_phase = true;
    static int side;
    
    printf("search_for_feeder\n");
    last_feeder_seen = 0;
    last_feeder_seen_after_others = 0;
    if( start_phase ) {
	if( drand() > 0.5 ) {
	    side = last_feeder_dir;
	} else {
	    side = !last_feeder_dir;
	}
        start_pos = my_khepera_get_position(LEFT);
        start_phase = false;
    }
    if( side == LEFT ) {
	left_speed = -MAX_FORWARD_SPEED;
	right_speed = MAX_FORWARD_SPEED;
    } else {
	left_speed = MAX_FORWARD_SPEED;
	right_speed = -MAX_FORWARD_SPEED;
    }
    khepera_set_speed(LEFT,left_speed);     /* Set the motor speeds */
    khepera_set_speed(RIGHT,right_speed);

    //printf("start:%ld,actleft:%ld\n",start_pos,my_khepera_get_position(LEFT));
    if( (side == LEFT && (my_khepera_get_position(LEFT) > start_pos - FULL_TURN) || 
	 side == RIGHT && (my_khepera_get_position(LEFT) < start_pos + FULL_TURN)) && 
        (left_food_stimuli + right_food_stimuli == 0)) {
	return true;
    } else {
	start_phase = true; // for the next call
	return false;
    }
	     
}

/*******************************************************************************************/

bool wall_following(void) 
{
    double left_sensor,middle_sensor,right_sensor;
    static int which_side = LEFT;
    static int step = 0;
    static int length = 2;
    static int middle_last = 0;
    
    printf("wall_following\n");
    // last_feeder_seen is not reset here because a search_for_feeder 
    // after wall-following is good
    last_feeder_seen_after_others = 1;
    if( step == 0 ) {
	if( left_bumper_stimuli > right_bumper_stimuli ) {
	    which_side = LEFT;
	} else {
	    which_side = RIGHT;
	}
    }
    left_sensor = (khepera_get_proximity(0) + khepera_get_proximity(1))/2.0;
    middle_sensor = (khepera_get_proximity(2) + khepera_get_proximity(3))/2.0;
    right_sensor = (khepera_get_proximity(4) + khepera_get_proximity(5))/2.0;
    if( which_side == LEFT ) {     	// left wall following
	if( middle_sensor > LOW_BUMPER_THRESHOLD - middle_last * LOW_BUMPER_THRESHOLD_BIAS ) {
	    middle_last = 1;
    	    left_speed = WALL_FOLLOWING_SPEED + 2;
	    right_speed = -WALL_FOLLOWING_SPEED;
	} else {
	    middle_last = 0;
	    printf("left\n");
    	    left_speed = left_sensor * 0.2 - 20.0;
	    right_speed = left_sensor * -0.2 + 60.0;
	}   
    } else {				// right wall following
	if( middle_sensor > LOW_BUMPER_THRESHOLD - middle_last * LOW_BUMPER_THRESHOLD_BIAS ) {
	    middle_last = 1;
    	    left_speed = -WALL_FOLLOWING_SPEED;
	    right_speed = WALL_FOLLOWING_SPEED + 2;
	} else {
	    middle_last = 0;
	    printf("right\n");
    	    left_speed = right_sensor * -0.2 + 60.0;
	    right_speed = right_sensor * 0.2 - 20.0;
	}
    }
    khepera_set_speed(LEFT,left_speed);     /* Set the motor speeds */
    khepera_set_speed(RIGHT,right_speed);
    ++step;
    if( left_food_stimuli + right_food_stimuli > 0 &&
	(left_food_stimuli > 0 && left_bumper_stimuli < LOW_BUMPER_THRESHOLD && last_close_feeder_dir != RIGHT ||
         right_food_stimuli > 0 && right_bumper_stimuli < LOW_BUMPER_THRESHOLD && last_close_feeder_dir != LEFT) || 
	 step > length * WALL_FOLLOWING_THRESHOLD / sim_speed ) {
	printf("wall_follow_leave,%d,%d,%d,%lf\n",left_food_stimuli,right_food_stimuli,step,length * WALL_FOLLOWING_THRESHOLD / sim_speed);
	step = 0;	// for next call
	last_close_feeder_dir = LEFT_AND_RIGHT;
	return false;
    } else {
	return true;
    }
}

/*******************************************************************************************/

bool escape(void) {
    static int step = 0;
    static const int ESCAPE_STEPS = 3;
    
    printf("escape\n");
    khepera_set_speed(LEFT,-MAX_FORWARD_SPEED);     /* Set the motor speeds */
    khepera_set_speed(RIGHT,-MAX_FORWARD_SPEED+1);
    if( ++step < ESCAPE_STEPS ) {
	return true;
    } else {
	step = 0;
	return false;
    }    
}

/*******************************************************************************************/
/*******************************************************************************************/

main() {
  int step = 0;
  khepera_live();
  create_gui();
  khepera_enable_proximity(IR_ALL_SENSORS);
  khepera_enable_position(LEFT_AND_RIGHT);
  k6300_enable();  
  
  for(;;)                                   /* Khepera never dies! */
  {
    sim_speed = khepera_fair_step() + EPSILON;
    if (gui_checkbox_is_active(imageCheckBox)) change_camera_image();
    trackImage();
    switch( set_active_module() ) {
	case MODULE_NORMAL_MOVE:
            if( normal_move() ) module_still_active = MODULE_NORMAL_MOVE;
	    else module_still_active = MODULE_EMPTY;
	    break;
	case MODULE_FEEDER_APPROACH:    
            if( feeder_approach() ) module_still_active = MODULE_FEEDER_APPROACH;
	    else module_still_active = MODULE_EMPTY;
	    break;
	case MODULE_SEARCH_FOR_FEEDER:
	    if( search_for_feeder() ) module_still_active = MODULE_SEARCH_FOR_FEEDER;
	    else module_still_active = MODULE_EMPTY;
	    break;   
	case MODULE_WALL_FOLLOWING:
	    if( wall_following() ) module_still_active = MODULE_WALL_FOLLOWING;
	    else module_still_active = MODULE_EMPTY;
	    break;   
	case MODULE_ESCAPE:
	    if( escape() ) module_still_active = MODULE_ESCAPE;
	    else module_still_active = MODULE_EMPTY;
	    break;   
    }	    
  }
}
