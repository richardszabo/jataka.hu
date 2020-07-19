/*****************************************************************************/
/* File:         AvoidController.java                                        */
/*                                                                           */
/* Language:     Blackdown Java 1.2.2 RC4, native threads                    */
/* APIs:         Webots ALife 2.0.3                                          */
/* Description:  Avoids obstacles as it moves according to the Braitenberg   */
/*               principles.                                                 */
/*                                                                           */
/* Author:       Torbjorn S. Dahl, Machine Learning Research Group           */
/*               Department of Computer Science, University of Bristol, UK   */
/*****************************************************************************/

import java.util.Vector;

public class AvoidController extends MoveController{

    // Instance Variables
    //====================
    
    // Data displays
    protected DisplayVector dispvec;

   // Sensors
    protected SubImageSensor sissrl;
    protected SubImageSensor sissrr;
    protected IRArraySensor irasrl;
    protected IRArraySensor irasrr;    
    
    // Senses
    protected GroundSense grsnsl;
    protected GroundSense grsnsr;
    protected ProximitySense pxsnsl;
    protected ProximitySense pxsnsr;

    // Drives
    protected AvoidDrive adl;
    protected AvoidDrive adr;

    // Class Methods
    //===============

    // Constructor
    //-------------
    public AvoidController(){
	super();
	//this.debuggingOn();
	// SubImageSensor, Left
	sissrl=new SubImageSensor(this,0,K6300_WIDTH/2,0,K6300_HEIGHT);
	//sissrl.debuggingOn();
	sissrl.start();
	// SubImageSensor, Right
	sissrr=
	    new SubImageSensor(this,K6300_WIDTH/2,K6300_WIDTH,0,K6300_HEIGHT);
	//sissrr.debuggingOn();
	sissrr.start();
	// IRArraySensor, Left
	irasrl=new IRArraySensor(this,0,1,2);
	//irasrl.debuggingOn();
	irasrl.start();
	// IRArraySensor, Right
	irasrr=new IRArraySensor(this,3,4,5);
	//irasrr.debuggingOn();
	irasrr.start();
	// GroundSense, Left
	grsnsl=new GroundSense(sissrl,Side.LEFT);
	//grsnsl.debuggingOn();
	grsnsl.start();
	// GroundSense, Right
	grsnsr=new GroundSense(sissrr,Side.RIGHT);
	//grsnsr.debuggingOn();
	grsnsr.start();
	// ProximitySense, Left
	pxsnsl=new ProximitySense(irasrl,Side.LEFT);
	//pxsnsl.debuggingOn();
	pxsnsl.start();
	// ProximitySense, Right
	pxsnsr=new ProximitySense(irasrr,Side.RIGHT);
	//pxsnsr.debuggingOn();
	pxsnsr.start();
	// AvoidDrive, Left
	Vector adlactrs=new Vector(2);
	adlactrs.add(grsnsr);
	adlactrs.add(pxsnsr);
	adl=new AvoidDrive(adlactrs,Side.LEFT);
	//adl.debuggingOn();
	adl.start();
	wactl.addActivator(adl);
	// AvoidDrive, Right
	Vector adractrs=new Vector(2);
	adractrs.add(grsnsl);
	adractrs.add(pxsnsl);
	adr=new AvoidDrive(adractrs,Side.RIGHT);
	//adr.debuggingOn();
	adr.start();
	wactr.addActivator(adr);
	// DisplayVector
	dispvec=new DisplayVector(this);
	dispvec.start();
	addActivator(dispvec);
	// CameraDisplay
	CameraDisplay camdisp=new CameraDisplay(this,2,1);
	dispvec.addDisplay(camdisp);
	// GroundFilterDisplay
	GroundFilterDisplay grfdisp=new GroundFilterDisplay(this,2,1);
	dispvec.addDisplay(grfdisp);
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
	return("AvoidController");
    } // Ends toString

} // Ends class AvoidController





