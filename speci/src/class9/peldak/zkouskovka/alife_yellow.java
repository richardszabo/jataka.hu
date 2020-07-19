/*****************************************************************************/
/* File:         alife_yellow.java                                           */
/*                                                                           */
/*                                                                           */
/* Author:       Emanuele.Ornella@cyberbotics.com                            */
/*               pavel@ucw.cz                                                */
/*               ebik@artax.karlin.mff.cuni.cz
/* Placed into public domain by Emanuele Ornella                             */
/*                                                                           */
/* Zkuskovka: pavel's modified robot.                                        */
/*****************************************************************************/

import KheperaController;
import java.lang.Math;   // for abs, random, min and max

class alife_yellow extends KheperaController {


    public static final int IMAGE_SCALE     = 2;
    public static final int FORWARD_SPEED   =30;

    public static final int TOLERANCE       =50;      //tolerance on color detection
    public static final int NILL            = 0;
    public static final int FOOD            = 1;
    public static final int MARK            = 2;
    public static final int CORNER          = 3;

    public static final int NORMAL          = 0;    
    public static final int WALL            = 1;
    public static final int IGNOREFOOD      = 2;

    int rolling = 0;
    int unstuck = 0;
    int was_at_wall = 0;
    int time = 128;

    int Interconn[] = {-5,-15,-18,6,4,4,3,5,4,4,6,-18,-15,-5,5,3};	
    int window2;
    int picture1[][] = new int[K6300_WIDTH][K6300_HEIGHT];
    int picture2[][] = new int[K6300_WIDTH][K6300_HEIGHT];
    int myImage[][] = new int[K6300_WIDTH][K6300_HEIGHT];
    int food[]		={0,255,0};
    int mark[]		={255,0,255};
    int corner[]	={255,255,0};
    boolean food_detected;
    int minX,maxX;        // min and max x coordinates of a detected KheperaFeeder
    int minY1, maxY1;
    int minY2, maxY2;
    int minY, maxY;  /* How do we pass params by pointer?! Bloody java */

    int left,right;
    int image;
    
    boolean freeField;
    int hunger=0;
    float acc = 0;
    float target = 0;
    double x = 0.3, z = 0.02, alpha = 0;
    int first = 2;

    int mood = 0;
    int mood_time = 5000;

    public float drand()
    {
	return (float) Math.random();
    }
    

    public void callback() {
    }

    void create_camera_image()
    {
	int x, y, x_rectangle, y_rectangle;
	long color = gui_color_get(0,0,0);
	
	for(x = 0 ; x < K6300_WIDTH ; x++) 
	    {
		for(y = 0 ; y < K6300_HEIGHT ; y++) 
		    {
			x_rectangle = 30 + IMAGE_SCALE * x;
			y_rectangle = 30 + IMAGE_SCALE * y;
			picture2[x][y] = gui_rectangle_new(window2,x_rectangle,y_rectangle,IMAGE_SCALE,IMAGE_SCALE,color,true);
		    }
	    }	
    }

    void paint_black_camera_image()
    {
	int x,y;
	for(y = 0 ; y < K6300_HEIGHT ; y++) 
	    for(x = 0 ; x < K6300_WIDTH ; x++)
		gui_rectangle_change_color(picture1[x][y],GUI_BLACK);

    }  

    void create_gui()
    {
	//	window1 = gui_window_new("MY Eye",30,50,220,180);
	//	window2 = gui_window_new("MY Filter",260,50,220,180);
	//create_camera_image();
	//	gui_window_show(window1);
	//gui_window_show(window2);
    }

    void update_filter_camera()
    {
	int x,y;
	long color;

	for(y = 0 ; y < K6300_HEIGHT ; y++)
	    for(x = 0 ; x < K6300_WIDTH ; x++)
		{
		    if (myImage[x][y]==FOOD) color = gui_color_get(food[0],food[1],food[2]);
		    else if (myImage[x][y]==MARK) color = gui_color_get(mark[0],mark[1],mark[2]);
		    else if (myImage[x][y]==CORNER) color = gui_color_get(corner[0],corner[1],corner[2]);
		    else color = gui_color_get(0,0,0);
		    gui_rectangle_change_color(picture2[x][y],color);
		}
    }


