/*****************************************************************************/
/* File:         SubImageSensor.java                                         */
/*                                                                           */
/* Language:     Blackdown Java 1.2.2 RC4, native threads                    */
/* APIs:         Webots ALife 2.0.3                                          */
/* Description:  Takes a copy of a part of a k6300 image.                    */
/*                                                                           */
/* Author:       Torbjorn S. Dahl, Machine Learning Research Group           */
/*               Department of Computer Science, University of Bristol, UK   */
/*****************************************************************************/

import java.util.Vector;

import uk.ac.bris.cs.tdahl.plancs.NeuralCircuit;

public class SubImageSensor extends NeuralCircuit{

    // Instance Variables
    //====================
    
    // KheperaController
    private KheperaController kc;

    // Sub-image dimensions
    private int sx;
    private int ex;
    private int sy;
    private int ey;
    private int w;
    private int h;

    // Image
    private int img;
    private short[][] imgr;
    private short[][] imgg;
    private short[][] imgb;
    
    // Class Methods
    //===============

    // Constructor
    //-------------

    // Constructor
    public SubImageSensor(NeuralController nc,int sx,int ex,int sy,int ey){
	super(nc);
	this.kc=nc;
	setSubImageData(sx,ex,sy,ey);
    } // Ends Constructor

    // Instance Methods
    //==================

    // setSubImageData
    private void setSubImageData(int sx,int ex,int sy,int ey){
	this.sx=sx;
	this.ex=ex;
	this.sy=sy;
	this.ey=ey;
	w=ex-sx;
	h=ey-sy;
	imgr=new short[w][h];
	imgg=new short[w][h];
	imgb=new short[w][h];
    } // Ends setSubImageData

    // Evaluate
    //----------
    // overrides
    protected void evaluate(){
	// Get Image
	img=kc.k6300_get_image();
	for(int x=0;x<w;x++){
	    for(int y=0;y<h;y++){
		imgr[x][y]=kc.k6300_get_red(img,sx+x,sy+y);
		imgg[x][y]=kc.k6300_get_green(img,sx+x,sy+y);
		imgb[x][y]=kc.k6300_get_blue(img,sx+x,sy+y);
	    }
	}
    } // Ends Evaluate
    
    // getRed
    public int getRedInteger(int x, int y){
	return(imgr[x][y]);
    } // Ends getRedInteger
    
    // getGreen
    public int getGreenInteger(int x, int y){
	return(imgg[x][y]);
    } // Ends getGreenInteger

    // getBlue
    public int getBlueInteger(int x, int y){
	return(imgb[x][y]);
    } // Ends getBlueInteger

    // getTop
    public int getTop(){
	return(sy);
    } // Ends getTop

    // getBottom
    public int getBottom(){
	return(ey);
    } // Ends getBottom

    // getHeight
    public int getHeight(){
	return(h);
    } // Ends getHeight

    // getWidth
    public int getWidth(){
	return(w);
    } // Ends getWidth

    // toString
    // overrides
    public String toString(){
	return("SubImageSensor("+sx+","+ex+","+sy+","+ey+")");
    } // Ends toString

} // Ends class SubImageSensor













