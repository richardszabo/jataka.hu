import java.lang.IllegalArgumentException;
import java.awt.Color;
import java.awt.Graphics;

public class zmodul 
{
    private double  minz, maxz;     // minimális, maximális z koordináta
    private int színnum;            // színek száma
    private Color szín[];           // a szinek
    private Graphics gr;            // ide kell rajzolni
    private double színkezd[];      // az egyes színekhez tartozó z értékek minimuma
    
    public zmodul(int színnum,Color minszín, Color maxszín,double minz, double maxz)
    {
        double  reddiff, greendiff, bluediff;
        int     redmin, greenmin, bluemin;

        if( minz > maxz )
        {
            throw new IllegalArgumentException();
        }    
        this.minz       = minz;
        this.maxz       = maxz;
        this.színnum    = színnum;
        redmin          = minszín.getRed();
        greenmin        = minszín.getGreen();
        bluemin         = minszín.getBlue();
        szín            = new Color[színnum];
        színkezd        = new double[színnum];
        szín[0]         = new Color(redmin, greenmin, bluemin);
        színkezd[0]     = minz;
        reddiff         = (maxszín.getRed() - redmin) / színnum;
        greendiff       = (maxszín.getGreen() - greenmin) / színnum;
        bluediff        = (maxszín.getBlue() - bluemin) / színnum;                
        for(int i=1; i < színnum; ++i)
        {
            szín[i]     = new Color((int)(redmin + i*reddiff),(int)(greenmin + i*greendiff),(int)(bluemin + i*bluediff));
            színkezd[i] = minz + i * (maxz - minz) / színnum;
        }
    }

    public zmodul(int színnum,Color minszín, Color maxszín)
    {
        this(színnum, minszín, maxszín, 0.0, 1.0);
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
        for(int i=1; i < színnum; ++i)
        {
            színkezd[i] = minz + i * (maxz - minz) / színnum;
        }        
    }
    
    public void set_gr(Graphics gr)
    {
        this.gr = gr;
    }
    
    private int aktszínnum(double z)
    {
	if( maxz == minz )
	{
	    return 0;
	}
        int sn = (int)(színnum * (z - minz) / (maxz - minz));
        
        if( sn < 0 )
        {
            return 0;
        }
        else if( sn >= színnum )
        {
            return színnum - 1;
        }
        return sn;
    }
    
    public void drawLine(vektor3 p1, vektor3 p2)
    {
        double znext, xpos, ypos, lxpos, lypos;
    
        if( szín[aktszínnum(p1.z)].equals(szín[aktszínnum(p2.z)]) )
        {
            gr.setColor(szín[aktszínnum(p1.z)]);
            gr.drawLine((int)p1.x,(int)p1.y,(int)p2.x,(int)p2.y);
//	    System.out.println(aktszínnum(p1.z)+"*: "+(int)p1.x+" "+(int)p1.y+
//" "+(int)p2.x+" "+(int)p2.y);
        }
//	System.out.println("Vegpontok: "+aktszínnum(p1.z)+" "+aktszínnum(p2.z));
	if (aktszínnum(p1.z) > aktszínnum(p2.z) )
	{
            vektor3 c =  p1;
            p1 = p2;
            p2 = c;
	}
	lxpos = p1.x;
        lypos = p1.y;
        for(int i=aktszínnum(p1.z); i<aktszínnum(p2.z); ++i)
        {
            znext = színkezd[i+1];
            xpos  = (színkezd[i+1] - p1.z) / (p2.z - p1.z) * (p2.x - p1.x) + p1.x;
            ypos  = (színkezd[i+1] - p1.z) / (p2.z - p1.z) * (p2.y - p1.y) + p1.y;            
            gr.setColor(szín[i]);
            gr.drawLine((int)lxpos, (int)lypos, (int)xpos, (int)ypos);
//            System.out.println(i+": "+(int)lxpos+" "+(int)lypos+" "+(int)xpos+
//" "+(int)ypos);
            lxpos = xpos;
            lypos = ypos;
        }
        gr.setColor(szín[aktszínnum(p2.z)]);
        gr.drawLine((int)lxpos, (int)lypos, (int)p2.x, (int)p2.y);
//        System.out.println(aktszínnum(p2.z)+"#: "+(int)lxpos+" "+(int)lypos+
//" "+(int)p2.x+" "+(int)p2.y);
    }

}