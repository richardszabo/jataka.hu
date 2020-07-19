import BackPropagation.*;

class BPtest {
    static double in[][] = {{-1.0,-1.0},{-1.0,1.0},{1.0,-1.0},{1.0,1.0}};
    static double out[][] = {{-1.0},{1.0},{1.0},{-1.0}};

    /**********************************************************************/
    public static void main(String args[]) {	

	int numberOfLayers = 3;
	int layers[] = new int[numberOfLayers];
	double learningRate = 0.5;
	double threshold = 0.0;
	double elasticity = 1.0;
	double calmingRate = 0.999;
	double momentum = 0.0;
	double elasticityRate = 0.0;
	layers[0] = 2;
	layers[1] = 2;
	layers[2] = 1;
	BackPropagation bpn = new BackPropagation(numberOfLayers,
						  layers,
						  learningRate,
						  threshold,
						  elasticity,
						  calmingRate,
						  momentum,
						  elasticityRate);

	System.out.println("bpn: " + bpn);

	//bpn.propagate(in[0]);

	double err;
	for( int i = 0; i <2000; ++i ) {
	    err = 0.0;
	    bpn.learn(in[0],out[0]);
	    err += bpn.absoluteError;
	    bpn.learn(in[1],out[1]);
	    err += bpn.absoluteError;
	    bpn.learn(in[2],out[2]);
	    err += bpn.absoluteError;
	    bpn.learn(in[3],out[3]);
	    err += bpn.absoluteError;
	    if( i % 100 == 0 ) {
        	System.out.println("error:" + err);
	    }
 	}
	System.out.println("learningRate:"+ bpn.getLearningRate());
	bpn.propagate(in[0]);
	System.out.println("out:"+ bpn.output(0) );
	bpn.propagate(in[1]);
	System.out.println("out:"+ bpn.output(0) );
	bpn.propagate(in[2]);
	System.out.println("out:"+ bpn.output(0) );
	bpn.propagate(in[3]);
	System.out.println("out:"+ bpn.output(0) );	

	System.out.println("bpn: " + bpn);
    }
}



