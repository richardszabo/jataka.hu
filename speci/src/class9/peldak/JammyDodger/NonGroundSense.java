/*****************************************************************************/
/* File:         NonGroundSense.java                                         */
/*                                                                           */
/* Language:     Blackdown Java 1.2.2 RC4, native threads                    */
/* APIs:         Webots ALife 2.0.3                                          */
/* Description:  Detects amount of non ground in a given image               */
/*                                                                           */
/* Author:       Torbjorn S. Dahl, Machine Learning Research Group           */
/*               Department of Computer Science, University of Bristol, UK   */
/*****************************************************************************/

import uk.ac.bris.cs.tdahl.plancs.LateralIntCircuit;
import uk.ac.bris.cs.tdahl.plancs.Side;

public class NonGroundSense extends LateralIntCircuit{

    // Class constants
    //=================

    // The color of ground 
    public static final int GROUND_RED = 255;
    public static final int GROUND_GREEN = 213;
    public static final int GROUND_BLUE = 123;
    private static final int COLOR_TOLERANCE = 15;
    
    // Area of interest
    protected static final int HIGHEST_GROUND = 30;

    // Instance Variables
    //====================

    // Maximum firing strength
    public int MAX_FIRING_STRENGTH;

    // Sub image width and height
    protected SubImageSensor sis;
    protected int siw;
    protected int sih;

    // Class Methods
    //===============

    // Constructor
    //-------------
    public NonGroundSense(SubImageSensor sis,boolean side){
	super(sis,side);
	this.sis=sis;
	siw=sis.getWidth();
	sih=sis.getHeight();
	MAX_FIRING_STRENGTH=siw*(sih-HIGHEST_GROUND);
    } //Ends Constructor

    // isGroundColor
    public static boolean isGroundColor(int r,int g,int b){
	if(r>GROUND_RED-COLOR_TOLERANCE &&
	   g>GROUND_GREEN-COLOR_TOLERANCE && g<GROUND_GREEN+COLOR_TOLERANCE &&
	   b>GROUND_BLUE-COLOR_TOLERANCE && b<GROUND_BLUE+COLOR_TOLERANCE){
	    return(true);
	}else{
	    return(false);
	}	   
    } // Ends isGround

    // Instance Methods
    //==================

    // evaluateFromInteger
    //---------------------
    protected void evaluateFromInteger(){
	int r;
	int g;
	int b;
	f=true;
	for(int x=0;x<siw;x++){
	    for(int y=HIGHEST_GROUND;y<sih;y++){
		r=sis.getRedInteger(x,y);
		g=sis.getGreenInteger(x,y);
		b=sis.getBlueInteger(x,y);
		if(!isGroundColor(r,g,b)){
		    fint++;
		}
	    }
	}
    } // Ends evaluateFromInteger

    // toString
    // overrides
    public String toString(){
	return("NonGroundSense-"+Side.sideToString(side));
    } // Ends toString

} // Ends class NonGroundSense







