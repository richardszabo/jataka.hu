/* SOM - Decompiled by JODE
 * http://www.informatik.uni-oldenburg.de/~delwi/jode/jode.html
 * Send comments or bug reports to jochen@gnu.org
 */
package SOM;

import java.applet.Applet;
import java.util.Random;

public class SOM
    extends Applet
{
    private FrameWindow myFrame;
    
    public SOM() {
	Random random_1_1_ = new Random();
	myFrame = new FrameWindow(random_1_1_, "SOM demo");
    }
    
    public static void main(String[] strings_0_2_) {
	SOM som_1_3_ = new SOM();
	som_1_3_.start();	
    }
    
    public void init() {
	/* empty */
    }
    
    public void start() {
	myFrame.show();
	myFrame.init();	
	Thread thread = new Thread(myFrame);
	thread.start();	
    }
    
    public void stop() {
	myFrame.setVisible(false);
    }
}
