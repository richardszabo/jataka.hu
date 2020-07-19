/*****************************************************************************/
/* File:         EscapeProximityDrive.java                                   */
/*                                                                           */
/* Language:     Blackdown Java 1.2.2 RC4, native threads                    */
/* APIs:         Webots ALife 2.0.3                                          */
/* Description:  Escape when stuck with competing avoid proximity drives.    */
/*                                                                           */
/* Author:       Torbjorn S. Dahl, Machine Learning Research Group           */
/*               Department of Computer Science, University of Bristol, UK   */
/*****************************************************************************/

import java.util.Vector;
import uk.ac.bris.cs.tdahl.plancs.ThresholdCtIntCircuit;

public class EscapeProximityDrive extends ThresholdCtIntCircuit{
    
    // Class Constants
    //=================
    
    // Ground
    private static final int ESCAPE_PROPULSION = 10;
    
    // Class Methods
    //===============
    
    // Constructor
    //-------------
    public EscapeProximityDrive(Vector inputs){
	super(inputs);
	ft=1;
    } // Ends Constructor
    
    // Instance Methods
    //==================

    // evaluateFromThreshold
    //-----------------------
    protected void evaluateFromThreshold(){
	f=true;
	fint=ESCAPE_PROPULSION;
    } // Ends evaluateFromThreshold
    
    // toString
    public String toString(){
	return("EscapeProximityDrive");
    } // Ends toString
    
} // Ends class EscapeProximityDrive


















