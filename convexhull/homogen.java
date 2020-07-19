class vektor3
{
    double x,y,z;

    /**
        Konstruktor
    */
	public vektor3(double x, double y, double z)
    {
    this.x = x;
    this.y = y;
    this.z = z;
    }
    
    /**
        Konstruktor
    */
	public vektor3(homogen h4)
    {
        if (h4.h != 0)
        {
            this.x = h4.x / h4.h;
            this.y = h4.y / h4.h;
            this.z = h4.z / h4.h;
        }
        else
        {
            this.x = this.y = this.z = 0;
        }
    }

    /**
        Egy vektor negalasa
    */
    public vektor3 minusz()
    {
        return new vektor3(-x,-y,-z);
    }
    
    /**
        Ket vektor kulonbsege
    */ 
    public vektor3 minusz(vektor3 v2)
    {
        return new vektor3(x-v2.x,y-v2.y,z-v2.z);
    }

    public vektor3 plusz(vektor3 v2)
    {
        return new vektor3(x+v2.x,y+v2.y,z+v2.z);
    }

    public vektor3 szor(double r)
    {
        return new vektor3(r*x,r*y,r*z);
    }
}

class homogen
{
    double x,y,z,h;

    /**
        Konstruktor
    */
    public homogen(double x, double y, double z, double h)
    {
        this.x = x;
        this.y = y;
        this.z = z;
        this.h = h;
    }
    
    /**
        Konstruktor
    */
    public homogen(vektor3 v)
    {
        this.x = v.x;
        this.y = v.y;
        this.z = v.z;
        this.h = 1;
    }
    
    /**
        Eltolas
    */
    public void T(vektor3 v)
    {
        x += h * v.x;
        y += h * v.y;
        z += h * v.z;
    }
    
    /**
        Kicsinyites
    */
    public void S(double s)
    {
        h /= s;
    }
    
    /**
        Nyiras Z tengely iranyaban
    */
    public void KZ(double ku, double kv)
    {
        x += z * ku;
        y += z * kv;
    }
    
    /**
        Projekcio
    */
    public void PZ(double  r)
    {
        h += z*r;
    }
    
    /**
        Elforgatas Z tengely menten
    */
    public void RZ(double alfa)
    {
        double  calfa=Math.cos(alfa),salfa=Math.sin(alfa),x1=x,y1=y;

        x = calfa * x1 + salfa * y1;
        y = -salfa * x1 + calfa * y1;
    }
    
    /**
        Elforgatas X tengely menten 
    */
    public void RX(double alfa)
    {
        double  calfa=Math.cos(alfa),salfa=Math.sin(alfa),z1=z,y1=y;

        z = calfa * z1 - salfa * y1;
        y = salfa * z1 + calfa * y1;
    }
    
    /**
        Elforgatas Y tengely menten 
    */
    public void RY(double alfa)
    {
        double  calfa=Math.cos(alfa),salfa=Math.sin(alfa),z1=z,x1=x;
        
        z = calfa * z1 + salfa * x1;
        x = -salfa * z1 + calfa * x1;
    }
}