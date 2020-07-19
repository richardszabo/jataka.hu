/*****************************************************************************/
/* File:         MoveDrive.java                                              */
/*                                                                           */
/* Language:     Blackdown Java 1.2.2 RC4, native threads                    */
/* APIs:         Webots ALife 2.0.3                                          */
/* Description:  Requests wheel movement.                                    */
/*                                                                           */
/* Author:       Torbjorn S. Dahl, Machine Learning Research Group           */
/*               Department of Computer Science, University of Bristol, UK   */
/*****************************************************************************/

import uk.ac.bris.cs.tdahl.plancs.IntegerCircuit;
import uk.ac.bris.cs.tdahl.plancs.NeurallyConnectable;

public class MoveDrive extends IntegerCircuit{
    
    // Class constants
    //=================

    // Requested speed
    private static final int SPEED = 20;

    // Instance Methods
    //==================
    
    // Constructor
    //-------------
    public MoveDrive(NeurallyConnectable input){
	super(input);
    } // Ends Constructor
    
    // evaluateFromInteger
    //---------------------
    protected void evaluateFromInteger(){
	f=true;
	fint=SPEED;
    } // Ends evaluateFromInteger
    
    // toString
    // overrides
    public String toString(){
	return("MoveDrive");
    } // Ends toString
    
} // Ends class MoveDrive


















