/*****************************************************************************/
/* File:         IRArraySensor.java                                          */
/*                                                                           */
/* Language:     Blackdown Java 1.2.2 RC4, native threads                    */
/* APIs:         Webots ALife 2.0.3                                          */
/* Description:  Gets the values of the spcified IR sensors.                 */
/*                                                                           */
/* Author:       Torbjorn S. Dahl, Machine Learning Research Group           */
/*               Department of Computer Science, University of Bristol, UK   */
/*****************************************************************************/

import uk.ac.bris.cs.tdahl.plancs.NeuralCircuit;

public class IRArraySensor extends NeuralCircuit{

    // Class constants
    //=================

    // Max proximity
    public static final int MAX_PROXIMITY = 1023;
    // Number of ir sensors
    public static final int NUMBER_OF_IR_SENSORS = 3;

    // Instance Variables
    //====================
    
    // KheperaController
    private KheperaController kc;

    // Proximity sensors
    private int[] irs;
    // Proximities
    private int[] pxs;

    // Class Methods
    //===============

    // Constructor
    //-------------

    // Constructor
    public IRArraySensor(NeuralController nc,int ir0, int ir1,int ir2){
	super(nc);
	this.kc=nc;
	irs = new int[NUMBER_OF_IR_SENSORS];
	pxs = new int[NUMBER_OF_IR_SENSORS];
	irs[0]=ir0;
	irs[1]=ir1;
	irs[2]=ir2;
    } // Ends Constructor

    // Instance Methods
    //==================

    // Evaluate
    //----------
    // overrides
    protected void evaluate(){
	// Get proximities
	for(int ir=0;ir<NUMBER_OF_IR_SENSORS;ir++){
	    pxs[ir]=kc.khepera_get_proximity(irs[ir]);
	}
	dprint("proximities "+pxs[0]+" "+pxs[1]+" "+pxs[2]+"\n");
    } // Ends Evaluate
    
    // getIR
    public int getProximity(int ir){
	return(pxs[ir]);
    } // Ends getIR
    
    // toString
    // overrides
    public String toString(){
	return("IRArraySensor("+irs[0]+","+irs[1]+","+irs[2]+")");
    } // Ends toString

} // Ends class IRArraySensor













