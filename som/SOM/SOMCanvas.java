/* SOMCanvas - Decompiled by JODE
 * http://www.informatik.uni-oldenburg.de/~delwi/jode/jode.html
 * Send comments or bug reports to jochen@gnu.org
 */
package SOM;

import java.awt.Canvas;
import java.awt.Graphics;

public class SOMCanvas
    extends Canvas
{
    private SOMNet somnet;
    
    public void setNet(SOMNet somnet_1_1_) {
	somnet = somnet_1_1_;
    }
    
    public void paint(Graphics graphics_1_3_) {
	if (somnet != null)
	    somnet.drawNet(graphics_1_3_, getSize(),getBackground());
    }
}
