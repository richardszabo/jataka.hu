package ConvexHull3D;
/**
   Pont.
*/
public class Point3D {
   public double x,y,z;
   public String toString() {
      return new Integer((int)x).toString()+","+
             new Integer((int)y).toString()+","+
             new Integer((int)z).toString();
   }
   public boolean equals(Point3D p) {
      return this.x==p.x && this.y==p.y && this.z==p.z;
   }
}