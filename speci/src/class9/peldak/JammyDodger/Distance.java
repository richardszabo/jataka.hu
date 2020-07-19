/*****************************************************************************/
/* File:         Distance.java                                               */
/*                                                                           */
/* Language:     Blackdown Java 1.2.2 RC4, native threads                    */
/* APIs:         Webots ALife 2.0.3                                          */
/* Description:  Symbolic information about distances.                       */
/*                                                                           */
/* Author:       Torbjorn S. Dahl, Machine Learning Research Group           */
/*               Department of Computer Science, University of Bristol, UK   */
/*****************************************************************************/

public abstract class Distance {

    // Class Constants
    //=================

    public static final int ABUTTING = 0;
    public static final int NEAR = 1;
    public static final int MIDDLE = 2;
    public static final int FAR = 3;

    // Class Methods

    // distanceToString
    public static String distanceToString(int d){
	switch(d){
	case 0: 
	    return("ABUTTING");
	case 1: 
	    return("NEAR");
	case 2: 
	    return("MIDDLE");
	case 3: 
	    return("FAR");
	default:
	    return("Not a valid distance");
	}
    } // Ends distanceToString 

} // Ends abstract class Distance



