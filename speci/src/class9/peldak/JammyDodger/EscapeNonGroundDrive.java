/*****************************************************************************/
/* File:         EscapeNonGroundDrive.java                                   */
/*                                                                           */
/* Language:     Blackdown Java 1.2.2 RC4, native threads                    */
/* APIs:         Webots ALife 2.0.3                                          */
/* Description:  Escape when stuck with competing avoid non ground drives.   */
/*                                                                           */
/* Author:       Torbjorn S. Dahl, Machine Learning Research Group           */
/*               Department of Computer Science, University of Bristol, UK   */
/*****************************************************************************/

import java.util.Vector;
import uk.ac.bris.cs.tdahl.plancs.IntegerCircuit;

public class EscapeNonGroundDrive extends IntegerCircuit{
    
    // Class Constants
    //=================
    
    // Ground
    private static final double ESCAPE_SATURATION_LEVEL = 0.8;
    private static final double ESCAPE_PROPULSION = 10;
    
    // Instance Variables
    //====================
    
    // Activators
    private AvoidNonGroundDrive angrdl;
    private AvoidNonGroundDrive angrdr;

    // Class Methods
    //===============

    // Constructor
    //-------------
    public EscapeNonGroundDrive(Vector inputs){
	super(inputs);
	angrdl=(AvoidNonGroundDrive)inputs.elementAt(0);
	angrdr=(AvoidNonGroundDrive)inputs.elementAt(1);
    } // Ends Constructor
   
    // Instance Methods
    //==================
    
    // evaluateFromInteger
    //---------------------
    protected void evaluateFromInteger(){
	int angrdlint=angrdl.getInteger();
	int angrdrint=angrdr.getInteger();
	double satl=angrdlint/(double)angrdl.MAX_FIRING_STRENGTH;
	double satr=angrdrint/(double)angrdr.MAX_FIRING_STRENGTH;
	dprintln("Saturation left/right "+satl+"/"+satr);
	if(satl>ESCAPE_SATURATION_LEVEL && satr>ESCAPE_SATURATION_LEVEL){
	    f=FIRING;
	    fint+=ESCAPE_PROPULSION;
	}
    } // Ends evaluateFromInteger
    
    // toString
    // overrides
    public String toString(){
	return("EscapeNonGroundDrive");
    } // Ends toString
    
} // Ends class EscapeNonGroundDrive


















