package ConvexHull2D;

public class Point2D {
   public double x,y;
   public double distance(Point2D p) {
      return distance(this,p);
   } 
   public static double distance(Point2D p1,Point2D p2) {
      return Math.sqrt((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y));
   } 
}
