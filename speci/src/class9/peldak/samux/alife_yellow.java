/***************************************************************************
*
* port of mzperx3 controller of the Webots1 contest in Java
*
***************************************************************************/
import KheperaController;
import java.awt.image.*;

/**********************************************************************************/
class Module {
    public static final int EMPTY = 0;
    public static final int NORMAL_MOVE = 1;
    public static final int FEEDER_APPROACH = 2;
    //public static final int TURN_TO_FEEDER = 3;
    public static final int SEARCH_FOR_FEEDER = 4;
    public static final int WALL_FOLLOWING = 5;
    public static final int ESCAPE = 6;
    
}
/**********************************************************************************/
class Speed {
    public static final int MAX_FORWARD_SPEED		= 20;
    public static final int TURNING_SPEED 		= 10;
    public static final int WALL_FOLLOWING_SPEED 	= 15;
    public static final int FULL_TURN  		        = 21;
    public static final int ESCAPE_STEPS                = 3;
    public static final int WALL_FOLLOWING_TURNS        = 3;
}

/**********************************************************************************/
class RoomObject {
    public static final int NILL_ID   = 0;
    public static final int FOOD_ID   = 1;

    public static final int FOOD[]    = {0,255,0};
}

/**********************************************************************************/
class Threshold {
    public static int FOOD_THRESHOLD = 20;
    public static int HIGH_BUMPER_THRESHOLD = 1200;
    public static int BUMPER_THRESHOLD = 700;
    public static int LOW_BUMPER_THRESHOLD = 300;
    public static int LOW_BUMPER_THRESHOLD_BIAS = 200;
    public static int FEEDER_SEARCH_START_THRESHOLD = 3000;
    public static int FEEDER_SEARCH_START_AFTER_OTHERS_THRESHOLD = 200;
    public static int WALL_FOLLOWING_START_THRESHOLD = 1500;
    public static int WALL_FOLLOWING_THRESHOLD = 1200;
}

/**********************************************************************************/
class alife_yellow extends KheperaController {

    //public static final int IMAGE_SCALE     = 2;
    //public static final double EPSILON = 0.00001;
    public static final double EPSILON = 50.0;

    protected int moduleStillActive = Module.SEARCH_FOR_FEEDER;;
    protected int myImage[];
    protected int leftFoodStimuli;
    protected int rightFoodStimuli;
    protected int leftBumperStimuli;
    protected int rightBumperStimuli;
	
    protected int lastFeederSeen = 0;
    protected int lastFeederSeenAfterOthers = 0;

    /* actual speed of the simulation according to khepera_fair_step() */
    protected double simSpeed = 64.0;

    protected boolean trapped = false;

    protected int wallFollowingTurn = 0;
    protected int lastModuleActive = Module.EMPTY;

    /* last direction where feeder was seen */
    protected int lastFeederDir = LEFT;
    protected int lastCloseFeederDir = LEFT_AND_RIGHT;  // no normal default value 

    // parameters of searchForFeeder
    protected long startPos;
    protected boolean startPhase = true;
    protected int side;

    // paramaters of wallFollowing
    protected int whichSide = LEFT;
    protected int step = 0;
    protected int length = 2;
    protected int middleLast = 0;

    /******************************************************************************/
    long myKheperaGetPosition(int side) {
	//long value = 
	return khepera_get_position(side)/90;
	/*	if( value > 32768 ) {
	    return value - 65536;
	} else {
	    return value;
	    }*/
    }

    /******************************************************************************/
    /*    void create_camera_image()
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

    void paint_black_camera_image()
    {
      uint16 x,y;
      for(y = 0 ; y < K6300_HEIGHT ; y++) for(x = 0 ; x < K6300_WIDTH ; x++)
       gui_rectangle_change_color(picture1[x][y],GUI_BLACK);
    }


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
    */
    
    /****************************************************************************************/

