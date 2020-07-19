import java.io.*;
import java.applet.*;
import java.awt.*;
import java.util.*;
import java.net.*;
import ConvexHull2D.*;
import ConvexHull3D.*;

class Rajzlap extends Canvas
{
    Graf_applet    fonok;
    int             egerx, egery;

    public Rajzlap(Graf_applet fonok)
    {
        super();
        this.fonok = fonok;
    }
    
    public boolean mouseDown(Event evt, int x, int y)
    {
        egerx = x;
        egery = y;
        return true;
    }
    
    public boolean mouseUp(Event evt, int x, int y)
    {
        egerx = -1;
        egery = -1;
        return true;
    }
    
    public boolean mouseDrag(Event evt, int x, int y)
    {
        if( egerx != -1 && (Math.abs(x-egerx) >= 8 || Math.abs(y-egery) >=8 ))
        {
            fonok.forgat((egery-y)*Math.PI/180,(x-egerx)*Math.PI/180,0);
            egerx = x;
            egery = y;
        }
        return true;
    }

    public void paint(Graphics g)
    {
        int i;
        int x1,y1,x2,y2,z1,z2;
        int meret;
        
        switch( fonok.algList.getSelectedIndex() )
        {
        case 0:
            g.setColor(fonok.hatterszin);
            g.fillRect(0, 0, fonok.d.width, fonok.d.height);
            g.setColor(Color.white);
            i=0;
            while( fonok.elek_2d != null && i<fonok.elek_2d.size() )
            {
                x1=(int)((Point2D)fonok.elek_2d.elementAt(i)).x;
                y1=(int)((Point2D)fonok.elek_2d.elementAt(i)).y;
                x2=(int)((Point2D)fonok.elek_2d.elementAt((i+1)%fonok.elek_2d.size())).x;
                y2=(int)((Point2D)fonok.elek_2d.elementAt((i+1)%fonok.elek_2d.size())).y;
                g.drawLine(x1, y1, x2, y2);                                               
                ++i;
            }
            g.setColor(fonok.pontszin);
            i=0;
            meret=4;
            while( fonok.point_2d != null && i<fonok.point_2d.size() )
            {
                x1=(int)((Point2D)fonok.point_2d.elementAt(i)).x;
                y1=(int)((Point2D)fonok.point_2d.elementAt(i)).y;
                g.fillArc(x1-meret/2,y1-meret/2, meret, meret, 0, 360);
                ++i;
            }
            break;
        case 1:
            switch( fonok.megjList_3d.getSelectedIndex() )
            {
            case 0: 
                g.setColor(fonok.hatterszin);
                g.fillRect(0, 0, fonok.d.width, fonok.d.height);
                g.setColor(Color.white);
                g.drawLine(fonok.d.width/2,0,fonok.d.width/2,fonok.d.height);
                g.drawLine(0,fonok.d.height/2,fonok.d.width,fonok.d.height/2);
                i=0;
                while( fonok.elek !=null && i<fonok.elek.size())
                {
                    x1=(int)fonok.eredeti[fonok.elekssz[i][0]].x+100;
                    y1=(int)fonok.eredeti[fonok.elekssz[i][0]].y+100;
                    z1=(int)fonok.eredeti[fonok.elekssz[i][0]].z-50;                
                    x2=(int)fonok.eredeti[fonok.elekssz[i][1]].x+100;
                    y2=(int)fonok.eredeti[fonok.elekssz[i][1]].y+100;
                    z2=(int)fonok.eredeti[fonok.elekssz[i][1]].z-50;
                    g.drawLine(x1,y1,x2,y2);
                    g.drawLine(fonok.d.width/2+z1,y1,fonok.d.width/2+z2,y2);
                    g.drawLine(x1,fonok.d.height/2+z1,x2,fonok.d.height/2+z2);
                    ++i;
                }
                break;
            case 1:
            case 2: 
                g.setColor(fonok.hatterszin);
                g.fillRect(0, 0, fonok.d.width, fonok.d.height);
                fonok.zm.set_gr(g);
    
                i = 0;
                while(fonok.elek != null && i<fonok.elek.size())
                {
                  if( fonok.lekepezett[fonok.elekssz[i][0]].z > 0 && fonok.lekepezett[fonok.elekssz[i][1]].z > 0)
                    {
                        if( fonok.megjList_3d.getSelectedIndex() == 1)
                        {
                            g.setColor(Color.white);
                            g.drawLine((int)fonok.lekepezett[fonok.elekssz[i][0]].x,
                              (int)fonok.lekepezett[fonok.elekssz[i][0]].y,
                              (int)fonok.lekepezett[fonok.elekssz[i][1]].x,
                              (int)fonok.lekepezett[fonok.elekssz[i][1]].y);
                        }
                        else
                        {
	                        fonok.zm.drawLine(fonok.lekepezett[fonok.elekssz[i][0]], fonok.lekepezett[fonok.elekssz[i][1]]);
	                    }
                    }                    
                    ++i;
                }                
                g.setColor(fonok.pontszin);
            
                i = 0;
                while(fonok.point != null && i<fonok.point.size())
                {
                    if( fonok.lekepezett[i].z > 0)
                    {
                        meret = 8-(int)(5 *(fonok.lekepezett[i].z-fonok.zmin)/(fonok.zmax-fonok.zmin));
                        g.fillArc((int)fonok.lekepezett[i].x-meret/2,
                        (int)fonok.lekepezett[i].y-meret/2,
                        meret, meret, 0, 360);
                    }
                    else
                    {
                        System.out.println(fonok.eredeti[i].x+" "+fonok.eredeti[i].y+" "+fonok.eredeti[i].z);
                    }
                    ++i;
                }  
                break;
            }      
        } 
    }
    
