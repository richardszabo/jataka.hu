/*****************************************************************************/
/* File:         GroundFilterDisplay.java                                    */
/*                                                                           */
/* Language:     Blackdown Java 1.2.2 RC4, native threads                    */
/* APIs:         Webots ALife 2.0.3                                          */
/* Description:  Displays a fragmented image filtered for ground.             */
/*                                                                           */
/* Author:       Torbjorn S. Dahl, Machine Learning Research Group           */
/*               Department of Computer Science, University of Bristol, UK   */
/*****************************************************************************/

public class GroundFilterDisplay extends CameraDisplay{

    // Class variables
    //=================

    private static long GROUND;

    // Class Methods
    //===============

    // Constructor
    //-------------
    public GroundFilterDisplay(KheperaController kc,int coln,int rown){
	super(kc,coln,rown);
	//System.out.println("Setting new x coordinate\n");
	winplx=700;
    } // Ends Constructor

    // Instance Methods
    //==================

    // setColor
    protected void setColor(){
	GROUND=kc.gui_color_get(NonGroundSense.GROUND_RED,
				NonGroundSense.GROUND_GREEN,
				NonGroundSense.GROUND_BLUE);
    }// Ends setColor
    
    // getDrawingColor
    protected long getDrawingColor(int r,int g,int b){
	if(NonGroundSense.isGroundColor(r,g,b)){
	    return(GROUND);
	}else{
	    return(kc.GUI_BLACK);
	}
    } // Ends getDrawingColor

     
    // toString
    // overridden
    public String toString(){
	return("GroundFilterDisplay");
    }

} // Ends class GroundFilterDisplay






