    /******************************************************************************/
    float propRGB(int red, int green, int blue, int goal[])
    {
      float prop[], propG[];
      float simil, aux;

      prop = new float[3];
      propG = new float[3];
      if (green > 0 || blue > 0) prop[0] = red / (float)(green + blue); else prop[0] = red/255;
      if (red > 0 || blue > 0) prop[1] = green / (float)(red + blue); else prop[1] = green/255;
      if (red > 0 || green > 0) prop[2] = blue / (float)(red + green); else prop[2] = blue/255;
      aux = Math.max(prop[0],prop[1]); aux = Math.max(aux,prop[2]);
      if (aux>0) { prop[0]/=aux; prop[1]/=aux; prop[2]/=aux; }
      else prop[0] = prop[1] = prop[2] = 1;

      if (goal[1] > 0 || goal[2] > 0) propG[0] = goal[0] / (float)(goal[1] + goal[2]); else propG[0] = goal[0]/255;    
      if (goal[0] > 0 || goal[2] > 0) propG[1] = goal[1] / (float)(goal[0] + goal[2]); else propG[1] = goal[1]/255;
      if (goal[0] > 0 || goal[1] > 0) propG[2] = goal[2] / (float)(goal[0] + goal[1]); else propG[2] = goal[2]/255;
      aux = Math.max(propG[0],propG[1]); aux = Math.max(aux,propG[2]);
      if (aux>0) { propG[0]/=aux; propG[1]/=aux; propG[2]/=aux; }
      else propG[0] = propG[1] = propG[2] = 1;

      // scaled between (0,1)
      simil = 1-(Math.abs(prop[0]-propG[0]) + Math.abs(prop[1]-propG[1]) + Math.abs(prop[2]-propG[2]))/3;    

      return simil;
    }

    /*******************************************************************************************/

    boolean colorDetected(int red, int green, int blue, int[] goal)
    {
      boolean resp;
      float margin,diff,simil;
      int TOLERANCE = 50;              //tolerance on color detection

      simil = propRGB(red,green,blue,goal);
      diff = Math.min(100,Math.abs(red - green) + Math.abs(red - blue) + Math.abs(green - blue));
      margin = TOLERANCE * ((2 - diff/100) + simil);        // adding some robustness to light intensity 
      resp = (Math.abs(red - goal[0])<margin) && 
	     (Math.abs(green - goal[1])<margin) && 
	     (Math.abs(blue - goal[2])<margin); 
             // printf("Margin: %2.2f diffs[%2.2f,%2.2f,%2.2f]  ",margin, Math.abs(red - goal[0]),Math.abs(green - goal[1]),Math.abs(blue - goal[2]));
      return resp;
    }

    /*******************************************************************************************/

    void trackImage()
    {
      int image = k6300_get_image();

	// It does not necessary to scan the whole image. 
	// If there is a feeder in sight then it can be seen from
	// the middle line of the image because the feeder is taller than the webot.
	for(int x=0;x<K6300_WIDTH;x++)
	{
	  int red   = k6300_get_red  (image,x,K6300_HEIGHT/2);
	  int green = k6300_get_green(image,x,K6300_HEIGHT/2);
	  int blue  = k6300_get_blue (image,x,K6300_HEIGHT/2);       
	  if( colorDetected(red,green,blue,RoomObject.FOOD) ) {
	    myImage[x] = RoomObject.FOOD_ID;
	  } else {
	    myImage[x] = RoomObject.NILL_ID;
	  }
	}
    }