    public void update(Graphics g)
    {
        paint(g);
    }

}

public class Graf_applet extends Applet
{
    BorderLayout    rakodo=new BorderLayout();
    dim3            ddd;
    zmodul          zm;
    Random          r=new Random();
    Panel           kezeloFelulet = new Panel();
    Rajzlap         grafika = new Rajzlap(this);
    Button          futtatGomb = new Button("Futtat");       
    Button          opciokGomb = new Button("Opciók");
    Button          segitGomb = new Button("Segítség");
    Button          forgatXpGomb = new Button("X+");
    Button          forgatXmGomb = new Button("X-");
    Button          forgatYpGomb = new Button("Y+");
    Button          forgatYmGomb = new Button("Y-");
    Button          forgatZpGomb = new Button("Z+");
    Button          forgatZmGomb = new Button("Z-");
    Panel           forgatPanel = new Panel();    
    Choice          algList = new Choice();
    Choice          pontList = new Choice();
    Choice          megjList_2d = new Choice();
    Choice          megjList_3d = new Choice();
    Panel           opciok = new Panel();
    GridBagConstraints gbc = new GridBagConstraints();
    Label           hatterLabel = new Label("Háttérszín");
    Choice          hatterChoice = new Choice();
    Label           pontLabel = new Label("Pontok színe");
    Choice          pontChoice = new Choice();
    Button          OKGomb = new Button("OK");
    Button          CancelGomb = new Button("Cancel");
    vektor3         eredeti[];
    vektor3         lekepezett[];
    int             elekssz[][];
    int             number;
    Vector          point_2d = new Vector();
    Vector          elek_2d = new Vector();
    Vector          point = new Vector();
    Vector          elek;
    double          zmin, zmax;
    Dimension       d=new Dimension(400,400);
    int             utolsó_dimenzió=0;
    URL             segit_html;
    Color           hatterszin=Color.black;
    Color           pontszin=Color.red;
    Color             szin[] = {Color.black, Color.blue, Color.cyan, Color.darkGray,
                        Color.gray, Color.green, Color.lightGray, Color.magenta,
                        Color.orange, Color.pink, Color.red, Color.white, Color.yellow};
    int             uthatterszin=0, utpontszin=10;                         
    private void szinfeltolt(Choice c)
    {
        c.addItem("Fekete");
        c.addItem("Kék");
        c.addItem("Cián");
        c.addItem("Sötétszürke");
        c.addItem("Szürke");
        c.addItem("Zöld");
        c.addItem("Világosszürke");
        c.addItem("Magenta");
        c.addItem("Narancssárga");
        c.addItem("Rózsaszín");
        c.addItem("Vörös");
        c.addItem("Fehér");
        c.addItem("Sárga");
    }
                 
