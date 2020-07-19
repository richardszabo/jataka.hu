package ConvexHull2D;

public class Vector2D {
   public Point2D start,end;   //a vektor vegpontjai start->end formaban
   //public double tangent,offset;

   /*------------------------------------------------------------*/
   /* a vektor altal meghatarozott egyenes homogen alakja Ax+By+C=0 */
   public double getA() {
     return start.y-end.y;
   }
   public double getB() {
     return end.x-start.x;
   }
   public double getC() {
     return start.x*end.y-end.x*start.y;
   }
   /*------------------------------------------------------------*/
   /* a vektor meredeksege */
   public double getM() {
      if( getB()!=0 ) 
         return getA()/getB();
      else
         return Double.NaN;
   }
   /*------------------------------------------------------------*/
   //a vektor szoge az x tengely pozitiv felegyenesevel, [-Pi..Pi]
   public double getAngle() {
      double M=getM(),B=getB(); 
      double a=Math.atan(M);
      if( (B>0 && M>0) || (B>0 && M<0) )
         return a;
      else if( B<0 && M<0 )
         return a+Math.PI;
      else if( B<0 && M>0 )
         return a-Math.PI;
      else if( M==0 && start.x<=end.x )
         return 0;
      else if( M==0 )
         return Math.PI;
      else if( B==0 && start.y<=end.y )
         return Math.PI/2;
      else 
         return -1*Math.PI/2;
   }
   /*------------------------------------------------------------*/
   /* vektorok egymassal bezart szoge */
   public double angle2D(Vector2D l) {
      return angle2D(this,l);
   }
   //a vektorok altal bezart szog -Pi..Pi intervallumban
   public static double angle2D(Vector2D l1,Vector2D l2) {
      double a1=l1.getAngle(),a2=l2.getAngle(); 
      if( a2-a1<-1*Math.PI )
         return a2-a1+2*Math.PI;
      else if( a2-a1>Math.PI ) 
         return a2-a1-2*Math.PI;
      else
         return a2-a1;
   }
   /*------------------------------------------------------------*/
}
