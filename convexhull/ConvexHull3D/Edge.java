package ConvexHull3D;

public class Edge {
   public Point3D start,end;
   public String toString() {
      return "start:"+start+" end:"+end;
   }
   public boolean equals(Object o) {
      if( o instanceof Edge ) {
         if( ((Edge)o).start==start && ((Edge)o).end==end ||
             ((Edge)o).start==end && ((Edge)o).end==start )
         return true;
      }
      return false;
   }
}

