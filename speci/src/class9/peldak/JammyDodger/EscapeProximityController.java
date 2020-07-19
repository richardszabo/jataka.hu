/*****************************************************************************/
/* File:         EscapeProximityController.java                              */
/*                                                                           */
/* Language:     Blackdown Java 1.2.2 RC4, native threads                    */
/* APIs:         Webots ALife 2.0.3                                          */
/* Description:  Resolve competing avoid proximity drives.                   */
/*               Braitenberg principles.                                     */
/*                                                                           */
/* Author:       Torbjorn S. Dahl, Machine Learning Research Group           */
/*               Department of Computer Science, University of Bristol, UK   */
/*****************************************************************************/

import java.util.Vector;

public class EscapeProximityController extends AvoidProximityController{

    // Instance Variables
    //====================
    
    // Drives
    protected EscapeProximityDrive epxd;

    // Class Methods
    //===============

    // Constructor
    //-------------
    public EscapeProximityController(){
	super();
	// EscapeProximityDrive
	Vector epxdis=new Vector(2);
	epxdis.add(apxdl);
	epxdis.add(apxdr);
	epxd=new EscapeProximityDrive(epxdis);
	wactl.addInput(epxd,1.0);
	wactr.addInput(epxd,-1.0);
	epxd.start();
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
	  // AvoidProximityController
	  //irasrl.debuggingOn();
	  //irasrr.debuggingOn();
	  pxsnsl.debuggingOn();
	  pxsnsr.debuggingOn();
	  apxdl.debuggingOn();
	  apxdr.debuggingOn();
	  // EscapeProximityController
	  epxd.debuggingOn();
	*/
    }
    
    // Instance Methods
    //==================

    // toString
    // overrides
    public String toString(){
	return("EscapeProximityController");
    } // Ends toString

} // Ends class EscapeProximityController






