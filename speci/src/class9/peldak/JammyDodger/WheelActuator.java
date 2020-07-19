/*****************************************************************************/
/* File:         WheelActuator.java                                          */
/*                                                                           */
/* Language:     Blackdown Java 1.2.2 RC4, native threads                    */
/* APIs:         Webots ALife 2.0.3                                          */
/* Description:  Renames the LateralFsStCircuit for debugging purposes.      */
/*                                                                           */
/* Author:       Torbjorn S. Dahl, Machine Learning Research Group           */
/*               Department of Computer Science, University of Bristol, UK   */
/*****************************************************************************/

import uk.ac.bris.cs.tdahl.plancs.LateralSumWgtIntCircuit;
import uk.ac.bris.cs.tdahl.plancs.NeurallyConnectable;
import uk.ac.bris.cs.tdahl.plancs.Side;

public class WheelActuator extends LateralSumWgtIntCircuit{

    // Class Methods
    //===============
    
    // Constructor
    //-------------

    public WheelActuator(NeurallyConnectable input,boolean side){
	super(input,side);
    } // Ends Constructor

    public WheelActuator(NeurallyConnectable input,double iw,boolean side){
	super(input,iw,side);
    } // Ends Constructor

    // Instance Methods
    //==================

    // toString
    //----------
    public String toString(){
	return("WheelActuator-"+Side.sideToString(side));
    } // Ends toString
    
} // Ends class WheelActuator