    /******************************************************************************/
    /**
     * This method selects the active module.
     */
    protected int setActiveModule() {
	int x;

	// collecting sensorial information
	leftFoodStimuli = 0;
	for(x=0;x<K6300_WIDTH/2;x++) {
	    if( myImage[x] == RoomObject.FOOD_ID ) {
		++leftFoodStimuli;
	    }
	}	
	rightFoodStimuli = 0;
	for(x=K6300_WIDTH/2;x<K6300_WIDTH;x++) {
	    if( myImage[x] == RoomObject.FOOD_ID ) {
		++rightFoodStimuli;
	    }
	}
	leftBumperStimuli = 0;
	for(x=0;x<3;x++) {
	    leftBumperStimuli += khepera_get_proximity(x);
	}
	rightBumperStimuli = 0;
	for(x=3;x<6;x++) {
	    rightBumperStimuli += khepera_get_proximity(x);
	}

        boolean foodInSight = (leftFoodStimuli + rightFoodStimuli > 0);  
	boolean foodNear = (leftFoodStimuli + rightFoodStimuli > Threshold.FOOD_THRESHOLD);  
        boolean obstacleNear = (leftBumperStimuli + rightBumperStimuli > Threshold.BUMPER_THRESHOLD);
        trapped = (leftBumperStimuli > Threshold.HIGH_BUMPER_THRESHOLD && 
                   rightBumperStimuli > Threshold.HIGH_BUMPER_THRESHOLD);

	++lastFeederSeen;

	// changing when food is in sight
	if( leftFoodStimuli > rightFoodStimuli ) {
	    lastFeederDir = LEFT;
	    if( foodNear ) {
		lastCloseFeederDir = LEFT;	    
	    }
	} else if( rightFoodStimuli > 0) {
	    lastFeederDir = RIGHT;
	    if( foodNear ) {
		lastCloseFeederDir = RIGHT;	    
	    }
	}

	//printf("last:%d",last_closeFeederDir);

	if( lastFeederSeenAfterOthers > 0 ) {
	    // WALL_FOLLOWING and FEEDER_APPROACH sets this value to one
	    // here it is incremented and after reaching FEEDER_SEARCH_START_AFTER_OTHERS_THRESHOLD
	    // searchForFeeder turns on and resets the counter. The incremention is also switched off.
	    // With this searchForFeeder can follow the other two module after a few steps.
	    ++lastFeederSeenAfterOthers;
	}

	/* module selection */
	if( moduleStillActive == Module.EMPTY ) {
	    if( trapped ){
		return Module.ESCAPE;
	    } else if( foodInSight && !obstacleNear ) {
		lastModuleActive = Module.FEEDER_APPROACH;
		return Module.FEEDER_APPROACH;
	    } else if( foodInSight && obstacleNear ||
		       obstacleNear && lastFeederSeen > Threshold.WALL_FOLLOWING_START_THRESHOLD / simSpeed ||
		       (leftBumperStimuli > Threshold.LOW_BUMPER_THRESHOLD ||
			rightBumperStimuli > Threshold.LOW_BUMPER_THRESHOLD) && 
			lastModuleActive == Module.SEARCH_FOR_FEEDER && 
			wallFollowingTurn < Speed.WALL_FOLLOWING_TURNS ) {
		if( wallFollowingTurn >= Speed.WALL_FOLLOWING_TURNS ) {
		    wallFollowingTurn = 0;
		}
		lastModuleActive = Module.WALL_FOLLOWING;
		return Module.WALL_FOLLOWING;
	    } else if( lastFeederSeen > Threshold.FEEDER_SEARCH_START_THRESHOLD / simSpeed ||
		       lastFeederSeenAfterOthers > Threshold.FEEDER_SEARCH_START_AFTER_OTHERS_THRESHOLD / simSpeed  ) {
		++wallFollowingTurn;
		lastModuleActive = Module.SEARCH_FOR_FEEDER;
		return Module.SEARCH_FOR_FEEDER;	 
	    } else {
		//printf("l:%d,r:%d,wft:%d\n",leftBumperStimuli,rightBumperStimuli,wallFollowing_turn);
		lastModuleActive = Module.NORMAL_MOVE;
		return Module.NORMAL_MOVE;
	    }

	} else {
	    return moduleStillActive;
	}   

    }

    /******************************************************************************/
    protected boolean normalMove() {
	//System.out.println("normalMove");
	/* obstacle avoidance vector */
	long Interconn[] = {-5,-15,-18,6,4,4,3,5,4,4,6,-18,-15,-5,5,3};	

	int leftSpeed = 0;
	int rightSpeed = 0;
	for(int i=0; i<8; i++) {               /* Connections sensors-motors */
	    long sensor = khepera_get_proximity(i);
	    rightSpeed += sensor * Interconn[i];
	    leftSpeed  += sensor * Interconn[8+i];
	}
	leftSpeed  /= 400;                     /* Global gain */
	rightSpeed /= 400;
	leftSpeed  += Speed.MAX_FORWARD_SPEED; /* Offset */
	rightSpeed += Speed.MAX_FORWARD_SPEED;
	khepera_set_speed(LEFT,leftSpeed);     /* Set the motor speeds */
	khepera_set_speed(RIGHT,rightSpeed);

	return false;
    }

