/*****************************************************************************/
/* File:         MoveController.java                                         */
/*                                                                           */
/* Language:     Blackdown Java 1.2.2 RC4, native threads                    */
/* APIs:         Webots ALife 2.0.3                                          */
/* Description:  A controller that moves according to the Braitenberg        */
/*               principles, as implemented in Toxicoreo                     */
/*                                                                           */
/* Author:       Torbjorn S. Dahl, Machine Learning Research Group           */
/*               Department of Computer Science, University of Bristol, UK   */
/*****************************************************************************/

import uk.ac.bris.cs.tdahl.plancs.Side;
import uk.ac.bris.cs.tdahl.plancs.NeurallyConnectable;

public class MoveController extends NeuralController{

    // Instance Variables
    //====================
    
    // Wheel Speeds
    protected int spl;
    protected int spr;

    // Actuators
    protected volatile WheelActuator wactl;
    protected volatile WheelActuator wactr;

    // Drives
    protected volatile MoveDrive md;

    // Class Methods
    //===============

    // Constructor
    //-------------
    public MoveController(){
	super();
	// MoveDrive, Left
	md=new MoveDrive(this);
	md.start();
	// WheelActuator, Left
	wactl=new WheelActuator(md,Side.LEFT);
	wactl.start();
	addInput(wactl);
	// WheelActuator, Right
	wactr=new WheelActuator(md,Side.RIGHT);
	wactr.start();
	addInput(wactr);
	// Debugging
	/*
	  this.debuggingOn();
	  // MoveController
	  wactl.debuggingOn();
	  wactr.debuggingOn();
	  md.debuggingOn();
	*/
    }
    
    // Instance Methods
    //==================

    // activateKhepera
    // implements
    protected void activateKhepera(){
	spl=wactl.getInteger();
	if(spl>20){
	    spl=20;
	}
	if(spl<-20){
	    spl=-20;
	}
	khepera_set_speed(this.LEFT,spl);
	spr=wactr.getInteger();
	if(spr>20){
	    spr=20;
	}
	if(spr<-20){
	    spr=-20;
	}
	khepera_set_speed(this.RIGHT,spr);	
	dprint("Set speeds "+spl+", "+spr+"\n");
    } // Ends activateKhepera
    
    // toString
    // overrides
    public String toString(){
	return("MoveController");
    } // Ends toString

} // Ends class MoveController