    float propRGB(int red, int green, int blue, int goal[]) 
    {
	float prop []= new float[3];
	float propG[]= new float[3];
	float simil, aux;
	//  printf("goal: %d %d %d  ",goal[0],goal[1], goal[2]);  
 
	if (green > 0 || blue > 0) prop[0] = red / (float)(green + blue); else prop[0] = red/255;
	if (red > 0 || blue > 0) prop[1] = green / (float)(red + blue); else prop[1] = green/255;
	if (red > 0 || green > 0) prop[2] = blue / (float)(red + green); else prop[2] = blue/255;
	aux = Math.max(prop[0],prop[1]); 
	aux = Math.max(aux,prop[2]);
	if (aux>0) { prop[0]/=aux; prop[1]/=aux; prop[2]/=aux; }
	else prop[0] = prop[1] = prop[2] = 1;
	  
	// printf("PROP: %2.2f %2.2f %2.2f  ",prop[0],prop[1],prop[2]);
  
	if (goal[1] > 0 || goal[2] > 0) propG[0] = goal[0] / (float)(goal[1] + goal[2]); else propG[0] = goal[0]/255;
	if (goal[0] > 0 || goal[2] > 0) propG[1] = goal[1] / (float)(goal[0] + goal[2]); else propG[1] = goal[1]/255;
	if (goal[0] > 0 || goal[1] > 0) propG[2] = goal[2] / (float)(goal[0] + goal[1]); else propG[2] = goal[2]/255;
	aux = Math.max(propG[0],propG[1]); 
	aux = Math.max(aux,propG[2]);
	if (aux>0) { propG[0]/=aux; propG[1]/=aux; propG[2]/=aux; }
	else propG[0] = propG[1] = propG[2] = 1;

	//  printf("PROPG: %2.2f %2.2f %2.2f  ",propG[0],propG[1],propG[2]);
	simil = 1-(Math.abs(prop[0]-propG[0]) + Math.abs(prop[1]-propG[1]) + Math.abs(prop[2]-propG[2]))/3;    // scaled between (0,1)
	//  printf("SIMIL: %2.4f ",simil);

	return(simil);
    }


    boolean colorDetected(int x, int y, int target)
    {
	boolean resp;
	float margin,diff,simil;
	int goal[] = new int [3]; 
	int red, green, blue;

	red   = k6300_get_red  (image,x,y);
	green = k6300_get_green(image,x,y);
	blue  = k6300_get_blue (image,x,y);

	switch (target)
	    {
	    case FOOD:   
		goal[0] = food[0]; 
		goal[1] = food[1]; 
		goal[2] = food[2]; 
		break;
	    case MARK:   
		goal[0] = mark[0]; 
		goal[1] = mark[1]; 
		goal[2] = mark[2]; 
		break;
	    case CORNER: 
		goal[0] = corner[0]; 
		goal[1] = corner[1]; 
		goal[2] = corner[2]; 
		break;
	    } 
  
	simil = propRGB(red,green,blue,goal);
	diff = Math.min(100,Math.abs(red - green) + Math.abs(red - blue) + Math.abs(green - blue));
	margin = TOLERANCE * ((2 - diff/100) + simil);        // adding some robustness to light intensity 
	resp =(boolean)  (Math.abs(red - goal[0])<margin) && (Math.abs(green - goal[1])<margin) && (Math.abs(blue - goal[2])<margin); // printf("Margin: %2.2f diffs[%2.2f,%2.2f,%2.2f]  ",margin, abs(red - goal[0]),abs(green - goal[1]),abs(blue - goal[2])) ;
	return(resp);
    }

    void find_yrange(int x)
    {
	int y;

	minY = -1;
	maxY = -1;
	if (food_detected) {
	    for(y=0;y<K6300_HEIGHT;y++) {
		boolean isfood = colorDetected(x, y, FOOD);
		if (isfood && (maxY != -1)) {
		    food_detected = false;
		}
		if (isfood && (minY == -1))
		    minY = y;
		if (((!isfood) || y == K6300_HEIGHT-1) && (minY != -1) && (maxY == -1))
		    maxY = y-1;
	    }
	}
		

    }

