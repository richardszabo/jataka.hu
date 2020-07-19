/*****************************************************************************/
/* File:         ProximitySensor.java                                        */
/*                                                                           */
/* Language:     Blackdown Java 1.2.2 RC4, native threads                    */
/* APIs:         Webots ALife 2.0.3                                          */
/* Description:  Adds up proximities that exceede the stated limit.          */
/*                                                                           */
/* Author:       Torbjorn S. Dahl, Machine Learning Research Group           */
/*               Department of Computer Science, University of Bristol, UK   */
/*****************************************************************************/

import uk.ac.bris.cs.tdahl.plancs.LateralFiringCircuit;
import uk.ac.bris.cs.tdahl.plancs.Side;

public class ProximitySense extends LateralFiringCircuit{

    // Class Constants
    //=================

    private static final int FIRING_LIMIT = 700;

    // Instance Variables
    //====================

    // Max firing strength
    public int MAX_FIRING_STRENGTH;
    
    // IRArraySensor
    private IRArraySensor irasr;
    
    // Class Methods
    //===============

    // Constructor
    //-------------
    public ProximitySense(IRArraySensor irasr,boolean side){
	super(irasr,side);
	this.irasr=irasr;    
    } // Ends Contructor
    
    // Instance Methods
    //==================

    // evaluateFromFiring
    //--------------------
    protected void evaluateFromFiring(){
	for(int pxc=0;pxc<IRArraySensor.NUMBER_OF_IR_SENSORS;pxc++){
	    int px=irasr.getProximity(pxc);
	    dprintln("proximity "+px+", firing limit "+FIRING_LIMIT);
	    if(px>FIRING_LIMIT){
		f=true;
	    }
	}
    } // Ends evaluateFromFiring

    // toString
    // overrides
    public String toString(){
	return("ProximitySense-"+Side.sideToString(side));
    } // Ends toString

} // Ends class ProximitySense













