package ConvexHull3D;
//import Debug.*;
/**
   Haromszog.
*/
public class Triangle3D {
   public Point3D a,b,c;   //haromszog csucsai
   Point3D n;  //a haromszoghoz tartozo sik normalvektora
   boolean first=true;

    //a haromszog altal meghatarozott sik kiszamolasa
   //a haromszog harom csucsara teljesulnie kell: minden
   //koordinataban minden pont kulonbozo
   //a kulonbozo koordinatak miatt rengeteg feltetelvizsgalat felesleges
   public void setPlane() {
      //egy egyenesbe esnek-e?
      //c-a,b-a parhuzamossaganak vizsgalata
      //ha nem

      n=new Point3D();      
      n.x=(c.z-a.z)*(b.y-a.y)-(c.y-a.y)*(b.z-a.z);
      n.y=(c.x-a.x)*(b.z-a.z)-(c.z-a.z)*(b.x-a.x); 
      n.z=(c.y-a.y)*(b.x-a.x)-(c.x-a.x)*(b.y-a.y);
      //System.out.println("N:"+new Double(n.x)+new Double(n.y)+
      //                   new Double(n.z)); 
      first=false;
   }
   //haromszog altal meghatarozott sik tavolsaga p ponttol
   //az elojel lenyeges, az oldalakat ez alapjan lehet meghatarozni
   public double dist3D(Point3D p) {
      //if( first ) 
      setPlane();
      return n.x*(p.x-a.x)+n.y*(p.y-a.y)+n.z*(p.z-a.z); 
   }
   //a ket haromszog bezart szoge
   //az ertek [0..Pi] koze esik
   public static double angle3D(Triangle3D t1,Triangle3D t2) {
      //a harom csucsbol ki lehet szamolni a sikot
      //if( t1.first ) 
      t1.setPlane();
      //if( t2.first ) 
      t2.setPlane();
      double len1=t1.n.x*t1.n.x+t1.n.y*t1.n.y+t1.n.z*t1.n.z;       
      double len2=t2.n.x*t2.n.x+t2.n.y*t2.n.y+t2.n.z*t2.n.z;       
      double len3=(t2.n.x-t1.n.x)*(t2.n.x-t1.n.x)+
                  (t2.n.y-t1.n.y)*(t2.n.y-t1.n.y)+
                  (t2.n.z-t1.n.z)*(t2.n.z-t1.n.z);
      //System.out.println("len:"+new Double(len1)+","+
      //                   new Double(len2)+","+new Double(len3));
      return Math.acos((len1+len2-len3)/(2*Math.sqrt(len1*len2)));
   }
   public boolean isPointIn(Point3D p) {
      return this.a.equals(p) ||
             this.b.equals(p) ||
             this.c.equals(p);
   }
   public String toString() {
      return "a:"+a+" b:"+b+" c:"+c;
   }
}
