import java.lang.IllegalArgumentException;
import java.awt.Color;
import java.awt.Graphics;

public class zmodul 
{
    private double  minz, maxz;     // minim�lis, maxim�lis z koordin�ta
    private int sz�nnum;            // sz�nek sz�ma
    private Color sz�n[];           // a szinek
    private Graphics gr;            // ide kell rajzolni
    private double sz�nkezd[];      // az egyes sz�nekhez tartoz� z �rt�kek minimuma
    
    public zmodul(int sz�nnum,Color minsz�n, Color maxsz�n,double minz, double maxz)
    {
        double  reddiff, greendiff, bluediff;
        int     redmin, greenmin, bluemin;

        if( minz > maxz )
        {
            throw new IllegalArgumentException();
        }    
        this.minz       = minz;
        this.maxz       = maxz;
        this.sz�nnum    = sz�nnum;
        redmin          = minsz�n.getRed();
        greenmin        = minsz�n.getGreen();
        bluemin         = minsz�n.getBlue();
        sz�n            = new Color[sz�nnum];
        sz�nkezd        = new double[sz�nnum];
        sz�n[0]         = new Color(redmin, greenmin, bluemin);
        sz�nkezd[0]     = minz;
        reddiff         = (maxsz�n.getRed() - redmin) / sz�nnum;
        greendiff       = (maxsz�n.getGreen() - greenmin) / sz�nnum;
        bluediff        = (maxsz�n.getBlue() - bluemin) / sz�nnum;                
        for(int i=1; i < sz�nnum; ++i)
        {
            sz�n[i]     = new Color((int)(redmin + i*reddiff),(int)(greenmin + i*greendiff),(int)(bluemin + i*bluediff));
            sz�nkezd[i] = minz + i * (maxz - minz) / sz�nnum;
        }
    }

    public zmodul(int sz�nnum,Color minsz�n, Color maxsz�n)
    {
        this(sz�nnum, minsz�n, maxsz�n, 0.0, 1.0);
    }

    public void set_zminmax(double minz, double maxz)
    {
        double  reddiff, greendiff, bluediff;
        int     redmin, greenmin, bluemin;    

        if( minz > maxz )
        {
            throw new IllegalArgumentException();
        }    
        this.minz = minz;
        this.maxz = maxz;
        for(int i=1; i < sz�nnum; ++i)
        {
            sz�nkezd[i] = minz + i * (maxz - minz) / sz�nnum;
        }        
    }
    
    public void set_gr(Graphics gr)
    {
        this.gr = gr;
    }
    
    private int aktsz�nnum(double z)
    {
	if( maxz == minz )
	{
	    return 0;
	}
        int sn = (int)(sz�nnum * (z - minz) / (maxz - minz));
        
        if( sn < 0 )
        {
            return 0;
        }
        else if( sn >= sz�nnum )
        {
            return sz�nnum - 1;
        }
        return sn;
    }
    
    public void drawLine(vektor3 p1, vektor3 p2)
    {
        double znext, xpos, ypos, lxpos, lypos;
    
        if( sz�n[aktsz�nnum(p1.z)].equals(sz�n[aktsz�nnum(p2.z)]) )
        {
            gr.setColor(sz�n[aktsz�nnum(p1.z)]);
            gr.drawLine((int)p1.x,(int)p1.y,(int)p2.x,(int)p2.y);
//	    System.out.println(aktsz�nnum(p1.z)+"*: "+(int)p1.x+" "+(int)p1.y+
//" "+(int)p2.x+" "+(int)p2.y);
        }
//	System.out.println("Vegpontok: "+aktsz�nnum(p1.z)+" "+aktsz�nnum(p2.z));
	if (aktsz�nnum(p1.z) > aktsz�nnum(p2.z) )
	{
            vektor3 c =  p1;
            p1 = p2;
            p2 = c;
	}
	lxpos = p1.x;
        lypos = p1.y;
        for(int i=aktsz�nnum(p1.z); i<aktsz�nnum(p2.z); ++i)
        {
            znext = sz�nkezd[i+1];
            xpos  = (sz�nkezd[i+1] - p1.z) / (p2.z - p1.z) * (p2.x - p1.x) + p1.x;
            ypos  = (sz�nkezd[i+1] - p1.z) / (p2.z - p1.z) * (p2.y - p1.y) + p1.y;            
            gr.setColor(sz�n[i]);
            gr.drawLine((int)lxpos, (int)lypos, (int)xpos, (int)ypos);
//            System.out.println(i+": "+(int)lxpos+" "+(int)lypos+" "+(int)xpos+
//" "+(int)ypos);
            lxpos = xpos;
            lypos = ypos;
        }
        gr.setColor(sz�n[aktsz�nnum(p2.z)]);
        gr.drawLine((int)lxpos, (int)lypos, (int)p2.x, (int)p2.y);
//        System.out.println(aktsz�nnum(p2.z)+"#: "+(int)lxpos+" "+(int)lypos+
//" "+(int)p2.x+" "+(int)p2.y);
    }

}