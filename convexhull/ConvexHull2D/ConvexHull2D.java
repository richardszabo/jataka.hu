package ConvexHull2D;

import java.util.Vector;

/*
class BadTypeException extends Exception {
   public BadTypeException() {
   }
   public BadTypeException(String s) {
      super(s);
   }
}
*/

/**
   Egy ponthalmaz sikbeli konvex burkat kiszamito osztaly.
*/
public class ConvexHull2D {
   /**
      A bejovo vektor egy ponthalmaz, a kimeno pedig a konvex burka,
      az elek sorrendjeben.
   */
   public static Vector createHull(Vector points) {
      //ez az eljaras rendezi a pontok vektorat x koordinata szerint
      //es meghivja gethull-t
      //lehet, hogy a vector-ban 3d pontok erkeznek es itt kellene 2d-re
      //konvertalni
      int i,j;
      /*
      //ellenorzes, hogy az adatok megfelelo tipusuak-e
      for(i=0;i<points.size();++i)
	 if( !(points.elementAt(i) instanceof Point2D) ) {
	    throw new BadTypeException(points.elementAt(i).toString());
	 }
      */
      //rendezes 1. koordinata szerint
      for(i=0;i<points.size();++i)
	 for(j=i;j<points.size();++j)
	     if( ((Point2D)points.elementAt(j)).x>((Point2D)points.elementAt(i)).x ) {
		 Object tmp=points.elementAt(j);
		 points.setElementAt(points.elementAt(i),j);
		 points.setElementAt(tmp,i);
	     }
      //a burok kiszamitasa
      return getHull(points);
   }

   static Vector getHull(Vector points) {
      Vector firsthalf,secondhalf;
      /* Ha a vektor harom elemu, akkor a harom pont a konvex burok.
         Ha hatnal kevesebb, akkor sem nagyon erdemes felezni.
         Hatra, otre lehet (otnel elso feleben legyen harom), negyet
         itt kell megoldani. */ 
      if( points.size()==1 || points.size()==2 ) 
         return points;
      else if( points.size()==3 ) {
         Point2D intern=new Point2D();
         intern.x=(((Point2D)points.elementAt(0)).x+
                   ((Point2D)points.elementAt(1)).x+
                   ((Point2D)points.elementAt(2)).x)/3;
         intern.y=(((Point2D)points.elementAt(0)).y+
                   ((Point2D)points.elementAt(1)).y+
                   ((Point2D)points.elementAt(2)).y)/3;
         Vector2D l1=new Vector2D(),l2=new Vector2D();
         l1.start=intern; l1.end=(Point2D)points.elementAt(0);
         l2.start=intern; l2.end=(Point2D)points.elementAt(1);
         double d1=Vector2D.angle2D(l1,l2);
         if( d1<0 ) d1=d1+2*Math.PI;
         l2.start=intern; l2.end=(Point2D)points.elementAt(2);
         double d2=Vector2D.angle2D(l1,l2);
         if( d2<0 ) d2=d2+2*Math.PI;
         if( d2<d1 ) {
            intern=(Point2D)points.elementAt(1);
            points.setElementAt(points.elementAt(2),1);
            points.setElementAt(intern,2);
         }
         return points;
      }
      else if( points.size()==4 ) {
         firsthalf=new Vector(3);
         firsthalf.addElement(points.elementAt(0));
         firsthalf.addElement(points.elementAt(1));
         firsthalf.addElement(points.elementAt(2));
         secondhalf=new Vector(1);
         secondhalf.addElement(points.elementAt(3));
      }
      else {
         int i,limit;
         limit=(int)((double)points.size()/2+0.5);
         firsthalf=new Vector(limit);
         secondhalf=new Vector(points.size()-limit);
         for(i=0;i<limit;++i)
            firsthalf.addElement(points.elementAt(i));
         for(i=limit;i<points.size();++i)
            secondhalf.addElement(points.elementAt(i));
      }         
      Vector poli1=getHull(firsthalf);
      Vector poli2=getHull(secondhalf);
      //System.out.println(poli1.size()+","+poli2.size());
      return mergeHull(poli1,poli2);
   }