    /******************************************************************************/
    protected boolean feederApproach() {
	//System.out.println("feederApproach");
	int leftSpeed = 0;
	int rightSpeed = 0;
	if( leftFoodStimuli != rightFoodStimuli ) {
	    int x;
	    if( leftFoodStimuli > 0 ) {
		for(x=0;x<K6300_WIDTH/2;x++) {
		    if( myImage[x] == RoomObject.FOOD_ID ) {
			leftSpeed -= (K6300_WIDTH/2 - x)/10;
		    }
		}
	    }
	    if( rightFoodStimuli > 0 ) {
		for(x=K6300_WIDTH/2;x<K6300_WIDTH;x++) {
		    if( myImage[x] == RoomObject.FOOD_ID ) {
			rightSpeed -= (x - K6300_WIDTH/2)/10;
		    }
		}
	    }
	}
	leftSpeed  += Speed.MAX_FORWARD_SPEED; 
	rightSpeed += Speed.MAX_FORWARD_SPEED; 
	//System.out.println("ls:" + leftSpeed + "rs:" + rightSpeed);
	/*
	if( leftSpeed < -Speed.MAX_FORWARD_SPEED ) {
	    leftSpeed = -Speed.MAX_FORWARD_SPEED;
	} else if( leftSpeed > Speed.MAX_FORWARD_SPEED ) {
	    leftSpeed = Speed.MAX_FORWARD_SPEED;
	}
	if( rightSpeed < -Speed.MAX_FORWARD_SPEED ) {
	    rightSpeed = -Speed.MAX_FORWARD_SPEED;
	} else if( leftSpeed > Speed.MAX_FORWARD_SPEED ) {
	    rightSpeed = Speed.MAX_FORWARD_SPEED;
	}*/

	//System.out.println("2ls:" + leftSpeed + "rs:" + rightSpeed);
	khepera_set_speed(LEFT,leftSpeed);
	khepera_set_speed(RIGHT,rightSpeed);

	return false;
    }

    /******************************************************************************/
    protected boolean searchForFeeder() {
	//System.out.println("searchForFeeder");

    	lastFeederSeen = 0;
	lastFeederSeenAfterOthers = 0;
	if( startPhase ) {
	    if( Math.random() > 0.5 ) {
		side = lastFeederDir;
	    } else if( lastFeederDir == LEFT ) {
    		side = RIGHT;
	    } else if( lastFeederDir == RIGHT ) {
    		side = LEFT;
	    }
	    startPos = myKheperaGetPosition(LEFT);
	    startPhase = false;
	}
	int leftSpeed;
	int rightSpeed;
    	if( side == LEFT ) {
	    leftSpeed = -Speed.MAX_FORWARD_SPEED;
	    rightSpeed = Speed.MAX_FORWARD_SPEED;
	} else {
	    leftSpeed = Speed.MAX_FORWARD_SPEED;
	    rightSpeed = -Speed.MAX_FORWARD_SPEED;
	}
	khepera_set_speed(LEFT,leftSpeed);     /* Set the motor speeds */
	khepera_set_speed(RIGHT,rightSpeed);

	if( (side == LEFT && (myKheperaGetPosition(LEFT) > startPos - Speed.FULL_TURN) || 
	     side == RIGHT && (myKheperaGetPosition(LEFT) < startPos + Speed.FULL_TURN)) && 
	    (leftFoodStimuli + rightFoodStimuli == 0)) {
	    return true;
	} else {
	    startPhase = true; // for the next call
	    return false;
	}
    }

