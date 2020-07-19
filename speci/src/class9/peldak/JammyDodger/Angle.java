/*****************************************************************************/
/* File:         Angle.java                                                  */
/*                                                                           */
/* Language:     Blackdown Java 1.2.2 RC4, native threads                    */
/* APIs:         Webots ALife 2.0.3                                          */
/* Description:  Symbolic information about angles.                          */
/*                                                                           */
/* Author:       Torbjorn S. Dahl, Machine Learning Research Group           */
/*               Department of Computer Science, University of Bristol, UK   */
/*****************************************************************************/

public abstract class Angle{

    // Class Constants
    //=================

    public static final int SMALL = 0;
    public static final int MIDDLE = 1;
    public static final int GREAT = 2;

    // Class Methods
    //===============

    // angleToString
    public static String angleToString(int d){
	switch(d){
	case 0: 
	    return("SMALL");
	case 1: 
	    return("MIDDLE");
	case 2: 
	    return("GREAT");
	default:
	    return("Not a valid distance");
	}
    } // Ends angleToString 

} // Ends abstract class Angle



