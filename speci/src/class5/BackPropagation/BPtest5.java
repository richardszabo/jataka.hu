import BackPropagation.*;

class BPtest5 {
    static double in[][] = {
	{ 1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0},
	{-1.0, 1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0},
	{-1.0,-1.0, 1.0,-1.0,-1.0,-1.0,-1.0,-1.0},
	{-1.0,-1.0,-1.0, 1.0,-1.0,-1.0,-1.0,-1.0},
	{-1.0,-1.0,-1.0,-1.0, 1.0,-1.0,-1.0,-1.0},
	{-1.0,-1.0,-1.0,-1.0,-1.0, 1.0,-1.0,-1.0},
	{-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, 1.0,-1.0},
	{-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, 1.0}};
    static double out[][] = {
	{ 1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0},
	{-1.0, 1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0},
	{-1.0,-1.0, 1.0,-1.0,-1.0,-1.0,-1.0,-1.0},
	{-1.0,-1.0,-1.0, 1.0,-1.0,-1.0,-1.0,-1.0},
	{-1.0,-1.0,-1.0,-1.0, 1.0,-1.0,-1.0,-1.0},
	{-1.0,-1.0,-1.0,-1.0,-1.0, 1.0,-1.0,-1.0},
	{-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, 1.0,-1.0},
	{-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, 1.0}};

    /**********************************************************************/
    public static void main(String args[]) {	

	int numberOfLayers = 3;
	int layers[] = new int[numberOfLayers];
	double learningRate = 0.4;
	double threshold = 0.0;
	double elasticity = 1.0;
	double calmingRate = 0.9995;
	double momentum = 0.0;
	double elasticityRate = 0.0;
        layers[0] = 8;
	layers[1] = 3;
	layers[2] = 8;
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
	int count = 0, thisTurn = 0;
	boolean stopCond = false;
	while( count < 10000 && !stopCond ) {
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
	    if( err < 0.001 ) {
		/* ha itt false ertek szerepel, akkor elszall a java! */
		stopCond = true;
	    }
	    if( count % 100 == 0 ) {
        	System.out.println("error:" + err);
		if( err > 0.1 && thisTurn > 1000 ) {
		    System.out.println("alter");
		    bpn.alterWeights();
		    bpn.setLearningRate(learningRate / 2.0 );
		    thisTurn = 0;
		}
	    }
	    ++count;
	    ++thisTurn;
 	}
	System.out.println("learning rate: " + bpn.getLearningRate());
	bpn.propagate(in[0]);
	System.out.println("hidden:"+ bpn.hidden(0) + "," + bpn.hidden(1) + "," + bpn.hidden(2));
	System.out.println("hidden:"+ isPos(bpn.hidden(0)) + "," + isPos(bpn.hidden(1)) + "," + isPos(bpn.hidden(2)));
	System.out.println("out:"+ bpn.output(0) + "," + bpn.output(1) + "," + bpn.output(2) + "," + bpn.output(3) + "," + bpn.output(4) + "," + bpn.output(5) + "," + bpn.output(6) + "," + bpn.output(7));
	bpn.propagate(in[1]);
	System.out.println("hidden:"+ bpn.hidden(0) + "," + bpn.hidden(1) + "," + bpn.hidden(2));
	System.out.println("hidden:"+ isPos(bpn.hidden(0)) + "," + isPos(bpn.hidden(1)) + "," + isPos(bpn.hidden(2)));
	System.out.println("out:"+ bpn.output(0) + "," + bpn.output(1) + "," + bpn.output(2) + "," + bpn.output(3) + "," + bpn.output(4) + "," + bpn.output(5) + "," + bpn.output(6) + "," + bpn.output(7));
	bpn.propagate(in[2]);
	System.out.println("hidden:"+ bpn.hidden(0) + "," + bpn.hidden(1) + "," + bpn.hidden(2));
	System.out.println("hidden:"+ isPos(bpn.hidden(0)) + "," + isPos(bpn.hidden(1)) + "," + isPos(bpn.hidden(2)));
	System.out.println("out:"+ bpn.output(0) + "," + bpn.output(1) + "," + bpn.output(2) + "," + bpn.output(3) + "," + bpn.output(4) + "," + bpn.output(5) + "," + bpn.output(6) + "," + bpn.output(7));
	bpn.propagate(in[3]);
	System.out.println("hidden:"+ bpn.hidden(0) + "," + bpn.hidden(1) + "," + bpn.hidden(2));	
	System.out.println("hidden:"+ isPos(bpn.hidden(0)) + "," + isPos(bpn.hidden(1)) + "," + isPos(bpn.hidden(2)));
	System.out.println("out:"+ bpn.output(0) + "," + bpn.output(1) + "," + bpn.output(2) + "," + bpn.output(3) + "," + bpn.output(4) + "," + bpn.output(5) + "," + bpn.output(6) + "," + bpn.output(7));
	bpn.propagate(in[4]);
	System.out.println("hidden:"+ bpn.hidden(0) + "," + bpn.hidden(1) + "," + bpn.hidden(2));	
	System.out.println("hidden:"+ isPos(bpn.hidden(0)) + "," + isPos(bpn.hidden(1)) + "," + isPos(bpn.hidden(2)));
	System.out.println("out:"+ bpn.output(0) + "," + bpn.output(1) + "," + bpn.output(2) + "," + bpn.output(3) + "," + bpn.output(4) + "," + bpn.output(5) + "," + bpn.output(6) + "," + bpn.output(7));
	bpn.propagate(in[5]);
	System.out.println("hidden:"+ bpn.hidden(0) + "," + bpn.hidden(1) + "," + bpn.hidden(2));	
	System.out.println("hidden:"+ isPos(bpn.hidden(0)) + "," + isPos(bpn.hidden(1)) + "," + isPos(bpn.hidden(2)));
	System.out.println("out:"+ bpn.output(0) + "," + bpn.output(1) + "," + bpn.output(2) + "," + bpn.output(3) + "," + bpn.output(4) + "," + bpn.output(5) + "," + bpn.output(6) + "," + bpn.output(7));
	bpn.propagate(in[6]);
	System.out.println("hidden:"+ bpn.hidden(0) + "," + bpn.hidden(1) + "," + bpn.hidden(2));	
	System.out.println("hidden:"+ isPos(bpn.hidden(0)) + "," + isPos(bpn.hidden(1)) + "," + isPos(bpn.hidden(2)));
	System.out.println("out:"+ bpn.output(0) + "," + bpn.output(1) + "," + bpn.output(2) + "," + bpn.output(3) + "," + bpn.output(4) + "," + bpn.output(5) + "," + bpn.output(6) + "," + bpn.output(7));
	bpn.propagate(in[7]);
	System.out.println("hidden:"+ bpn.hidden(0) + "," + bpn.hidden(1) + "," + bpn.hidden(2));	
	System.out.println("hidden:"+ isPos(bpn.hidden(0)) + "," + isPos(bpn.hidden(1)) + "," + isPos(bpn.hidden(2)));
	System.out.println("out:"+ bpn.output(0) + "," + bpn.output(1) + "," + bpn.output(2) + "," + bpn.output(3) + "," + bpn.output(4) + "," + bpn.output(5) + "," + bpn.output(6) + "," + bpn.output(7));

	System.out.println("bpn:"+ bpn);
    }

    public static char isPos(double value) { 
	if( value > 0 ) {
	    return '+';
	} else if( value < 0 ) {
	    return '-';
	} else {
	    return '0';
	}
    }
}



