/*****************************************************************************/
/* File:         FeedController.java                                         */
/*                                                                           */
/* Language:     Blackdown Java 1.2.2 RC4, native threads                    */
/* APIs:         Webots ALife 2.0.3                                          */
/* Description:  A controller that heads towards close feeders, inhibiting   */
/*               the avoid drives.                                           */
/*                                                                           */
/* Author:       Torbjorn S. Dahl, Machine Learning Research Group           */
/*               Department of Computer Science, University of Bristol, UK   */
/*****************************************************************************/

import java.util.Vector;
import uk.ac.bris.cs.tdahl.plancs.Side;

public class FeedController extends EscapeProximityController{

    // Instance Variables
    //====================

    // Senses
    protected FullFeederSense ffsnsl;
    protected FullFeederSense ffsnsr;

    // Drives
    protected FeedDrive fdl;
    protected FeedDrive fdr;

    // Class Methods
    //===============

    // Constructor
    //-------------
    public FeedController(){
	super();
	// FeederSense, Left
	ffsnsl=new FullFeederSense(sissrl,Side.LEFT);
	ffsnsl.start();
	// FeederSense, Right
	ffsnsr=new FullFeederSense(sissrr,Side.RIGHT);
	ffsnsr.start();
	// FeedDrive, Left
	fdl=new FeedDrive(ffsnsl,Side.LEFT);
	wactl.addInput(fdl,-1.0);
	angrdl.addInput(fdl,-1200.0);
	angrdr.addInput(fdl,-1200.0);
	fdl.start();
	// FeedDrive, Right
	fdr=new FeedDrive(ffsnsr,Side.RIGHT);
	wactr.addInput(fdr,-1.0);
	angrdl.addInput(fdr,-1200.0);
	angrdr.addInput(fdr,-1200.0);
	fdr.start();
	// FullFeederFilterDisplay
	Vector ffsnss=new Vector(2);
	ffsnss.add(ffsnsl);
	ffsnss.add(ffsnsr);
	FullFeederFilterDisplay fffdisp=
	    new FullFeederFilterDisplay(this,2,1,ffsnss);
	dispvec.addDisplay(fffdisp);
	dispvec.addInput(ffsnsl);
	dispvec.addInput(ffsnsr);
	// Debugging
	this.debuggingOn();
	// MoveController
	wactl.debuggingOn();
	wactr.debuggingOn();
	//md.debuggingOn();
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
	// FeedController
	ffsnsl.debuggingOn();
	ffsnsr.debuggingOn();
	fdl.debuggingOn();
	fdr.debuggingOn();
	//fffdisp.debuggingOn();
    }

    // Instance Methods
    //==================

    // toString
    // overrides
    public String toString(){
	return("FeedController");
    } // Ends toString

} // Ends class FeedController








