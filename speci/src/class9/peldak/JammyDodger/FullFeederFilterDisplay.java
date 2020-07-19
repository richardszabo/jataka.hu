/*****************************************************************************/
/* File:         FullFeederFilterDisplay.java                                */
/*                                                                           */
/* Language:     Blackdown Java 1.2.2 RC4, native threads                    */
/* APIs:         Webots ALife 2.0.3                                          */
/* Description:  Displays a fragmented image filtered for full feeders.      */
/*                                                                           */
/* Author:       Torbjorn S. Dahl, Machine Learning Research Group           */
/*               Department of Computer Science, University of Bristol, UK   */
/*****************************************************************************/

import java.util.Vector;

public class FullFeederFilterDisplay extends CameraDisplay{

    // Class variables
    //=================

    private static long FULL_FEEDER;

    // Instance Variables
    //====================

    private FullFeederSense[][] ffsnss;

    // Class Methods
    //===============

    // Constructor
    //-------------
    public FullFeederFilterDisplay(KheperaController kc,int coln,int rown,
				   Vector ffsnss){
	super(kc,coln,rown);
	this.ffsnss=new FullFeederSense[coln][rown];
	for(int c=0;c<coln;c++){
	    for(int r=0;r<rown;r++){
		this.ffsnss[c][r]=(FullFeederSense)ffsnss.elementAt(c);
	    }
	}
	winplx=850;
    } // Ends Constructor

    // Instance Methods
    //==================

    // updateDisplay
    public void updateDisplay(){
	dprint("Updating display\n");
	int imgsx;
	int imgsy;
	int red;
	int green;
	int blue;
	long color;
	int pmx;
	int pmy;
	int imgin=kc.k6300_get_image();
	setColor();
	// Set vertical sub-image separation light grey
	for(int c=0;c<(coln-1);c++){
	    for(int is=0;is<IMAGE_SEPARATION;is++){
		for(int p=0;p<pixmaph;p++){
		    pmx=is+(c+1)*subimgw+c*IMAGE_SEPARATION;
		    pmy=p;
		    kc.gui_pixmap_draw_pixel(pixmap,pmx,pmy,LIGHT_GREY);
		}
	    }
	}
	boolean[][] isblack = new boolean[coln][rown];
	for(int c=0;c<coln;c++){
	    for(int r=0;r<rown;r++){
		if(!ffsnss[c][r].isFiring()){
		    isblack[c][r]=true;
		}else{
		    isblack[c][r]=ffsnss[c][r].kheperaDetected();
		}
		dprintln("Set isblack["+c+"]["+r+"] "+isblack[c][r]);
	    }
	}
	// Change sub-images
	for(int c=0;c<coln;c++){
	    for(int r=0;r<rown;r++){
		//dprint("Canging sub-image "+c+"/"+r+"\n");
		imgsx=c*subimgw;
		imgsy=r*subimgh;
		for(int x=0;x<subimgw;x++){
		    for(int y=0;y<subimgh;y++){
			if(isblack[c][r]){
			    color=kc.GUI_BLACK;
			}else{
			    // Get image colors
			    red=(int)kc.k6300_get_red(imgin,imgsx+x,imgsy+y);
			    green=
				(int)kc.k6300_get_green(imgin,imgsx+x,imgsy+y);
			    blue=(int)kc.k6300_get_blue(imgin,imgsx+x,imgsy+y);
			    // Get drawing color
			    color=getDrawingColor(red,green,blue);
			}
			// Find coordinates
			pmx=x+c*(subimgw+IMAGE_SEPARATION);
			pmy=y+r*(subimgh+IMAGE_SEPARATION);
			// Draw pixel
			kc.gui_pixmap_draw_pixel(pixmap,pmx,pmy,color);
		    }
		}
	    }
	}
	int imgout=kc.gui_image_new_from_pixmap(window,
						WIN_BORDER,WIN_BORDER,pixmap);
	kc.gui_image_show(imgout);
	//dprint("Updated sub-images\n");
    } // Ends updateDisplay
    
    // setColor
    protected void setColor(){
	FULL_FEEDER=kc.gui_color_get(FullFeederSense.FULL_FEEDER_RED,
				FullFeederSense.FULL_FEEDER_GREEN,
				FullFeederSense.FULL_FEEDER_BLUE);
    }// Ends setColor
    
    // getDrawingColor
    protected long getDrawingColor(int r,int g,int b){
	if(FullFeederSense.isFullFeederColor(r,g,b)){
	    return(FULL_FEEDER);
	}else{
	    return(kc.GUI_BLACK);
	}
    } // Ends getDrawingColor
    
     
    // toString
    // overridden
    public String toString(){
	return("FullFeederFilterDisplay");
    }

} // Ends class FullFeederFilterDisplay



















