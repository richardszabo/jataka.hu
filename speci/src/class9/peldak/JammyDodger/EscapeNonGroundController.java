/*****************************************************************************/
/* File:         EscapeNonGroundController.java                              */
/*                                                                           */
/* Language:     Blackdown Java 1.2.2 RC4, native threads                    */
/* APIs:         Webots ALife 2.0.3                                          */
/* Description:  Avoids obstacles from camera input according to the         */
/*               Braitenberg principles.                                     */
/*                                                                           */
/* Author:       Torbjorn S. Dahl, Machine Learning Research Group           */
/*               Department of Computer Science, University of Bristol, UK   */
/*****************************************************************************/

import java.util.Vector;

public class EscapeNonGroundController extends AvoidNonGroundController{

    // Instance Variables
    //====================
    
    // Drives
    protected EscapeNonGroundDrive engrd;

    // Class Methods
    //===============

    // Constructor
    //-------------
    public EscapeNonGroundController(){
	super();
	// EscapeNonGroundDrive
	Vector engrdis=new Vector(2);
	engrdis.add(angrdl);
	engrdis.add(angrdr);
	engrd=new EscapeNonGroundDrive(engrdis);
	wactl.addInput(engrd,1.0);
	wactr.addInput(engrd,-1.0);
	engrd.start();
	// Debugging
	/*
	  this.debuggingOn();
	  // MoveController
	  wactl.debuggingOn();
	  wactr.debuggingOn();
	  md.debuggingOn();
	  // AvoidNonGroundController
	  //sissrl.debuggingOn();
	  //sissrr.debuggingOn();
	  ngrsnsl.debuggingOn();
	  ngrsnsr.debuggingOn();
	  angrdl.debuggingOn();
	  angrdr.debuggingOn();
	  //dispvec.debuggingOn();
	  // EscapeNonGroundController
	  engrd.debuggingOn();
	*/
    }
    
    // Instance Methods
    //==================

    // toString
    // overrides
    public String toString(){
	return("EscapeNonGroundController");
    } // Ends toString

} // Ends class EscapeNonGroundController






