
import java.util.*;
import ConvexHull3D.*;

class Kocka {
   public Vector getPoints() {
      Vector v=new Vector();
      Point3D p=new Point3D();
      p.x=64; p.y=45; p.z=47;
      v.addElement(p);
      p=new Point3D(); 
      p.x=76; p.y=34; p.z=48;
      v.addElement(p);
      p=new Point3D(); 
      p.x=11; p.y=129; p.z=90;
      v.addElement(p);
      p=new Point3D(); 
      p.x=79; p.y=28; p.z=35;
      v.addElement(p);
      p=new Point3D(); 
      p.x=54; p.y=54; p.z=70;
      v.addElement(p);
      return v;
   }
}

class Gula {
}