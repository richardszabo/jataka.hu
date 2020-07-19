class dim3
{
    /**
        A vetites kozeppontja
    */
    private vektor3 c;
    /**
        Erre a pontra nezunk
    */
    private vektor3 la;
    
    /**
        Kozelsik
    */
    private double k;

    /**
        Tavolsik
    */
    private double t;    

    /**
        Kepernyotavolsag
    */
    private double d;    
    
    /**
        Latoszog
    */
    private double alfa;
    
    /**
        A kepkeret
    */
    private double umin,umax,vmin,vmax;
    
    /**
        A kepkoordinatarendszer meretei
    */
    public double kkx0,kky0,kkx1,kky1;
    
    /**
        Van-e latoszog megadva
    */
    private boolean lszog;
    
    /**
        Kepkeret kozeppontja
    */
    private vektor3 M;
    
    /**
        Kepkeret meretenek fele
    */
    private double m;
    
    /**
        Konstruktor
    */
    public dim3()
    {
        c = new vektor3(0,0,0);
        la = new vektor3(0,0,1);
        alfa = Math.PI/3;
        lszog = false;
        kkx0 = -1;
        kky0 = -1;
        kkx1 = 1;
        kky1 = 1;
        M = new vektor3(0,0,0);
    }
    
    /**
        Konstruktor
    */    
    public dim3(vektor3 c)
    {
        this.c = c;
        la = new vektor3(c.x,c.y,c.z+1);
        alfa = Math.PI/3;
        lszog = false;
        M = new vektor3(0,0,0);
    }

    /**
        Konstruktor
    */    
    public dim3(vektor3 c, vektor3 la)
    {
        this.c = c;
        this.la = la;
        alfa = Math.PI/3;
        lszog = false;
        M = new vektor3(0,0,0);
    }
    
    /**
        Kozeppont megadasa
    */
    public void set_kozeppont(vektor3 c)
    {
        this.c = c;
    }

    /**
        A nezett pont megadasa
    */
    public void set_la(vektor3 la)
    { 
        this.la = la;
    }

    public void set_latoszog(double alfa)
    {
        this.alfa = alfa;
        lszog = true;
    }
    
/*    public void get_kozeppont(vektor3 &c)
    {
        c = this.c;
    }*/

    /**
        Kepkoordinatarendszer beallitasa 
    */
    public void set_kkr(double kkx0,double kky0,double kkx1, double kky1)
    {
        this.kkx0 = kkx0;
        this.kky0 = kky0;
        this.kkx1 = kkx1;
        this.kky1 = kky1;
    }
    
    /**
        Kozelsik, tavolsik, kepernyotavolsag beallitasa
    */
    public void set_ktd(double k, double t, double d)
    {
        this.k = k;
        this.t = t;
        this.d = d;
        M.z = d;
    }

    /**
        Kozelsik lekerdezese
    */
    public double get_k()
    {
        return k;
    }

    /**
        Tavolsik lekerdezese
    */
    public double get_t()
    {
        return t;
    }

    /** 
        Kepernyotavolag lekerdezese
    */
    public double get_d()
    {
        return d;
    }
    
    /**
        Kepkeret beallitasa
    */
    public void set_keret(double umin, double vmin, double umax, double vmax)
    {
        this.umin = umin;
        this.vmin = vmin;
        this.umax = umax;
        this.vmax = vmax;
        M.z = d;
        M.x = (umin+umax) / 2;
        M.y = (vmin+vmax) / 2;
        if( (umax - umin) > (vmax - vmin))
        {
            m = (umax - umin) / 2;
        }
        else
        {
            m = (vmax - vmin) / 2;
        }
        lszog = false;
    }

    /**
        Egy terbeli pont lekepezese 
    */
    public void d2_kep(vektor3 v,vektor3 v2)
    {
        homogen v4 = new homogen(v);
        double  k1,d1,t1,d3,t3,r;
        double  ku,kv;
        double  szog;
        vektor3 irany = la.minusz(c);
        homogen hir = new homogen(irany);

        // Eltolas -c -vel
        v4.T(c.minusz());
        d1 = d - c.z;
        k1 = k - c.z;
        t1 = t - c.z;
        // Elforgatas x tengely menten
        if (irany.y != 0)
        {
            if (irany.z == 0)
            {
                szog = Math.PI/2;
            }
            else
            {
                szog = Math.atan(irany.y/irany.z);
            }
            v4.RX(szog);
            hir.RX(szog);
        }
        // Nyiras
        ku = -(M.x - c.x) / d1;
        kv = -(M.y - c.y) / d1;
        v4.KZ(ku,kv);
        // Eltolas,leptekezes
        v4.T(new vektor3(0,0,-k1));
        d3 = d1 - k1;
        t3 = t1 - k1;
        v4.S(1/(m*k1/d1));
        // Vetites
        r = 1/k1;
        v4.PZ(r);
        v2 = new vektor3(v4);
        v2.x = (v2.x+1) * (kkx1-kkx0) /2 + kkx0;
        v2.y = (v2.y+1) * (kky1-kky0) /2 + kky0;
    }
    
    /**
        Tobb terbeli pont lekepezese
    */
    public void d2_kep_tomb(int num,vektor3 v[],vektor3 v2[])
    {
        homogen     []v4 = new homogen[num];
        double      k1,d1,t1,/*d3,t3,*/r,s;
        double      ku,kv;
        int         ii;
        vektor3     cm=this.c.minusz();
        vektor3     irany=la.minusz(c);
        homogen     hir = new homogen(irany);
        double      szog;
        homogen     hM = new homogen(M);

        for (ii = 0; ii < num; ++ii)
        {
            v4[ii] = new homogen(v[ii]);
        }
        // Eltolas -c -vel
        if (c.x != 0 || c.y != 0 || c.z != 0)
        {
            for (ii = 0; ii < num; ++ii)
            {
                v4[ii].T(cm);
            }
            hM.T(cm);
        }
        d1 = d - c.z;
        k1 = k - c.z;
        t1 = t - c.z;
        if (lszog)
        {
            // Elforgatas x tengely menten
            if (irany.y != 0)
            {
                if (irany.z == 0)
                {
                    szog = Math.PI/2;
                }
                else
                {
                    szog = Math.atan(irany.y/irany.z);
                }
                for (ii = 0; ii < num; ++ii)
                {
                    v4[ii].RX(szog);
                }
                hir.RX(szog);
                hM.RX(szog);
            }
            k1 = 0.1;
            t1 = 1000;
            m = d1 * Math.tan(alfa);
        }
        else
        {
            // Nyiras
            ku = -(hM.x/hM.h) / d1;
            kv = -(hM.y/hM.h) / d1;
            if (ku != 0 || kv != 0)
            {
                for (ii = 0; ii < num; ++ii)
                {
                    v4[ii].KZ(ku,kv);
                }
            }
        }
        // Eltolas,leptekezes
        vektor3 elt2 = new vektor3(0,0,-k1);
        for (ii=0;ii<num;++ii)
        {
            v4[ii].T(elt2);
        }
        if ( (s = 1/(m*k1/d1)) != 1)
        {
            for (ii = 0; ii < num; ++ii)
            {
                v4[ii].S(s);
            }
        }
        // Vetites
        r = 1/k1;
        for (ii=0;ii<num;++ii)
        {
            v4[ii].PZ(r);
        }
        // Kepernyo koordinatarendszerere teres
        double kszx = (kkx1-kkx0) /2, kszy = (kky1-kky0) /2;
        for (ii=0;ii<num;++ii)
        {
            v2[ii] = new vektor3(v4[ii]);
            v2[ii].x = (v2[ii].x+1) * kszx + kkx0;
            v2[ii].y = (v2[ii].y+1) * kszy + kky0;
        }
    }
}