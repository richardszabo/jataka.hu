import BackPropagation.*;

class BPtest2 {
    static double in[][] = {{-1.0,-1.0,1.0},{-1.0,1.0,1.0},
			    {1.0,-1.0,1.0},{1.0,1.0,1.0},
                            {-1.0,-0.3,-1.0},{-1.0,1.0,-1.0},
			    {1.0,-1.0,-1.0},{0.8,0.8,-1.0}};
    static double out[][] = {{-1.0,0.8},{1.0,-1.0},{0.5,-1.0},{-1.0,1.0},
                             {1.0,-1.0},{0.2,0.2},{-1.0,-0.45},{1.0,1.0}};

    /**********************************************************************/
    public static void main(String args[]) {	

	int numberOfLayers = 3;
	int layers[] = new int[numberOfLayers];
	double learningRate = 0.4;
	double threshold = 0.0;
	double elasticity = 1.0;
	double calmingRate = 0.9999;
	double momentum = 0.0;
	double elastictyRate = 0.0;
	layers[0] = 3;
	layers[1] = 4;
	layers[2] = 2;
	BackPropagation bpn = new BackPropagation(numberOfLayers,
						  layers,
						  learningRate,
						  threshold,
						  elasticity,
						  calmingRate,
						  momentum,
						  elastictyRate);

	System.out.println("bpn: " + bpn);

	//bpn.propagate(in[0]);

	double err;
	for( int i = 0; i <3000; ++i ) {
	    err = 0.0;
	    bpn.learn(in[0],out[0]);
	    err += bpn.absoluteError;
	    bpn.learn(in[1],out[1]);
	    err += bpn.absoluteError;
	    bpn.learn(in[2],out[2]);
	    err += bpn.absoluteError;
	    bpn.learn(in[3],out[3]);
	    err += bpn.absoluteError;
	    bpn.learn(in[4],out[4]);
	    err += bpn.absoluteError;
	    bpn.learn(in[5],out[5]);
	    err += bpn.absoluteError;
	    bpn.learn(in[6],out[6]);
	    err += bpn.absoluteError;
	    bpn.learn(in[7],out[7]);
	    err += bpn.absoluteError;
	    if( i % 100 == 0 ) {
        	System.out.println("error:" + err);
	    }
 	}
	System.out.println("learningRate:"+ bpn.getLearningRate());
	bpn.propagate(in[0]);
	System.out.println("out:"+ bpn.output(0) + "," + bpn.output(1));
	bpn.propagate(in[1]);
	System.out.println("out:"+ bpn.output(0) + "," + bpn.output(1));
	bpn.propagate(in[2]);
	System.out.println("out:"+ bpn.output(0) + "," + bpn.output(1));
	bpn.propagate(in[3]);
	System.out.println("out:"+ bpn.output(0) + "," + bpn.output(1));	
	bpn.propagate(in[4]);
	System.out.println("out:"+ bpn.output(0) + "," + bpn.output(1));	
	bpn.propagate(in[5]);
	System.out.println("out:"+ bpn.output(0) + "," + bpn.output(1));	
	bpn.propagate(in[6]);
	System.out.println("out:"+ bpn.output(0) + "," + bpn.output(1));	
	bpn.propagate(in[7]);
	System.out.println("out:"+ bpn.output(0) + "," + bpn.output(1));	

	System.out.println("bpn: " + bpn);

	bpn.saveNeuro(".","bpn.utf");
	BackPropagation bpn2 = BackPropagation.loadNeuro(".","bpn.utf");
	
	System.out.println("bpn2: " + bpn2);
    }
}




