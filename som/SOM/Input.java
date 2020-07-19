/* Input - Decompiled by JODE
 * http://www.informatik.uni-oldenburg.de/~delwi/jode/jode.html
 * Send comments or bug reports to jochen@gnu.org
 */
package SOM;

public class Input
{
    private double[] features;
    int size;
    
    public Input() {
	/* empty */
    }
    
    public int getSize() {
	return size;
    }
    
    public double[] getFeatures() {
	return features;
    }
    
    public void setFeatures(double[] ds_1_4_, int i_2_5_) {
	features = ds_1_4_;
	size = i_2_5_;
    }
}