    public Graf_applet()
    {
        setLayout(rakodo);
        kezeloFelulet.setLayout(new GridLayout(7,1,3,30));
        forgatPanel.setLayout(new GridLayout(2,3));
        forgatPanel.add(forgatXpGomb);
        forgatPanel.add(forgatYpGomb);
        forgatPanel.add(forgatZpGomb);
        forgatPanel.add(forgatXmGomb);
        forgatPanel.add(forgatYmGomb);
        forgatPanel.add(forgatZmGomb);
        algList.addItem("Konvex Burok 2D");
        algList.addItem("Konvex Burok 3D");
        algList.select(1);
        kezeloFelulet.add(algList);
        pontList.addItem("Véletlen pontok");
        pontList.select(0);
        kezeloFelulet.add(pontList);        
        kezeloFelulet.add(futtatGomb);
        kezeloFelulet.add(forgatPanel);
        megjList_3d.addItem("Nézetek");
        megjList_3d.addItem("Drótváz");
        megjList_3d.addItem("Z-Moduláció");
        megjList_3d.select(2);        
        megjList_2d.addItem("Felülnézet");
        megjList_2d.select(0);
        kezeloFelulet.add(megjList_3d);
        kezeloFelulet.add(opciokGomb);
        //kezeloFelulet.add(segitGomb);
	gbc.gridwidth = 1;
	gbc.gridheight = 1;
	gbc.anchor = GridBagConstraints.CENTER;
	gbc.fill = GridBagConstraints.NONE;
        opciok.setLayout(new GridBagLayout());
	gbc.gridx = 0;
	gbc.gridy = 0;
        opciok.add(hatterLabel,gbc);
        szinfeltolt(hatterChoice);
        hatterChoice.select(uthatterszin);
	gbc.gridx = 1;
	gbc.gridy = 0;
        opciok.add(hatterChoice,gbc);
	gbc.gridx = 0;
	gbc.gridy = 1;
        opciok.add(pontLabel,gbc);
        szinfeltolt(pontChoice);
        pontChoice.select(utpontszin);
	gbc.gridx = 1;
	gbc.gridy = 1;
        opciok.add(pontChoice,gbc);
	gbc.gridx = 0;
	gbc.gridy = 2;
        opciok.add(OKGomb,gbc);
	gbc.gridx = 1;
	gbc.gridy = 2;
        opciok.add(CancelGomb,gbc);
        add("West",kezeloFelulet);
        add("Center",grafika);
        validate();
    }

    public String getAppletInfo()
    {
        return "Számítógépi Grafika beadandó program";
    }
    
    public void init()
    {
        ddd = new dim3(new vektor3(0,0,0),new vektor3(0,0,160));
        ddd.set_kkr(0,0,350,350);
        ddd.set_ktd(41,160,40);
        ddd.set_keret(-100,-100,100,100);
        zm = new zmodul(32, Color.white, Color.black, 0, 2);  
        try
        {
            segit_html = new URL("http://app2.html");
        }
        catch(MalformedURLException e)
        {
            
        }
    }

    public void forgat(double xv, double yv, double zv)
    {
        if(utolsó_dimenzió == 3 && (xv != 0  || yv != 0 || zv != 0))
        {
            vektor3 elore = new vektor3(0,0,100);
            vektor3 hatra = elore.minusz();
            
            // pontok forgatasa
            for(int i=0; i<point.size();++i )
            {
                homogen h = new homogen(eredeti[i]);
                if(xv!=0)
                {
                    h.T(hatra);
                    h.RX(xv);
                    h.T(elore);
                }
                if(yv!=0)
                {
                    h.T(hatra);
                    h.RY(yv);
                    h.T(elore);
                }
                if(zv!=0) h.RZ(zv);
                eredeti[i] = new vektor3(h);
            }
            ddd.d2_kep_tomb(point.size(), eredeti, lekepezett);
            grafika.repaint();
        }    
    }
    