   /*-----------------------------------------------------------------------*/
   /* A ket poligon osszekapcsolasahoz tartozo eljarasok. */
   static Vector mergeHull(Vector p1,Vector p2) {
      int i,j;
      Vector2D l1=new Vector2D(),l2=new Vector2D();
      Vector tmp;
      //egy belso pont keresese
      Point2D intern=new Point2D();
      intern.x=(((Point2D)p1.elementAt(0)).x+((Point2D)p1.elementAt(1)).x+
                ((Point2D)p1.elementAt(2)).x)/3;
      intern.y=(((Point2D)p1.elementAt(0)).y+((Point2D)p1.elementAt(1)).y+
                ((Point2D)p1.elementAt(2)).y)/3;
      //a ket sarokpont megkeresese p2-ben
      if( p2.size()>2 ) {
         int c1=0,c2=0;
         l1.start=intern; l2.start=intern;
         l1.end=(Point2D)p2.elementAt(0);
         double max=0,act;
         //kereses, amig novekszik a szog
         for(i=1;i<p2.size();++i) {
            l2.end=(Point2D)p2.elementAt(i);
            act=Math.abs(Vector2D.angle2D(l1,l2));
            if( act>max ) {
               max=act;
               c1=i;
            }
            else break;
         }
         l2.end=(Point2D)p2.elementAt(c1);
         //kereses, amig novekszik a szog
         for(i=0;i<p2.size();++i) {
            l1.end=(Point2D)p2.elementAt(i);
            act=Math.abs(Vector2D.angle2D(l1,l2));
            if( act>max ) {
               max=act;
               c2=i;
            }
         }
         if( c1>c2 ) { int t=c1;  c1=c2;  c2=t; }
         //c1 es c2 mutatja a p2-beli hatarokat
         /* if(
Point2D.distance(intern,(Point2D)p2.elementAt(mod(c1+1,p2.size())))>=
             Point2D.distance(intern,(Point2D)p2.elementAt(mod(c1-1,p2.size()))) ) {
         */
         //c1,intern,c2-vel nem egyezo iranyu-e a c1,c1+1,c2 szog
         l1.start=intern; l2.start=intern;
         l1.end=(Point2D)p2.elementAt(c1); l2.end=(Point2D)p2.elementAt(c2); 
         double ang1=Vector2D.angle2D(l1,l2);
         boolean fel;
         if( mod(c1+1,p2.size())!=c2 ) {
            fel=true;
            l1.start=(Point2D)p2.elementAt(mod(c1+1,p2.size())); 
         }
         else {   
            fel=false;
            l1.start=(Point2D)p2.elementAt(mod(c1-1,p2.size())); 
         }
         l2.start=l1.start;
         double ang2=Vector2D.angle2D(l1,l2);
         if( (fel && ang1*ang2<0) || (!fel && ang1*ang2>0) ) {
            tmp=new Vector(c2-c1+1);
            for(i=c1;i<=c2;++i) 
               tmp.addElement((Point2D)p2.elementAt(i));
         }
         else {
            tmp=new Vector(p2.size()-(c2-c1)+1);
            for(i=c2;i<p2.size();++i)
               tmp.addElement((Point2D)p2.elementAt(i));
            for(i=0;i<=c1;++i)
               tmp.addElement((Point2D)p2.elementAt(i));
         }
      }
      else if( p2.size()==2 ){
         tmp=new Vector(2);
         tmp.addElement(p2.elementAt(0));  
         tmp.addElement(p2.elementAt(1));  
      }
      else {
         tmp=new Vector(1);
         tmp.addElement(p2.elementAt(0));  
      }  
      //p1 es tmp (p2 maradeka) osszefesulese
      //tmp-beli kezdo keresese
      l1.start=intern; l1.end=(Point2D)p1.elementAt(0);
      l2.start=intern;
      l2.end=(Point2D)tmp.elementAt(0);
      double min=Vector2D.angle2D(l1,l2),act;
      if( min<0 ) min=min+2*Math.PI;
      int starttmp=0;
      for(i=1;i<tmp.size();++i) {
         l2.end=(Point2D)tmp.elementAt(i);
         act=Vector2D.angle2D(l1,l2);
         if( act<0 ) act=act+2*Math.PI;
         if( act<min ) {
            min=act;
            starttmp=i;  
         }
      }
      //osszefesules
      l1.start=intern; l2.start=intern;
      Vector poli=new Vector();
      poli.addElement(p1.elementAt(0));
      i=1; j=starttmp;
      boolean volti=false,voltj=false;
      double act1,act2;
      while( (!volti || i!=0) && (!voltj || j!=starttmp) ) {
         l1.end=(Point2D)poli.lastElement();
         l2.end=(Point2D)p1.elementAt(i);
         act1=Vector2D.angle2D(l1,l2);
         if( act1<0 ) act1=act1+2*Math.PI;
         l2.end=(Point2D)tmp.elementAt(j);
         act2=Vector2D.angle2D(l1,l2);
         if( act2<0 ) act2=act2+2*Math.PI;
         if( act1<act2 ) {
            volti=true; 
            poli.addElement(p1.elementAt(i));
            i=mod(i+1,p1.size());
         }
         else if( act1>act2 ) {
            voltj=true; 
            poli.addElement(tmp.elementAt(j));
            j=mod(j+1,tmp.size());
         }
         else {
            if( Point2D.distance(intern,(Point2D)p1.elementAt(i))<
                Point2D.distance(intern,(Point2D)tmp.elementAt(j)) )
               poli.addElement(tmp.elementAt(j));
            else
               poli.addElement(p1.elementAt(i));
            volti=true; voltj=true; 
            i=mod(i+1,tmp.size()); j=mod(j+1,tmp.size());
         }
      }
      while( !volti || i!=0 ) {
         volti=true;
         poli.addElement(p1.elementAt(i));
         i=mod(i+1,p1.size());
      }
      while( !voltj || j!=starttmp ) {
         voltj=true;
         poli.addElement(tmp.elementAt(j));
         j=mod(j+1,tmp.size());
      }
      //poli vegigjarasa a graham's scan-nal
      //az eredeti algoritmus nem jo, leall, ha start nem eleme a konvex
      //buroknak
      //ezert elozetes kereses kell
      int start=0;
      double maxx=((Point2D)poli.elementAt(0)).x; 
      for(i=1;i<poli.size();++i) {
         if( ((Point2D)poli.elementAt(i)).x>maxx ) {
            start=i;
            maxx=((Point2D)poli.elementAt(i)).x;
         }
      }
      i=start; 
      while( mod(i+1,poli.size())!=start ) {
         l1.start=(Point2D)poli.elementAt(i);
         l1.end=(Point2D)poli.elementAt(mod(i+1,poli.size()));
         l2.start=(Point2D)poli.elementAt(mod(i+1,poli.size()));
         l2.end=(Point2D)poli.elementAt(mod(i+2,poli.size()));
         if( Vector2D.angle2D(l1,l2)>0 )
            i=mod(i+1,poli.size());
         else {
            poli.removeElementAt(mod(i+1,poli.size()));
            if( i!=start ) i=mod(i-1,poli.size()); 
         }
      }
      return poli;
   }

   static int mod(int i,int j) {
      if( i<0 )
         return (j+(i%j));
      else
         return i%j;
   }
   /*-----------------------------------------------------------------------*/
}
