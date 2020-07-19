package ConvexHull3D;
import java.util.Vector;
//egyszeru algoritmus a terbeli konvex burok eloallitasara

public class ConvexHull3D {
   public static Vector createHull(Vector points) {
      int i,j,k,elso=0;
      double max=0;

      //legnagyobb x koordinataju pont keresese
      for(i=0;i<points.size();++i) {
         Point3D p=(Point3D)points.elementAt(i);
         if( max<p.x ) {
            max=p.x;
            elso=i;
         }
      }
      //ciklusban-ciklus az elso fedo haromszog megkeresesere
         //a megfelelo haromszog altal meghatarozott sikhoz kepest
         //a tobbi pont egy oldalon fekudjon
         //ha elojelvaltas tortenik kereses kozben, akkor uj pont kell
      Triangle3D kezd=new Triangle3D();
      kezd.a=(Point3D)points.elementAt(elso); 
      icimke: for(i=0;i<points.size();++i)
         jcimke: for(j=0;j<points.size();++j) {
            if( i==j || elso==i || elso==j ) continue;
            kezd.b=(Point3D)points.elementAt(i);
            kezd.c=(Point3D)points.elementAt(j);
            int elojel=0;
            kcimke: for(k=0;k<points.size();++k) {
               if( k==i || k==j || k==elso ) continue kcimke;
               double dist=kezd.dist3D((Point3D)points.elementAt(k));
               //elojelvaltas
               if( dist>0 && elojel<0 || dist<0 && elojel>0 ) continue jcimke;
               else if( dist>0 ) elojel=1; 
               else if( dist<0 ) elojel=-1;
            }
            break icimke;
         }
      //megvan a kezdo haromszog
      Vector talalt=new Vector();
      Vector talaltelek=new Vector();
      talalt.addElement(kezd);
      Vector elek=new Vector();
      Edge el;
      el=new Edge();
      el.start=kezd.a; el.end=kezd.b; 
      elek.addElement(el);
      talaltelek.addElement(el);
      el=new Edge();
      el.start=kezd.a; el.end=kezd.c; 
      elek.addElement(el);
      talaltelek.addElement(el);
      el=new Edge();
      el.start=kezd.b; el.end=kezd.c; 
      elek.addElement(el);
      talaltelek.addElement(el);
      //amig van egyoldalu el
      elcimke: for(;elek.size()>0;) {
         Triangle3D regi=null,uj,t;
         el=(Edge)elek.elementAt(0);
         boolean volt=false;
         for(i=0;i<talalt.size();++i) {
            t=(Triangle3D)talalt.elementAt(i);
            if( t.isPointIn(el.start) && t.isPointIn(el.end) ) {
               if( volt ) {
                  elek.removeElementAt(0);
                  continue elcimke;
               }
               else {
                  regi=t; volt=true;
               }
            }
         } 
         if( !volt ) return null;
         uj=new Triangle3D();
         uj.a=el.start; uj.b=el.end;
         jcimke2: for(j=0;j<points.size();++j) {
            if( regi.isPointIn(((Point3D)(points.elementAt(j)))) )
               continue jcimke2;
            uj.c=(Point3D)points.elementAt(j);
            int elojel=0;
            kcimke2: for(k=0;k<points.size();++k) {
               if( uj.a.equals(((Point3D)(points.elementAt(k)))) || 
                   uj.b.equals(((Point3D)(points.elementAt(k)))) ||
                   uj.c.equals(((Point3D)(points.elementAt(k)))) ) 
                      continue kcimke2;
               double dist=uj.dist3D((Point3D)points.elementAt(k));
               //elojelvaltas
               if( dist>0 && elojel<0 || dist<0 && elojel>0 ) continue 
                   jcimke2;
               else if( dist>0 ) elojel=1; 
               else if( dist<0 ) elojel=-1;
            }
            break jcimke2;
         }
         //if( !volt ) return null;
         talalt.addElement(uj);
         el=new Edge();
         el.start=uj.a; el.end=uj.c; 
         elek.addElement(el);
         talaltelek.addElement(el);
         el=new Edge();
         el.start=uj.b; el.end=uj.c;
         elek.addElement(el); 
         talaltelek.addElement(el);
      }
      //ismetlodo egyenesek kivagasa
      i=0;
      while(i<talaltelek.size()) {
         el=((Edge)(talaltelek.elementAt(i)));
	 j=i+1;
         while(j<talaltelek.size()) {
            Edge el2=((Edge)(talaltelek.elementAt(j)));
            if( i!=j && 
                (el.start.equals(el2.start) && el.end.equals(el2.end)) ||
                (el.start.equals(el2.end) && el.end.equals(el2.start)) )
	    {
               talaltelek.removeElement(el2);
            }
	    else
            {
		++j;
	    }
         }
	 ++i;
      }
      return talaltelek;
   }
}