    void trackImage()
    {
	int red,green,blue;
	int x,y;
	boolean resp;
	boolean lastFood = false;

	image = k6300_get_image();

	food_detected = true;
	if (false) {
	for(y=0;y<K6300_HEIGHT;y++)
	    for(x=0;x<K6300_WIDTH;x++)
		{
		    resp = colorDetected(x,y,FOOD);
		    if (resp) 
			{
			    myImage[x][y] = FOOD;
			    food_detected = true;
			}
		    else	
			myImage[x][y] = 0;
		}
	}

	if (food_detected) {
	    maxX = -1;
	    minX = -1;
	    y = K6300_HEIGHT/2 - 1;
	    if (colorDetected(0,y,FOOD))
		minX = 0;
	    for(x=0;x<K6300_WIDTH;x++) {
		boolean isfood = colorDetected(x,y,FOOD);
		if (isfood && (maxX != -1)) {
		    food_detected = false;
		}
		if (isfood && (minX == -1))
		    minX = x;
		if (!isfood && (minX != -1) && (maxX == -1))
		    maxX = x-1;
	    }
	    if (myImage[K6300_WIDTH-1][y] == FOOD)
		maxX = K6300_WIDTH-1;
	    if ((minX == -1) || (maxX == -1))
		food_detected = false;
	}

	find_yrange(minX);
	minY1 = minY;
	maxY1 = maxY;
	find_yrange(maxX);
	minY2 = minY;
	maxY2 = maxY;


	if (food_detected) {
	    int point;

	    myImage[minX][2] = MARK;
	    myImage[maxX][2] = MARK;
		    
	    target = ((float)minX + (float)maxX)/2;
	    if ((maxY1 - minY1) > (maxY2 - minY2 + 1)) {
		System.out.println( "Moving target right" );
		target += 15;
	    }
	    if ((maxY1 - minY1 + 1) < (maxY2 - minY2)) {
		System.out.println( "Moving target left" );
		target -= 15;
	    }


	    point = (int) target;
	    if ((point < 0) || (point > K6300_WIDTH-1))
		System.out.println( "target out of range" );
	    else
		myImage[(int) target][2] = CORNER;

	    target -= K6300_WIDTH/2; 
	}

	//	update_filter_camera();  
    }




    void updateSpeeds_wall()
    {
	int sens[]={0,1,2,3,4,5,6,7};
	int roto[]={2,4,8,12,20,20}; //fast but dont work with slow image process
	//int roto[]={1,2,4,6,12,20};  slow but works ok with slow controller
	int rotr[]={650,300,130,130,400,800};
	int cor0g=600,cor0l=450,cor1g=115,cor1l=85,
	    trsh0=100,trsh1=30,mul1l=40,mul1r=25,mul0=70;
	int unstuck1=30,unstuck0=400;
	int i;
	boolean d;
	boolean debug = false;


	d=true;		

	left=right=0;
	for (i=0;i<8;i++) {
	    sens[i] = khepera_get_proximity(i);
	    if (debug) {
		System.out.print((sens[i]<10)?"  ":(sens[i]<100)?" ":"");
		System.out.print(sens[i]);
		System.out.print(" ");
		if (i==2) System.out.print("^ ");
		if (i==5) System.out.print(" | ");
	    }
	}
	if ((sens[0]>unstuck0)&&(sens[1]<unstuck1)) {
	    if (unstuck<30) {
		if (debug)
		    System.out.print(".!.");
		unstuck++;
	    } else {
		if (debug)
		    System.out.print("!!! unstucking !!!!");
		unstuck=0;
		right=-(left=20);
		d=false;
	    }
	} else {
	    if (debug)
		System.out.print("|  ");
	    if (unstuck>0) unstuck=0;
	}
			

	if (d) for (i=0;i<6;i++) 
	    if (sens[i]>rotr[i]) {
		right=-(left=roto[i]); 
		d=false;
		was_at_wall=5000/time;//5 secs
		if (debug) {
		    System.out.print(">");
		    System.out.print(i);
		    System.out.print(",");
		    System.out.print(sens[i]);
		}
	    }
	if (debug)
	    if (! d) System.out.println(">");

	if (d&&(sens[0]<trsh0)&&(sens[1]<trsh1)) {
	    if (was_at_wall>0) {
		System.out.println("was_at_wall  <<-      3 10");
		was_at_wall--;
		left=7;
		right=20;
	    } else {
		System.out.println("free         -X-      !GO!");
		left=right=20;

	    }
	    d=false;
	}
	

	if (d) {
	    if (debug)
		System.out.print("corr        ");
	    if (sens[1]>cor1g) {
		i=(sens[1]-cor1g)/mul1r;
		right=19-2*i;left=20;
		if (debug)
		    System.out.print("1 >");
	    } else if (sens[1]<cor1l) {
		if (sens[1]>trsh1) {
		    i=(cor1l-sens[1])/mul1l;
		    right=20; left=19-2*i; 
		    if (debug)
			System.out.print("1 <");
		} else {
		    if (debug)
			System.out.print("1!<");
		    right=17;
		    left=3+sens[0]/100;
		}
	    } else if (sens[0]>cor0g) {
		i=(sens[0]-cor0g)/mul0;
		right=20;left=19-2*i;
		if (debug)
		    System.out.print("0 <");
	    } else if (sens[0]<cor0l) {
		i=(cor0l-sens[0])/mul0;
		left=20;right=19-2*i;
		if (debug)
		    System.out.print("0 >");
	    } else {
		left=right=20;
		if (debug)
		    System.out.print("  X");
	    }
	    if (debug) {
		System.out.print((left>=0)?((left<10)?"   ":"  "):(left>-10)?"  ":" ");
		System.out.print(left);
		System.out.print("L R");
		System.out.println(right);
	    }
	}

	khepera_set_speed(LEFT,left);
	khepera_set_speed(RIGHT,right);
    }