    /******************************************************************************/
    protected boolean wallFollowing() {
	//System.out.println("wallFollowing");

	// lastFeederSeen is not reset here because a search_forFeeder 
	// after wall-following is good
	lastFeederSeenAfterOthers = 1;
	
	if( step == 0 ) {
	    if( leftBumperStimuli > rightBumperStimuli ) {
		whichSide = LEFT;
	    } else {
		whichSide = RIGHT;
	    }
	}
	double leftSensor = (khepera_get_proximity(0) + khepera_get_proximity(1))/2.0;
	double middleSensor = (khepera_get_proximity(2) + khepera_get_proximity(3))/2.0;
	double rightSensor = (khepera_get_proximity(4) + khepera_get_proximity(5))/2.0;
	int leftSpeed;
	int rightSpeed;
	if( whichSide == LEFT ) {     	// left wall following
	    if( middleSensor > Threshold.LOW_BUMPER_THRESHOLD - middleLast * Threshold.LOW_BUMPER_THRESHOLD_BIAS ) {
		middleLast = 1;
		leftSpeed = Speed.WALL_FOLLOWING_SPEED + 2;
		rightSpeed = -Speed.WALL_FOLLOWING_SPEED;
	    } else {
		middleLast = 0;
		//printf("left\n");
		leftSpeed = (int)(leftSensor * 0.2 - 20.0);
		rightSpeed = (int)(leftSensor * -0.2 + 60.0);
	    }   
	} else {				// right wall following
	    if( middleSensor > Threshold.LOW_BUMPER_THRESHOLD - middleLast * Threshold.LOW_BUMPER_THRESHOLD_BIAS ) {
		middleLast = 1;
		leftSpeed = -Speed.WALL_FOLLOWING_SPEED;
		rightSpeed = Speed.WALL_FOLLOWING_SPEED + 2;
	    } else {
		middleLast = 0;
		//printf("right\n");
		leftSpeed = (int)(rightSensor * -0.2 + 60.0);
		rightSpeed = (int)(rightSensor * 0.2 - 20.0);
	    }
	}
	khepera_set_speed(LEFT,leftSpeed);     /* Set the motor speeds */
	khepera_set_speed(RIGHT,rightSpeed);
	++step;
	if( leftFoodStimuli + rightFoodStimuli > 0 &&
	    (leftFoodStimuli > 0 && 
	     leftBumperStimuli < Threshold.LOW_BUMPER_THRESHOLD && 
	     lastCloseFeederDir != RIGHT ||
    
	     rightFoodStimuli > 0 && 
	     rightBumperStimuli < Threshold.LOW_BUMPER_THRESHOLD && 
	     lastCloseFeederDir != LEFT) || 
    
	     step > length * Threshold.WALL_FOLLOWING_THRESHOLD / simSpeed ) {
	    //printf("wall_follow_leave,%d,%d,%d,%lf\n",leftFoodStimuli,rightFoodStimuli,step,length * WALL_FOLLOWING_THRESHOLD / sim_speed);
	    step = 0;	// for next call
	    lastCloseFeederDir = LEFT_AND_RIGHT;
	    return false;
	} else {
	    return true;
	}
    }

    /******************************************************************************/
    protected boolean escape() {
	//System.out.println("escape");

	khepera_set_speed(LEFT,-Speed.MAX_FORWARD_SPEED);     /* Set the motor speeds */
	khepera_set_speed(RIGHT,-Speed.MAX_FORWARD_SPEED+1);
	if( ++step < Speed.ESCAPE_STEPS ) {
	    return true;
	} else {
	    step = 0;
	    return false;
	}    
    }

    /******************************************************************************/
    /**
     * Initialization of the webot. 
     */
    public void init(boolean sync) {
	khepera_enable_proximity(IR_ALL_SENSORS); 
	k6300_enable(); 
	khepera_enable_position(LEFT_AND_RIGHT);
	myImage = new int[K6300_WIDTH];
	//createGui();
    }

    /******************************************************************************/
    /**
     * Step-by-step behavior of the webot, the main cycle of its working mechanism.
     */
    public void step(long ms) {

	//  determining the speed of the simulation
	simSpeed = ms + EPSILON;
	//System.out.println("simspeed:"+simSpeed);

	//if (gui_checkbox_is_active(imageCheckBox)) changeCameraImage();
	// tracking the image
	trackImage();
	switch( setActiveModule() ) {
	    case Module.NORMAL_MOVE:
                if( normalMove() ) moduleStillActive = Module.NORMAL_MOVE;
	        else moduleStillActive = Module.EMPTY;
	        break;
	    case Module.FEEDER_APPROACH:
                if( feederApproach() ) moduleStillActive = Module.FEEDER_APPROACH;
	        else moduleStillActive = Module.EMPTY;
	        break;
	    case Module.SEARCH_FOR_FEEDER:
	        if( searchForFeeder() ) moduleStillActive = Module.SEARCH_FOR_FEEDER;
	        else moduleStillActive = Module.EMPTY;
	        break;   
	    case Module.WALL_FOLLOWING:
                if( wallFollowing() ) moduleStillActive = Module.WALL_FOLLOWING;
	        else moduleStillActive = Module.EMPTY;
	        break;
	    case Module.ESCAPE:
	        if( escape() ) moduleStillActive = Module.ESCAPE;
	        else moduleStillActive = Module.EMPTY;
	        break;   
	}
    }

    /******************************************************************************/
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/



