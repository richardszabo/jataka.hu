/*****************************************************************************/
/* File:         AvoidProximityDrive.java                                    */
/*                                                                           */
/* Language:     Blackdown Java 1.2.2 RC4, native threads                    */
/* APIs:         Webots ALife 2.0.3                                          */
/* Description:  Avoid close obstacles from IR sensors according to          */
/*               Braitenberg principles.                                     */
/*                                                                           */
/* Author:       Torbjorn S. Dahl, Machine Learning Research Group           */
/*               Department of Computer Science, University of Bristol, UK   */
/*****************************************************************************/

import java.util.Vector;
import uk.ac.bris.cs.tdahl.plancs.LateralThCtIntCircuit;
import uk.ac.bris.cs.tdahl.plancs.Side;

public class AvoidProximityDrive extends LateralThCtIntCircuit{
    
    // Class Constants
    //=================

    // Repulsion strength
    public static final int PROXIMITY_REPULSION = 20;

    // Instance Variables
    //====================
    
    // Inputs
    private ProximitySense pxsns;
    
    // Input influences
    private float pxrps;
    
    // Class Methods
    //===============
    
    // Constructor
    //-------------
    public AvoidProximityDrive(ProximitySense pxsns,boolean side){
	super(pxsns,side);
	this.pxsns=pxsns;
    } // Ends Constructor
    
    // Instance Methods
    //==================

    // evaluateFromThreshold
    //-----------------------
    protected void evaluateFromThreshold(){
	f=true;
	fint=PROXIMITY_REPULSION;
    } // Ends evaluateFromThreshold
    
    // toString
    // overrides
    public String toString(){
	return("AvoidProximityDrive-"+Side.sideToString(side));
    } // Ends toString
    
} // Ends class AvoidProximityDrive


















