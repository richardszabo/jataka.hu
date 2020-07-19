/*****************************************************************************/
/* File:         AvoidNonGroundController.java                               */
/*                                                                           */
/* Language:     Blackdown Java 1.2.2 RC4, native threads                    */
/* APIs:         Webots ALife 2.0.3                                          */
/* Description:  Avoids obstacles from camera input according to the         */
/*               Braitenberg principles.                                     */
/*                                                                           */
/* Author:       Torbjorn S. Dahl, Machine Learning Research Group           */
/*               Department of Computer Science, University of Bristol, UK   */
/*****************************************************************************/

import uk.ac.bris.cs.tdahl.plancs.Side;
import uk.ac.bris.cs.tdahl.plancs.NeurallyConnectable;

public class AvoidNonGroundController extends MoveController{

    // Instance Variables
    //====================
    
    // Data displays
    protected DisplayVector dispvec;

   // Sensors
    protected SubImageSensor sissrl;
    protected SubImageSensor sissrr;
    
    // Senses
    protected NonGroundSense ngrsnsl;
    protected NonGroundSense ngrsnsr;

    // Drives
    protected AvoidNonGroundDrive angrdl;
    protected AvoidNonGroundDrive angrdr;

    // Class Methods
    //===============

    // Constructor
    //-------------
    public AvoidNonGroundController(){
	super();
	// SubImageSensor, Left
	sissrl=new SubImageSensor(this,0,K6300_WIDTH/2,0,K6300_HEIGHT);
	sissrl.start();
	// SubImageSensor, Right
	sissrr=
	    new SubImageSensor(this,K6300_WIDTH/2,K6300_WIDTH,0,K6300_HEIGHT);
	sissrr.start();
	// GroundSense, Left
	ngrsnsl=new NonGroundSense(sissrl,Side.LEFT);
	ngrsnsl.start();
	// GroundSense, Right
	ngrsnsr=new NonGroundSense(sissrr,Side.RIGHT);
	ngrsnsr.start();
	// AvoidNonGroundDrive, Left
	angrdl=new AvoidNonGroundDrive(ngrsnsl,Side.LEFT);
	wactr.addInput(angrdl,-1.0);
	angrdl.start();
	// AvoidNonGroundDrive, Right
	angrdr=new AvoidNonGroundDrive(ngrsnsr,Side.RIGHT);
	wactl.addInput(angrdr,-1.0);
	angrdr.start();
	// DisplayVector
	dispvec=new DisplayVector(this);
	dispvec.start();
	addInput(dispvec);
	// CameraDisplay
	CameraDisplay camdisp=new CameraDisplay(this,2,1);
	dispvec.addDisplay(camdisp);
	// GroundFilterDisplay
	GroundFilterDisplay grfdisp=new GroundFilterDisplay(this,2,1);
	dispvec.addDisplay(grfdisp);
	// Debugging
	/*
	  this.debuggingOn();
	  // MoveController
	  wactl.debuggingOn();
	  wactr.debuggingOn();
	  md.debuggingOn();
	  // AvoidNonGroundController
	  dispvec.debuggingOn();
	  //sissrl.debuggingOn();
	  //sissrr.debuggingOn();
	  ngrsnsl.debuggingOn();
	  ngrsnsr.debuggingOn();
	  angrdl.debuggingOn();
	  angrdr.debuggingOn();
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
	return("AvoidNonGroundController");
    } // Ends toString

} // Ends class AvoidNonGroundController