    void updateSpeeds_normal()
    {
	int sensor,i;
	float mean, proximity;
	
	freeField = true;
	left  = 0;
	right = 0;
	for(i=0; i<8; i++)                      /* Connections sensors-motors */
	    {
		sensor = khepera_get_proximity(i);
		right += sensor * Interconn[i];
		left  += sensor * Interconn[8+i];
		//    printf("(%d)",sensor);
		freeField = freeField && (khepera_get_proximity(i)<20);
	    }
	// printf("\n");
	left  /= 400;                     /* Global gain */
	right /= 400;
	left  += FORWARD_SPEED;           /* Offset */
	right += FORWARD_SPEED;

	khepera_set_speed(LEFT,left);     /* Set the motor speeds */
	khepera_set_speed(RIGHT,right);

	mean = 0;
	if (food_detected)
	    {
		rolling = 0;
		hunger = 0;
		acc = target;
		mean = acc;
		System.out.println("I can see some food");
	    }
	else
	    {
		hunger++; 
		if (hunger>20) 
		    {
			hunger = 0;
			acc = (float) (50 * (drand()-0.5)); 
			if (drand()>0.96) {
			    System.out.println( "What about pireuete?" );
			    rolling = 10;
			}
		    }
		mean = acc;
	    }

	if (false) {
	    System.out.print("Using modification of ");
	    System.out.print(acc);
	    System.out.print(" with freefield ");
	    System.out.print(freeField);
	    System.out.println("");
	}

	if (food_detected) {
	    /* Be much more aggresive when you see food */
	    if (freeField) { left += acc*1.0; right -= acc*1.0; }
	    else           { left += acc*0.3; right -= acc*0.3; }
	} else {
	    if (acc!=0)
		if (acc<0){ right -= acc; left /=2; }
		else { left += acc; right /=2; }
	    else if (mean<0) right -= mean;
	    else left += mean;
	}

	if (rolling != 0) {
	    rolling--;
	    right = -10;
	    left = 10;
	}
		    
	khepera_set_speed(LEFT,left);     /* Set the motor speeds */
	khepera_set_speed(RIGHT,right);
    }

    public void updateSpeeds()
    {
	if ((mood == IGNOREFOOD) && (mood_time < 0)) {
	    System.out.println( "Ignored food for too long -> WALL" );
	    mood = WALL;
	    mood_time = 50000;
	}
	if ((mood == WALL) && food_detected) {
	    System.out.println( "I see food -> NORMAL" );
	    mood = NORMAL;
	    mood_time = 25000;
	}
	if ((mood == WALL) && (mood_time < 0)) {
	    System.out.println( "I was waling at wall for too long -> NORMAL" );
	    mood = NORMAL;
	    mood_time = 25000;
	}
	if ((mood == NORMAL) && (mood_time < 0))
	    if (food_detected) {
		System.out.println( "That food is poisoned -> IGNOREFOOD" );
		mood = IGNOREFOOD;
		mood_time = 10000;
	    } else {
		System.out.println( "I can't see food any more -> WALL" );
		mood = WALL;
		mood_time = 90000;
	    }
		

	if (mood == NORMAL)
	    updateSpeeds_normal();
	else
	    updateSpeeds_wall();
		
    }

    public void init(boolean sync)
    {
        if (sync) System.out.println("WARNING: you should run the ALife contest controllers in Asynchronous Mode.");
       	create_gui();
       	khepera_enable_proximity(IR_ALL_SENSORS); 
       	k6300_enable(); 
    }

    public void step(long ms)
    {
       	short i;
        updateSpeeds();
       	trackImage();
	
	if (first>0) {
	    first--;
	    return;
	}
	if (ms == 0)
	    ms = 32;
	alpha = alpha + ms*((double) right-(double) left)/54.04;
	{
	    double a2;
	    a2 = (alpha*360.0)/(2*3.141592654);
	    z = z - ((double) left+(double) right)*ms*Math.cos(a2)*0.000007857275;
	    x = x + ((double) left+(double) right)*ms*Math.sin(a2)*0.000007857275;
	}

	mood_time -= (5*ms);

	if (true) {
	    System.out.print("x=");	
	    System.out.print(x);
	    System.out.print(";z=");	
	    System.out.print(z);
	    System.out.print(";alpha=");	
	    System.out.print(alpha);
	    System.out.print(";mood=");
	    System.out.print(mood);
	    System.out.print(";mood_time=");
	    System.out.print(mood_time);
	    System.out.print(";step=");
	    System.out.println(ms);
	}
    }
}
