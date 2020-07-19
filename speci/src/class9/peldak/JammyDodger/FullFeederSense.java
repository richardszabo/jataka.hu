/*****************************************************************************/
/* File:         FullFeederSense.java                                        */
/*                                                                           */
/* Language:     Blackdown Java 1.2.2 RC4, native threads                    */
/* APIs:         Webots ALife 2.0.3                                          */
/* Description:  Detects amount of full feeder(s) in a given image           */
/*                                                                           */
/* Author:       Torbjorn S. Dahl, Machine Learning Research Group           */
/*               Department of Computer Science, University of Bristol, UK   */
/*****************************************************************************/


import java.util.Vector;
import uk.ac.bris.cs.tdahl.plancs.LateralObjCircuit;
import uk.ac.bris.cs.tdahl.plancs.Side;

public class FullFeederSense extends LateralObjCircuit{

    // Class constants
    //=================

    // Colors
    public static final int FULL_FEEDER_RED = 0;    
    public static final int FULL_FEEDER_GREEN = 255;    
    public static final int FULL_FEEDER_BLUE = 0;    
    private static final int COLOR_TOLERANCE = 100;

    // The maximum height for the bottom of a feeder
    private static final int HIGHEST_FEEDER_BOTTOM = 30;

    // Instance Variables
    //====================

    // Sub image width and height
    public int IMAGE_TOP;
    public int IMAGE_BOTTOM;
    public int IMAGE_WIDTH;
    public int IMAGE_HEIGHT;

    // Input
    private SubImageSensor sims;

    // Full feeder perception
    private FullFeederPerception ffpcn;
    
    // Khepera detected
    private boolean kd;

    // Class Methods
    //===============

    // Constructor
    //-------------
    public FullFeederSense(SubImageSensor sims,boolean side){
	super(sims,side);
	this.sims=sims;
	IMAGE_TOP=sims.getTop();
	IMAGE_BOTTOM=sims.getBottom();
	IMAGE_WIDTH=sims.getWidth();
	IMAGE_HEIGHT=sims.getHeight();
    } //Ends Constructor

    // isFullFeeder
    public static boolean isFullFeederColor(int r,int g,int b){
	//if(Math.abs(r-b)<COLOR_TOLERANCE && r<g){
	if(r<COLOR_TOLERANCE && g>255-COLOR_TOLERANCE && b<COLOR_TOLERANCE){
	    return(true);
	}else{
	    return(false);
	}	   
    } // Ends isFeeder

    // Instance Methods
    //==================

    // evaluateFromObject
    //--------------------
    protected void evaluateFromObject(){
	int r;
	int g;
	int b;
	int ffbtlx=-1;
	int ffbtly=-1;
	int ffbtrx=-1;
	int ffbtry=-1;
	int ffbblx=-1;
	int ffbbly=-1;
	int ffbbrx=-1;
	int ffbbry=-1;
	// Find corners
	for(int y=0;y<IMAGE_HEIGHT;y++){
	    for(int x=0;x<IMAGE_WIDTH;x++){
		r=sims.getRedInteger(x,y);
		g=sims.getGreenInteger(x,y);
		b=sims.getBlueInteger(x,y);
		if(isFullFeederColor(r,g,b)){
		    f=FIRING;
		    // Set top on first green pixel
		    if(ffbtlx==-1){
			ffbtlx=x;
			ffbtly=y;
			ffbtrx=x;
			ffbtry=y;
			ffbblx=x;
			ffbbly=y;
			ffbbrx=x;
			ffbbry=y;
			dprintln("Found initial full feeder pixel "+
				 ffbtlx+"/"+ffbtly);
		    }
		    // Stretch to the right
		    if(x>ffbtrx){
			ffbtrx=x;
			ffbtry=y;
			ffbbrx=x;
			ffbbry=y;
		    }
		    // Stretch to the left
		    if(x<ffbtlx){
			ffbtlx=x;
			ffbtly=y;
			ffbblx=x;
			ffbbly=y;
		    }
		    // Stretch right side downwards
		    if(x==ffbbrx && y>ffbbry){
			ffbbry=y;
		    }		
		    // Stretch left side downwards
		    if(x==ffbblx && y>ffbbly){
			ffbbly=y;
		    }		
		}
	    }
	}
	if(f){
	    // Something green was found
	    kd=checkForKhepera(ffbtlx,ffbtly,ffbtrx,ffbtry,
			       ffbblx,ffbbly,ffbbrx,ffbbry);
	    if(!kd){
		// Found feeder
		f=FIRING;
		fobj=new FullFeederPerception(ffbtlx,ffbtly,ffbtrx,ffbtry,
					      ffbblx,ffbbly,ffbbrx,ffbbry);
		dprintln("Found "+fobj.toString());
	    }else{
		// Found another Khepera
		// Reset firing value
		dprintln("Image contains another Khepera!");
		f=NOT_FIRING;
	    }
	}
    } // Ends evaluateFromObject
    
