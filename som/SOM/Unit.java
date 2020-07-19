/* Unit - Decompiled by JODE
 * http://www.informatik.uni-oldenburg.de/~delwi/jode/jode.html
 * Send comments or bug reports to jochen@gnu.org
 */
package SOM;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Point;

public class Unit
{
    private double[] weights;
    private double[] shownWeights;
    int size;
    
    public Unit(int i_1_1_) {
	size = i_1_1_;
	weights = new double[i_1_1_];
	shownWeights = new double[i_1_1_];
    }
    
    public void learn(double d_1_3_, Input input_3_4_)
    throws TypeMismatchException {
	if (size != input_3_4_.getSize())
	    throw new TypeMismatchException();
	double[] ds_4_5_ = input_3_4_.getFeatures();
	for (int i_5_6_ = 0; i_5_6_ < size; i_5_6_++)
	    weights[i_5_6_] += d_1_3_ * (ds_4_5_[i_5_6_] - weights[i_5_6_]);
    }
    
    public double distance(Unit unit_1_8_)
    throws TypeMismatchException {
	if (size != unit_1_8_.size)
	    throw new TypeMismatchException();
	return distance(this, unit_1_8_);
    }
    
    public static double distance(Unit unit_0_9_, Unit unit_1_10_)
    throws TypeMismatchException {
	if (unit_0_9_.size != unit_1_10_.size)
	    throw new TypeMismatchException();
	return distance(unit_0_9_.weights, unit_1_10_.weights, unit_0_9_.size);
    }
    
    public double distance(Input input_1_12_)
    throws TypeMismatchException {
	if (size != input_1_12_.getSize())
	    throw new TypeMismatchException();
	return distance(this, input_1_12_);
    }
    
    public static double distance(Unit unit_0_13_, Input input_1_14_)
    throws TypeMismatchException {
	if (unit_0_13_.size != input_1_14_.getSize())
	    throw new TypeMismatchException();
	return distance(unit_0_13_.weights, input_1_14_.getFeatures(), unit_0_13_.size);
    }
    
    protected static double distance(double[] ds_0_15_, double[] ds_1_16_, int i_2_17_) {
	double d_3_18_ = 0.0;
	for (int i_5_19_ = 0; i_5_19_ < i_2_17_; i_5_19_++)
	    d_3_18_ += Math.abs(ds_0_15_[i_5_19_] - ds_1_16_[i_5_19_]);
	return d_3_18_;
    }
    
    protected Point coords(Graphics graphics_1_21_, Dimension dimension_2_22_) {
	Point point_3_23_ = new Point((int) ((double) dimension_2_22_.width * (weights[0] + 1.0) / 2.0), 
				      (int) ((double) dimension_2_22_.height * (weights[1] + 1.0) / 2.0));
	return point_3_23_;
    }
    
    protected Point prevCoords(Graphics graphics_1_21_, Dimension dimension_2_22_) {
	Point point_3_23_ = null;

        if( shownWeights != null ) {
	     point_3_23_ = new Point((int) ((double) dimension_2_22_.width * (shownWeights[0] + 1.0) / 2.0), 
			  	     (int) ((double) dimension_2_22_.height * (shownWeights[1] + 1.0) / 2.0));
	}
	return point_3_23_;
    }

    public void drawUnit(Graphics graphics_1_25_, Dimension dimension_2_26_, Color background) {
	Color color_4_28_ = graphics_1_25_.getColor();

	Point p1 = prevCoords(graphics_1_25_, dimension_2_26_);
	if( p1 != null ) {
	    graphics_1_25_.setColor(background);
	    graphics_1_25_.fillArc(p1.x - 3, p1.y - 3, 6, 6, 0, 360);
	}

	Point p2 = coords(graphics_1_25_, dimension_2_26_);
	graphics_1_25_.setColor(Color.yellow);
	graphics_1_25_.fillArc(p2.x - 3, p2.y - 3, 6, 6, 0, 360);

	graphics_1_25_.setColor(color_4_28_);
    }

    public void storeWeights() {
	for( int i = 0; i < size; ++i) {
	    shownWeights[i] = weights[i];
	}
    }
    
    public void drawLink(Graphics graphics_1_30_, Dimension dimension_2_31_, Unit unit_3_32_, Color background) {
	Color color_6_35_ = graphics_1_30_.getColor();

	Point p1s = prevCoords(graphics_1_30_, dimension_2_31_);
	Point p1e = unit_3_32_.prevCoords(graphics_1_30_, dimension_2_31_);
	if( p1s != null && p1e != null ) {
	    graphics_1_30_.setColor(background);
	    graphics_1_30_.drawLine(p1s.x, p1s.y, p1e.x, p1e.y);
	}

	Point p2s = coords(graphics_1_30_, dimension_2_31_);
	Point p2e = unit_3_32_.coords(graphics_1_30_, dimension_2_31_);
	graphics_1_30_.setColor(Color.cyan);
	graphics_1_30_.drawLine(p2s.x, p2s.y, p2e.x, p2e.y);

	graphics_1_30_.setColor(color_6_35_);
    }
}
