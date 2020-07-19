/*****************************************************************************/
/* File:         FeedDrive.java                                              */
/*                                                                           */
/* Language:     Blackdown Java 1.2.2 RC4, native threads                    */
/* APIs:         Webots ALife 2.0.3                                          */
/* Description:  Makes contact with close full feeder.                       */
/*                                                                           */
/* Author:       Torbjorn S. Dahl, Machine Learning Research Group           */
/*               Department of Computer Science, University of Bristol, UK   */
/*****************************************************************************/

import uk.ac.bris.cs.tdahl.plancs.LateralColIntCircuit;
import uk.ac.bris.cs.tdahl.plancs.Side;

public class FeedDrive extends LateralColIntCircuit{

    // Class constants
    //=================

    private static final int CLOSE_SPEED_REDUCTION = 5;
    private static final int APPROACH_SPEED_REDUCTION = 1;

    // Instance Variables
    //====================

    // Input
    private FullFeederSense ffsns;

    // InputObject
    private FullFeederPerception ffpcn;

    // Class Methods
    //===============

    // Constructor
    //-------------
    public FeedDrive(FullFeederSense ffsns,boolean side){
	super(ffsns,side);
	this.ffsns=ffsns;
    } //Ends Constructor

    // Instance Methods
    //==================

    // evaluateFromCollect
    //---------------------
    protected void evaluateFromCollect(){
	if(ffsns.isFiring()){
	    ffpcn=(FullFeederPerception)iobjs.elementAt(0);
	    int ffdist=ffpcn.getDistance();
	    int ffang=ffpcn.getAngle();
	    if(ffdist==Distance.NEAR || ffang==Angle.GREAT){
		dprintln("Closing");
		f=FIRING;
		fint=CLOSE_SPEED_REDUCTION;
	    }else{
		dprintln("Approaching");
		f=FIRING;
		fint=APPROACH_SPEED_REDUCTION;
	    }
	}
    } // Ends evaluateFromCollect
    
    // toString
    // overrides
    public String toString(){
	return("FeedDrive-"+Side.sideToString(side));
    } // Ends toString

} // Ends class FeedDrive