    // checkForKhepera
    // Checks for pixels above top and that left and right sides are solid
    private boolean checkForKhepera(int ffbtlx,int ffbtly,
				    int ffbtrx,int ffbtry,
				    int ffbblx,int ffbbly,
				    int ffbbrx,int ffbbry){
	int r;
	int g;
	int b;
	int stopy;
	int startx;
	int stopx; 
	// Look for green pixels above top between left and right
	dprintln("Checking that highest side is also lowest side");
	if(ffbtly<=ffbtry){
	    if(ffbbly<ffbbry){
		dprintln("Highest side did not have lowest point");
		return(true);
	    }
	    stopy=ffbtly;
	}else{
	    if(ffbbly>ffbbry){
		dprintln("Highest side did not have lowest point");
		return(true);
	    }	    
	    stopy=ffbtry;
	}
	dprintln("Checking for full feeder pixels above top corner "+stopy);
	for(int y=0;y<stopy;y++){
	    for(int x=ffbtlx;x<ffbtrx;x++){
		r=sims.getRedInteger(ffbtlx,y);
		g=sims.getGreenInteger(ffbtlx,y);
		b=sims.getBlueInteger(ffbtlx,y);
		if(isFullFeederColor(r,g,b)){
		    dprintln("Green pixel found above top corner, x/y "+
			     x+"/"+y);
		    dprintln("Image contains another Khepera!");
		    return(true);
		}
	    }
	}
	// Look for non green pixels between top and bottom of left edge
	dprintln("Checking that left edge is vertical and solid");
	for(int y=ffbtly;y<ffbbly;y++){
	    r=sims.getRedInteger(ffbtlx,y);
	    g=sims.getGreenInteger(ffbtlx,y);
	    b=sims.getBlueInteger(ffbtlx,y);
	    if(!isFullFeederColor(r,g,b)){
		dprintln("Non green pixel found in left edge, x/y "+
			 ffbtlx+"/"+y);
		return(true);
	    }
	}
	// Look for non green pixels between top and bottom of right edge
	dprintln("Checking that rightt edge is vertical and solid");
	for(int y=ffbtry;y<ffbbry;y++){
	    r=sims.getRedInteger(ffbtrx,y);
	    g=sims.getGreenInteger(ffbtrx,y);
	    b=sims.getBlueInteger(ffbtrx,y);
	    if(!isFullFeederColor(r,g,b)){
		dprintln("Non green pixel found in right edge, x/y "
			 +ffbtrx+"/"+y);
		return(true);
	    }
	}
	return(false);
    } // Ends checkForKhepera 

    // kheperaDetected
    public boolean kheperaDetected(){
	return(kd);
    } // Ends kheperaDetected
    
    // toString
    // overrides
    public String toString(){
	return("FullFeederSense-"+Side.sideToString(side));
    } // Ends toString

} // Ends class FullFeederSense





