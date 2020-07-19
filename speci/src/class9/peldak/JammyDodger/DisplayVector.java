/*****************************************************************************/
/* File:         DisplayVector.java                                          */
/*                                                                           */
/* Language:     Blackdown Java 1.2.2 RC4, native threads                    */
/* APIs:         Webots ALife 2.0.3                                          */
/* Description:  Creates and updates displays                                */
/*                                                                           */
/* Author:       Torbjorn S. Dahl, Machine Learning Research Group           */
/*               Department of Computer Science, University of Bristol, UK   */
/*****************************************************************************/

import java.util.Vector;
import java.util.Enumeration;
import uk.ac.bris.cs.tdahl.plancs.NeuralCircuit;
import uk.ac.bris.cs.tdahl.plancs.NeurallyConnectable;

public class DisplayVector extends NeuralCircuit{

    // Instance Variables
    //====================
    Vector disps = new Vector();

    // Class Methods
    //===============

    // Constructor
    //-------------
    public DisplayVector(NeurallyConnectable inputs){
	super(inputs);
    } // Ends Constructor

    // Instance Methods
    //==================
    
    // evaluate
    //----------
    public void evaluate(){
	Enumeration dispsen=disps.elements();
	CameraDisplay disp;
	while(dispsen.hasMoreElements()){
	    disp=(CameraDisplay)dispsen.nextElement();
	    disp.updateDisplay();
	    dprintln("Updated "+disp.toString());
	}
    } // Ends evaluate

    // addDisplay
    public void addDisplay(CameraDisplay d){
	d.initDisplay();
	disps.add(d);
	dprintln("Added "+d.toString());
    } // Ends addDisplay

    // toString
    // overridden
    public String toString(){
	return("DisplayVector");
    }    

} // Ends class DisplayVector
