/*****************************************************************************/
/* File:         AvoidProximityController.java                               */
/*                                                                           */
/* Language:     Blackdown Java 1.2.2 RC4, native threads                    */
/* APIs:         Webots ALife 2.0.3                                          */
/* Description:  Avoids obstacles from IR input according to the             */
/*               Braitenberg principles.                                     */
/*                                                                           */
/* Author:       Torbjorn S. Dahl, Machine Learning Research Group           */
/*               Department of Computer Science, University of Bristol, UK   */
/*****************************************************************************/

import java.util.Vector;
import uk.ac.bris.cs.tdahl.plancs.Side;

public class AvoidProximityController extends EscapeNonGroundController{

    // Instance Variables
    //====================
    
   // Sensors
    protected IRArraySensor irasrl;
    protected IRArraySensor irasrr;    
    
    // Senses
    protected ProximitySense pxsnsl;
    protected ProximitySense pxsnsr;

    // Drives
    protected AvoidProximityDrive apxdl;
    protected AvoidProximityDrive apxdr;

    // Class Methods
    //===============

    // Constructor
    //-------------
    public AvoidProximityController(){
	super();
	// IRArraySensor, Left
	irasrl=new IRArraySensor(this,0,1,2);
	irasrl.start();
	// IRArraySensor, Right
	irasrr=new IRArraySensor(this,3,4,5);
	irasrr.start();
	// ProximitySense, Left
	pxsnsl=new ProximitySense(irasrl,Side.LEFT);
	pxsnsl.start();
	// ProximitySense, Right
	pxsnsr=new ProximitySense(irasrr,Side.RIGHT);
	pxsnsr.start();
	// AvoidProximityDrive, Left
	apxdl=new AvoidProximityDrive(pxsnsl,Side.LEFT);
	wactr.addInput(apxdl,-1.0);
	apxdl.start();
	// AvoidProximityDrive, Right
	apxdr=new AvoidProximityDrive(pxsnsr,Side.RIGHT);
	wactl.addInput(apxdr,-1.0);
	apxdr.start();
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
	*/
    }
    
    // Instance Methods
    //==================

    // Init
    //------
    // overrides (hence public)
    public void init(boolean s){
	super.init(s);
	// Enable camera
	k6300_enable();
	dprint("Enabled Camera\n");
	// Enable IR sensors
	khepera_enable_proximity(IR_ALL_SENSORS);
	dprint("Enabled IR sensors\n");
    } // Ends Init
    
    // toString
    // overrides
    public String toString(){
	return("AvoidProximityController");
    } // Ends toString

} // Ends class AvoidProximityController





