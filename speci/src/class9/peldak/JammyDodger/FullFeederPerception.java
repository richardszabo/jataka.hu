/*****************************************************************************/
/* File:         FullFeederPerception.java                                   */
/*                                                                           */
/* Language:     Blackdown Java 1.2.2 RC4, native threads                    */
/* APIs:         Webots ALife 2.0.3                                          */
/* Description:  The perceptional data for a full feeder.                    */
/*                                                                           */
/* Author:       Torbjorn S. Dahl, Machine Learning Research Group           */
/*               Department of Computer Science, University of Bristol, UK   */
/*****************************************************************************/

import uk.ac.bris.cs.tdahl.plancs.DebuggableObject;

public class FullFeederPerception extends DebuggableObject{

    // Class Variables
    //=================

    // Distance
    private static final int MAX_FAR_FEEDER_HEIGHT = 4;
    private static final int MAX_MIDDLE_FEEDER_HEIGHT = 10;

    // Angle
    private static final double MAX_SMALL_WIDTH_BY_HEIGHT = 0.2;

    // Instance Variables
    //====================

    // Feeder corners
    private int ffbtlx,ffbtly;
    private int ffbtrx,ffbtry;
    private int ffbblx,ffbbly;
    private int ffbbrx,ffbbry;    

    // Feeder top, bottom, width, height and width by height
    private int ft;
    private int fb;
    private int fw;
    private int fh;
    private double fwbh;

    // Feeder horizontal center
    private int fhc;

    // Feeder distance
    private int fdist;

    // Feeder angle
    private int fang;    

    // Class Methods
    //===============

    // Constructor
    //-------------
    public FullFeederPerception(int ffbtlx,int ffbtly,int ffbtrx,int ffbtry,
				int ffbblx,int ffbbly,int ffbbrx,int ffbbry){
	this.ffbtlx=ffbtlx;
	this.ffbtly=ffbtly;
	this.ffbtrx=ffbtrx;
	this.ffbtry=ffbtry;
	this.ffbblx=ffbblx;
	this.ffbbly=ffbbly;
	this.ffbbrx=ffbbrx;
	this.ffbbry=ffbbry;
	// Calculate properties
	if(ffbbly>ffbbry){
	    ft=ffbtly;
	    fb=ffbbly;
	}else{
	    ft=ffbtry;
	    fb=ffbbry;
	}
	
	dprintln("Feeder top/bottom "+ft+"/"+fb);
	fw=ffbtrx-ffbtlx;
	fh=fb-ft;
	dprintln("Feeder width/height "+fw+"/"+fh);
	fhc=ffbtlx+ffbtrx/2;
	if(fh<MAX_FAR_FEEDER_HEIGHT){
	    fdist=Distance.FAR;
	    dprintln("Feeder distance is "+Distance.distanceToString(fdist));
	}else if(fh<MAX_MIDDLE_FEEDER_HEIGHT){
	    dprintln("Feeder distance is "+Distance.distanceToString(fdist));
	    fdist=Distance.MIDDLE;
	}else if(ft==0 && fb==39){
	    dprintln("Feeder distance is "+Distance.distanceToString(fdist));
	    fdist=Distance.ABUTTING;
	}else{
	    dprintln("Feeder distance is "+Distance.distanceToString(fdist));
	    fdist=Distance.NEAR;
	}
	fwbh=fw/(double)fh;
	dprintln("Feeder width by height "+fwbh);
	if(fwbh<MAX_SMALL_WIDTH_BY_HEIGHT){
	    fang=Angle.SMALL;
	    dprintln("Feeder angle is "+Angle.angleToString(fang));
	}else{
	    fang=Angle.GREAT;
	    dprintln("Feeder angle is "+Angle.angleToString(fang));
	}
    } // Ends Constructor

    // Instance Methods
    //==================

    // getTop
    public int getTop(){
	return(ft);
    } // Ends getTop

    // getBottom
    public int getBottom(){
	return(fb);
    } // Ends getBottom

    // getHorizontalCenter
    public int getHorizontalCenter(){
	return(fhc);
    } // Ends getHorizontalCenter

    // getDistance
    public int getDistance(){
	return(fdist);
    } // Ends getDistance

    // getAngle
    public int getAngle(){
	return(fang);
    } // Ends getAngle

    // toString
    public String toString(){
	return("FullFeederPerception("+ffbtlx+"/"+ffbtly+","+
	       ffbtrx+"/"+ffbtry+","+ffbblx+"/"+ffbbly+","+
	       ffbbrx+"/"+ffbbry+")");
    } // Ends toString
    
} // Ends class FullFeederPerception



