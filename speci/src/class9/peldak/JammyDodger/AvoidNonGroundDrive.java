/*****************************************************************************/
/* File:         AvoidNonGroundDrive.java                                    */
/*                                                                           */
/* Language:     Blackdown Java 1.2.2 RC4, native threads                    */
/* APIs:         Webots ALife 2.0.3                                          */
/* Description:  Avoid non ground areas according to Braitenberg principles. */
/*                                                                           */
/* Author:       Torbjorn S. Dahl, Machine Learning Research Group           */
/*               Department of Computer Science, University of Bristol, UK   */
/*****************************************************************************/

import java.util.Vector;
import uk.ac.bris.cs.tdahl.plancs.LateralThSumWgtIntCircuit;
import uk.ac.bris.cs.tdahl.plancs.Side;

public class AvoidNonGroundDrive extends LateralThSumWgtIntCircuit{
    
    // Class Constants
    //=================
    
    // Ground
    private static final double REDUCED_REPULSION_SATURATION_LEVEL = 0.5;
    private static final double REPULSION_REDUCTION = 0.8;
    
    // Instance Variables
    //====================

    // Maximum firing strength
    public int MAX_FIRING_STRENGTH;
    
    // Inputs
    private NonGroundSense ngrsns;
    
    // Input influences
    private float ngrrps;
    
    // Class Methods
    //===============
    
    // Constructor
    //-------------
    public AvoidNonGroundDrive(NonGroundSense ngrsns,boolean side){
	super(ngrsns,side);
	this.ngrsns=ngrsns;
	MAX_FIRING_STRENGTH=20;
	ngrrps=MAX_FIRING_STRENGTH/(float)ngrsns.MAX_FIRING_STRENGTH;
    } // Ends Constructor
    
    // evaluateFromThreshold
    //-----------------------
    protected void evaluateFromThreshold(){
	//  Evaluate non ground repulsion
	int ngrrep=(int)(swis*ngrrps);
	double sat=swis/(float)ngrsns.MAX_FIRING_STRENGTH;
	f=true;
	if(sat>REDUCED_REPULSION_SATURATION_LEVEL){
	    dprintln("Normal saturation level "+sat);
	    fint=ngrrep;
	}else{
	    dprintln("Reduced repulsion saturation level "+sat);
	    fint=(int)(ngrrep*REPULSION_REDUCTION);
	}
    } // Ends evaluateFromThreshold
    
    // toString
    // overrides
    public String toString(){
	return("AvoidNonGroundDrive-"+Side.sideToString(side));
    } // Ends toString
    
} // Ends class AvoidNonGroundDrive


