    public boolean action(Event evt,Object what) 
    {
        double xv=0,yv=0,zv=0;
//        double gy3 = Math.sqrt(3)+0.1;    
        double gy3=2.3;
        boolean rand;
        
        System.out.println("action"+evt+"  "+what);
        if( what.equals("Futtat") )
        {
        switch(algList.getSelectedIndex())
        {
        case 0:
            number = (Math.abs(r.nextInt())%26)+3;
            rand = (pontList.getSelectedIndex() == 0);            
            point_2d.removeAllElements();

            //---------------------
            //random lista hasznalata
            {
                    for(int i=0;i<number;++i) 
                    {
                        Point2D p=new Point2D();
                        p.x=d.width*r.nextDouble(); 
                        p.y=d.height*r.nextDouble();
                        point_2d.addElement(p);
                    }
            }
            elek_2d=null;
            repaint();
            elek_2d=ConvexHull2D.createHull(point_2d);
            utolsó_dimenzió=2;
            break;
        case 1:
            number = (Math.abs(r.nextInt())%126)+4;
            rand = (pontList.getSelectedIndex() == 0);
            
            point.removeAllElements();
            if( !rand ) 
            {
                try 
                {
                    DataInputStream dis;
                    dis=new DataInputStream(new FileInputStream("pontok.dat"));
                    try 
                    {
                        number=0;
                        while(true) 
                        {
                            ++number;
                            Point3D p=new Point3D();
                            readNextPoint(dis,p);
                            point.addElement(p);
                        }
                    }
                    catch(EOFException eof) { dis.close(); }
                }
                catch(IOException io) { System.out.println("File hiba!"); }
            }
            //---------------------
            //random lista hasznalata es elmentese
            else
            {
//                try 
//                {
//                    DataOutputStream dos;
//                    dos=new DataOutputStream(new FileOutputStream("pontok.dat"));
                    for(int i=0;i<number;++i) 
                    {
                        Point3D p=new Point3D();
                        p.x=(d.width*r.nextDouble()/2-d.width/4) / gy3; 
                        p.y=(d.height*r.nextDouble()/2-d.width/4) / gy3;
                        p.z=(100-50/gy3+100/gy3*r.nextDouble()); 
//                        writeNextPoint(dos,p);
                        point.addElement(p);
                    }
//                    dos.close();
//                }
//                catch(IOException io) { System.out.println("File hiba!"); }
            }
            eredeti = new vektor3[number];
            lekepezett = new vektor3[number];
            //---------------------
            elek=ConvexHull3D.createHull(point);
            elekssz = new int[elek.size()][2];
            for(int i=0; i<elek.size(); ++i)
            {
                elekssz[i][0] = point.indexOf((Object)(((Edge)elek.elementAt(i)).start));
                elekssz[i][1] = point.indexOf((Object)(((Edge)elek.elementAt(i)).end));
            }
//            eleredeti = new vektor3[elek.size()][2];
//            ellekepezett = new vektor3[elek.size()][2];
            for(int i=0;i<point.size();++i)
            {
                eredeti[i] = new vektor3(((Point3D)(point.elementAt(i))).x, 
                    ((Point3D)(point.elementAt(i))).y,((Point3D)(point.elementAt(i))).z );
            }
            ddd.d2_kep_tomb(point.size(),eredeti, lekepezett);
            zmax=0;
             zmin=Double.MAX_VALUE;
            for(int i=0;i<point.size();++i)
            {
                if( lekepezett[i].z < zmin )
                {
                    zmin = lekepezett[i].z;
                }
                if( lekepezett[i].z > zmax )
                {
	                zmax = lekepezett[i].z;
                }
            }    
            zmin = 0.8 * zmin;
            zmax = 1.5 * zmax;
            zm.set_zminmax(zmin, zmax);
            utolsó_dimenzió=3;
            break;
            }
            grafika.repaint();
        }
        if( what.equals("X+") )
        {
            xv = Math.PI/45;    
        }
        if( what.equals("X-") )
        {
            xv = -Math.PI/45;
        }
        if( what.equals("Y+") )
        {
            yv = Math.PI/45;    
        }
        if( what.equals("Y-") )
        {
            yv = -Math.PI/45;
        }        
        if( what.equals("Z+") )
        {
            zv = Math.PI/45;    
        }
        if( what.equals("Z-") )
        {
            zv = -Math.PI/45;
        }        
        forgat(xv,yv,zv);
        if( what.equals("Konvex Burok 2D"))
        {
            kezeloFelulet.remove(megjList_3d);
            kezeloFelulet.add(megjList_2d,4);
            kezeloFelulet.validate();
        }
        if( what.equals("Konvex Burok 3D"))
        {
            kezeloFelulet.remove(megjList_2d);
            kezeloFelulet.add(megjList_3d,4);
            kezeloFelulet.validate();
        }
        if( what.equals("Opciók") )
        {
            uthatterszin=hatterChoice.getSelectedIndex();
            utpontszin=pontChoice.getSelectedIndex();
            remove(grafika);
            add("Center",opciok);
            validate();
        }
        if( what.equals("Segítség") )
        {
            getAppletContext().showDocument(segit_html,"Segitseg");
        }
        if( what.equals("Nézetek") ||
            what.equals("Drótváz") ||
            what.equals("Z-Moduláció") ||
            what.equals("Felülnézet") )
        {
            grafika.repaint();
        }
        
        if( what.equals("OK") )
        {
            hatterszin = szin[hatterChoice.getSelectedIndex()];
            pontszin = szin[pontChoice.getSelectedIndex()];
            remove(opciok);
            add("Center",grafika);
            validate();
        }
        if( what.equals("Cancel") )
        {
            hatterChoice.select(uthatterszin);
            pontChoice.select(utpontszin);
            remove(opciok);
            add("Center",grafika);
            validate();
        }
//        System.out.println("action vége");
        return true;
    }

   void readNextPoint(DataInputStream dis,Point3D p) throws IOException {
      p.x=dis.readDouble(); p.y=dis.readDouble(); p.z=dis.readDouble(); 
   }

   void writeNextPoint(DataOutputStream dos,Point3D p) throws IOException {
      dos.writeDouble(p.x); dos.writeDouble(p.y); dos.writeDouble(p.z);
   }    
}
