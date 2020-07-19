/*****************************************************************************/
/* File:         CameraDisplay.java                                          */
/*                                                                           */
/* Language:     Blackdown Java 1.2.2 RC4, native threads                    */
/* APIs:         Webots ALife 2.0.3                                          */
/* Description:  Displays a fragmented k6300 image                           */
/*                                                                           */
/* Author:       Torbjorn S. Dahl, Machine Learning Research Group           */
/*               Department of Computer Science, University of Bristol, UK   */
/*****************************************************************************/

import uk.ac.bris.cs.tdahl.plancs.DebuggableObject;

public class CameraDisplay extends DebuggableObject{

    // Class constants
    //=================

    // The number of pixels around the sub-images
    protected static final int WIN_BORDER = 10;
    // The number of pixels between two sub-images
    protected static final int IMAGE_SEPARATION = 10;

    // The colours of light grey
    protected static final int LIGHT_GREY_RED = 211;
    protected static final int LIGHT_GREY_GREEN = 211;
    protected static final int LIGHT_GREY_BLUE = 211;

    // Class variables
    //=================
    
    // Light grey
    protected static long LIGHT_GREY;

    // Instance Variables
    //====================

    // The placement of the window on the screen
    protected int winplx = 550;
    protected int winply = 550;

    protected  KheperaController kc;

    protected int coln;
    protected int rown;
    protected int pixmapw;
    protected int pixmaph;
    protected int subimgw;
    protected int subimgh;
    protected int window;
    protected int pixmap;


    // Class Methods
    //===============

    // Constructor
    //-------------
    public CameraDisplay(KheperaController kc,int coln,int rown){
	//System.out.println("Constructing camera display");
	this.kc=kc;
	this.coln=coln;
	this.rown=rown;
	pixmapw=kc.K6300_WIDTH+IMAGE_SEPARATION*(coln-1);
	pixmaph=kc.K6300_HEIGHT+IMAGE_SEPARATION*(rown-1);
	subimgw=kc.K6300_WIDTH/coln;
	subimgh=kc.K6300_HEIGHT/rown;
    } // Ends Constructor

    // Instance Methods
    //==================

    // initDisplay
    public void initDisplay(){
	createImage();
    } // Ends initDisplay

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
	// Change sub-images
	for(int c=0;c<coln;c++){
	    for(int r=0;r<rown;r++){
		//dprint("Canging sub-image "+c+"/"+r+"\n");
		imgsx=c*subimgw;
		imgsy=r*subimgh;
		for(int x=0;x<subimgw;x++){
		    for(int y=0;y<subimgh;y++){
			// Get image colors
			red=(int)kc.k6300_get_red(imgin,imgsx+x,imgsy+y);
			green=(int)kc.k6300_get_green(imgin,imgsx+x,imgsy+y);
			blue=(int)kc.k6300_get_blue(imgin,imgsx+x,imgsy+y);
			// Get drawing color
			color=getDrawingColor(red,green,blue);
			pmx=x+c*(subimgw+IMAGE_SEPARATION);
			pmy=y+r*(subimgh+IMAGE_SEPARATION);
			//dprint("Got drawing color"+color+"\n");
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

    // createImage
    private void createImage(){
	int pmx;
	int pmy;
	int ww=kc.K6300_WIDTH+IMAGE_SEPARATION*(coln-1)+2*WIN_BORDER;
	int wh=kc.K6300_HEIGHT+IMAGE_SEPARATION*(rown-1)+2*WIN_BORDER;
	window=kc.gui_window_new(toString(),winplx,winply,ww,wh);
	pixmap=kc.gui_pixmap_new_from_size(pixmapw,pixmaph);
	// Set vertical sub-image separation black
	for(int c=0;c<(coln-1);c++){
	    for(int is=0;is<IMAGE_SEPARATION;is++){
		for(int p=0;p<pixmaph;p++){
		    pmx=is+(c+1)*subimgw+c*IMAGE_SEPARATION;
		    pmy=p;
		    kc.gui_pixmap_draw_pixel(pixmap,pmx,pmy,kc.GUI_BLACK);
		}
	    }
	}	
	// Create sub-images
	for(int c=0;c<coln;c++){
	    for(int r=0;r<rown;r++){
		//System.out.print("CameraDisplay: Creating black sub-image "
		//		 +c+"/"+r+"\n");
		for(int x=0;x<subimgw;x++){
		    for(int y=0;y<subimgh;y++){
			pmx=x+c*(subimgw+IMAGE_SEPARATION);
			pmy=y+r*(subimgh+IMAGE_SEPARATION);
			kc.gui_pixmap_draw_pixel(pixmap,pmx,pmy,kc.GUI_BLACK);
		    }
		}
	    }
	}
	//System.out.print("CameraDisplay: Created subimages\n");
	kc.gui_image_new_from_pixmap(window,WIN_BORDER,WIN_BORDER,pixmap);
	kc.gui_window_show(window);
	//System.out.print("CameraDisplay: Created display\n");
    } // Ends createCameraImage
    
    // setColor
    protected void setColor(){
	LIGHT_GREY=kc.gui_color_get(LIGHT_GREY_RED,
				    LIGHT_GREY_GREEN,
				    LIGHT_GREY_BLUE);
    }// Ends setColor
    
    // getDrawingColor
    protected long getDrawingColor(int r,int g,int b){
	return(kc.gui_color_get(r,g,b));
    } // Ends getDrawingColor

    // toString
    // overridden
    public String toString(){
	return("CameraDisplay");
    }

} // Ends class CameraDisplay



















