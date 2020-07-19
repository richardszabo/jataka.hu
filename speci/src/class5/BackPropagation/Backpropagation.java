package backpropagation;

import java.util.Random;
import java.io.FileReader;
import java.io.BufferedReader;
import java.io.File;  // for UTF
import java.io.RandomAccessFile; // for UTF
import java.io.IOException;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.PrintWriter;
import java.util.StringTokenizer;
import java.util.GregorianCalendar;
import java.text.SimpleDateFormat;

import gnu.regexp.*;

/****************************************************************
 *
 * Implementation of the well-known backpropagation algorithm
 * with optional number of layers. <P>
 * @Book{Szabo01,
 * author =       "R. Szabó",
 * title =        "Mobil robotok szimulációja",
 * publisher =    "Eötvös Kiadó",
 * year =         "2001",
 * }
 * 
 * Author: Richard Szabo <BR>
 *
 ****************************************************************/
public class Backpropagation {

    /****************************************************************/
    /** 
     * Learning rate of the network.
     */
    protected double learningRate = 0.01;	
    /**
     * Threshold of the sigmoid function.
     */
    protected double theta = 0.0;		
    /**
     * Elasticity of the sigmoid function. 
     */
    protected double elasticity = 1.0;	
    /**
     * Calming rate of the learning rate: 1.0 means constant learning rate.
     */
    protected double calmingRate = 1.0;   	
    /** 
     * Constant determining the influence of the previous weight change
     * on the actual. 0.0 means no influence.
     */
    protected double momentum = 0.0;
    /**
     * Changing rate of the elasticity. It must be in the [0.0,1.0]. 
     */
    protected double elasticityRate = 0.0;	

    /****************************************************************/
    /**
     * Number of the layers, must be at least 2. 
     */
    protected int numberOfLayers;	
    /**
     * Number of the neurons in layers, must be at least 1. 
     */
    protected int numberOfNeurons[];	

    /****************************************************************/
    /** 
     * Layers of the network. 
     */
    protected Neuron layers[][];

    /****************************************************************/
    /**
     * Summed absolute error on the components of the output vector.
     */
    public double absoluteError;

    /****************************************************************
     *
     * Basic constructor.
     *
     ****************************************************************/
    public Backpropagation(int numberOfLayers, 
			   int numberOfNeurons[]) {

        if( !(numberOfLayers >= 2) ) {
	    System.out.println("Wrong number of layers!");
	    return;
        }

        if( numberOfNeurons.length != numberOfLayers ) {
	    System.out.println("Wrong size of layer descriptor array!");
	    return;
	}

        boolean faulty = false;
   	for( int i = 0; i < numberOfNeurons.length && !faulty; ++i ) {
  	    if( !(numberOfNeurons[i] > 0) ) {
	        faulty = true;
	    }
	}
	if( faulty ) {
	    System.out.println("Wrong size of layer descriptor array!");
	    return;
	}
        this.numberOfLayers = numberOfLayers;
        this.numberOfNeurons = numberOfNeurons;

        layers = new Neuron[numberOfLayers][];
        /* input layer is just implicity existing */
        for( int i = 1; i < numberOfLayers; ++i ) {
            layers[i] = new Neuron[numberOfNeurons[i]];
            for( int j = 0; j < numberOfNeurons[i]; ++j) {
                layers[i][j] = new Neuron(this,numberOfNeurons[i - 1]);
            }
        }

        /* initialization */
	this.init();
    }

    /****************************************************************
     *
     * Creation of a new BackProp network with lots of parameters.
     *
     ****************************************************************/
    public Backpropagation(int numberOfLayers, 
			   int numberOfNeurons[], 
			   double learningRate, 
			   double theta, 
			   double elasticity,
		           double calmingRate,
			   double momentum,
			   double elasticityRate) {

	/* checking the adequacy of the parameters */
	this(numberOfLayers,numberOfNeurons);

	if( !(learningRate > 0.0 && learningRate <= 1.0) ) {
	    System.out.println("Invalid learning rate!");
	    return;
        }

	if( !(calmingRate > 0.0 && calmingRate <= 1.0) ) {
	    System.out.println("Invalid calmingRate!");
	    return;
        }

	if( !(momentum >= 0.0 && momentum <= 1.0) ) {
	    System.out.println("Invalid momentum!");
	    return;
        }

	if( !(elasticityRate >= 0.0 && elasticityRate <= 1.0) ) {
	    System.out.println("Invalid elasticityRate!");
	    return;
        }

	/* creation of the network structure */
        this.learningRate = learningRate;
        this.theta = theta;
        this.elasticity = elasticity;
        this.calmingRate = calmingRate;
        this.momentum = momentum;
        this.elasticityRate = elasticityRate;
    }

    /****************************************************************
     *
     * Propagates the network input to get the output.
     * Output is in the (-1,1) interval.
     *
     ****************************************************************/
    public void propagate(double input[]) {
        /* checking the dimension of the input vector */
        if( numberOfNeurons[0] != input.length ) {
	    System.out.println("Wrong number of input!");
	    return;
	}
        for( int i = 1; i < numberOfLayers; ++i ) {
       	    double output[] = new double[numberOfNeurons[i]];
            for( int j = 0; j < numberOfNeurons[i]; ++j ) {
                output[j] = layers[i][j].propagate(input);
            }
            input = output;
        }
    }

    /****************************************************************
     *
     * Teaches the network according to the input-output value pair. 
     * Output is in the (-1,1) interval.
     *
     ****************************************************************/
    public void learn(double input[],double output[]) {
        /* checking the dimension of the output vector */
        if( numberOfNeurons[numberOfLayers - 1] != output.length ) {
	    System.out.println("Wrong number of output!");
	    return;
	}

        /* forward phase */
        propagate(input);

        /* backward phase - output layer */
        double error[] = new double[numberOfNeurons[numberOfLayers - 1]];
        double prevError[];
        absoluteError = 0.0;
        for( int j = 0; j < numberOfNeurons[numberOfLayers - 1]; ++j ) {
	    /* calculation of the error term of this neuron */
            error[j] = (output[j] - layers[numberOfLayers - 1][j].output) * 
                       d1sigmoid(layers[numberOfLayers - 1][j].state);
	    /* learning with the error term  */
            layers[numberOfLayers - 1][j].learn(error[j]);
	    /* calculation of network error */
            absoluteError += Math.abs(error[j]);
        }

        /* backward phase - hidden layers */
        for( int i = numberOfLayers - 2; i > 0 ; --i ) {
            prevError = error;
            error = new double[numberOfNeurons[i]];
            for( int j = 0; j < numberOfNeurons[i]; ++j ) {
		/* calculation of the error term of this neuron */
		/* using the next layers' error */
                for( int k = 0; k < numberOfNeurons[i + 1]; ++k ) {
                    error[j] += prevError[k] * layers[i + 1][k].weights[j];
                }
                error[j] *= d1sigmoid(layers[i][j].state);
		/* learning with the error term  */
                layers[i][j].learn(error[j]);
            }
        }

	/* diminishing the learning rate */
	learningRate *= calmingRate;

	/* increasing temperature */
        elasticity += (1.0 - elasticity) * elasticityRate;
    }

    /****************************************************************
     *
     * Alteration of the network with Gaussian.
     *
     ****************************************************************/	
    public void alterWeights(){
	Random random = new Random();

        for( int i = 1; i < numberOfLayers; ++i ) {
            for( int j = 0; j < numberOfNeurons[i]; ++j ) {
                layers[i][j].alter(random);
            }
        }
    }

    /**
     * Structure file contstants.
     */
    final static String nameStr = "Backpropagation network structure description";
    final static String authorStr = "@author: Richard Szabo";
    final static String versionStr = "Version: 0.1";
    final static String dateStr = "Creation: ";

    final static String layersStr = "layers";
    final static String neuronsStr = "neurons";
    final static String learningRateStr = "learning rate";
    final static String thetaStr = "theta";
    final static String elasticityStr = "elasticity";
    final static String calmingRateStr = "calming rate";
    final static String momentumStr = "momentum";
    final static String elasticityRateStr = "elasticity rate";
    
    /****************************************************************
     *
     * Saves the network structure to textfile.
     *
     ****************************************************************/
    public void saveStructure(String filename) throws IOException {

	// creating the structure file
	PrintWriter pw = new PrintWriter(new FileWriter(filename));

	// Format the current time.
	GregorianCalendar gc = new GregorianCalendar();
	SimpleDateFormat formatter = new SimpleDateFormat ("yyyy-MMM-dd");
	String formattedStr = formatter.format(gc.getTime());

	// writing the header information
	pw.println("/* " + nameStr + " */");
	pw.println("/* " + authorStr + " */");
	pw.println("/* " + versionStr + " */");
	pw.println("/* " + dateStr + formattedStr + " */");

	// empty line
	pw.println("");

	// writing the layer number 
	pw.println(layersStr + ": " + numberOfLayers);
	
	// writing the neuron number by layer
	pw.print(neuronsStr + ":");
        for( int i = 0; i < numberOfLayers; ++i ) {
	    pw.print(" " + numberOfNeurons[i]);
        }
	pw.println("");

	// writing learning rate
	pw.println(learningRateStr + ": " + learningRate);

	// writing theta
	pw.println(thetaStr + ": " + theta);

	// writing elasticity
	pw.println(elasticityStr + ": " + elasticity);

	// writing calming rate
	pw.println(calmingRateStr + ": " + calmingRate);

	// writing momentum
	pw.println(momentumStr + ": " + momentum);

	// writing elasticity rate
	pw.println(elasticityRateStr + ": " + elasticityRate);

	// closing the written file
	pw.close();

    }

    /****************************************************************
     *
     * Loads the network structure from textfile.
     *
     ****************************************************************/
    public static Backpropagation loadStructure(String filename) throws FileNotFoundException, IOException {

	// regular expressions to search for 
	RE layersRe         = null;
	RE neuronsRe        = null;
	RE learningRateRe   = null;
	RE thetaRe          = null;
	RE elasticityRe     = null;
	RE calmingRateRe    = null;
	RE momentumRe       = null;
	RE elasticityRateRe = null;
	try {
	    layersRe         = new RE(layersStr + ": (.+)");
	    neuronsRe        = new RE(neuronsStr + ":(( .+)+)");
	    learningRateRe   = new RE(learningRateStr + ": (.+)");
	    thetaRe          = new RE(thetaStr + ": (.+)");
	    elasticityRe     = new RE(elasticityStr + ": (.+)");
	    calmingRateRe    = new RE(calmingRateStr + ": (.+)");
	    momentumRe       = new RE(momentumStr + ": (.+)");
	    elasticityRateRe = new RE(elasticityRateStr + ": (.+)");
	} catch(REException ree) {
	    throw new IOException("Invalid regexp:" + ree);
	}

	// creation of an empty network
	Backpropagation bpn = new Backpropagation();    

	// opening the network structure file
	BufferedReader br = new BufferedReader(new FileReader(filename));

	String line;
	REMatch rem = null;
	boolean layersSet = false;

	// reading the file line by line
	while( (line = br.readLine()) != null ) {

	    String which = "";
	    try {
		// reading the layer number 
		rem = layersRe.getMatch(line);
		if( rem != null ) {
		    bpn.numberOfLayers = Integer.parseInt(rem.toString(1));
		    bpn.layers = new Neuron[bpn.numberOfLayers][];
		
		    layersSet = true;		
		}

		// reading the neuron number by layer
		rem = neuronsRe.getMatch(line);
		if( rem != null && layersSet ) {
		    StringTokenizer st = new StringTokenizer(rem.toString(1));

		    if( bpn.numberOfLayers != st.countTokens() ) {
			throw new IOException("Invalid number of layers: " + 
					      bpn.numberOfLayers + " " +
					      st.countTokens());
		    }

		    bpn.numberOfNeurons = new int[bpn.numberOfLayers];

		    /* input layer is just implicity existing */
		    bpn.numberOfNeurons[0] = Integer.parseInt(st.nextToken());
		    for( int i = 1; i < bpn.numberOfLayers; ++i ) {
			bpn.numberOfNeurons[i] = Integer.parseInt(st.nextToken());
			bpn.layers[i] = new Neuron[bpn.numberOfNeurons[i]];
			for( int j = 0; j < bpn.numberOfNeurons[i]; ++j) {
			    bpn.layers[i][j] = new Neuron(bpn,bpn.numberOfNeurons[i - 1]);
			}
		    }
		    
		
		} else if( rem != null ) {		    
		    // layers setting must precede neurons
		    throw new IOException("Layers setting must precede neurons.");
		}

		// reading learning rate
		which = learningRateStr;
		rem = learningRateRe.getMatch(line);
		if( rem != null ) {
		    bpn.learningRate = Double.parseDouble(rem.toString(1));
		}

		// reading theta
		which = thetaStr;
		rem = thetaRe.getMatch(line);
		if( rem != null ) {
		    bpn.theta = Double.parseDouble(rem.toString(1));
		}

		// reading elasticity
		which = elasticityStr;
		rem = elasticityRe.getMatch(line);
		if( rem != null ) {
		    bpn.elasticity = Double.parseDouble(rem.toString(1));
		}

		// reading calming rate
		which = calmingRateStr;
		rem = calmingRateRe.getMatch(line);
		if( rem != null ) {
		    bpn.calmingRate = Double.parseDouble(rem.toString(1));
		}

		// reading momentum
		which = momentumStr;
		rem = momentumRe.getMatch(line);
		if( rem != null ) {
		    bpn.momentum = Double.parseDouble(rem.toString(1));
		}

		// reading elasticity rate
		which = elasticityRateStr;
		rem = elasticityRateRe.getMatch(line);
		if( rem != null ) {
		    bpn.elasticityRate = Double.parseDouble(rem.toString(1));
		}

	    } catch(NumberFormatException nfe) {
		String s = "";
		if(rem != null ) {
		    s = rem.toString(1);
		}
		throw new IOException("Invalid " + which + ":" + s);
	    }

	}

	// init network weights
	bpn.init();

	return bpn;
    }

    /****************************************************************
     *
     * Saves the learned network weights in UTF format.
     *
     ****************************************************************/
    public void saveNeuro(String path, String name){
	try{
	    File rawfile = new File(path, name);
	    try{
		RandomAccessFile file = new RandomAccessFile(rawfile,"rw");
			
		/* writing the file type */
		file.writeUTF("BPNN");

		/* saving the network parameters */
		file.writeInt(numberOfLayers);
		for( int i = 0; i < numberOfLayers; ++i ) {
		    file.writeInt(numberOfNeurons[i]);		    
		}
		file.writeDouble(learningRate);
		file.writeDouble(theta);
		file.writeDouble(elasticity);
		file.writeDouble(calmingRate);
		file.writeDouble(momentum);
		file.writeDouble(elasticityRate);

		/* saving the network weights */
		for( int i = 1; i < numberOfLayers; ++i ) {
		    for( int j = 0; j < numberOfNeurons[i]; ++j ) {
			layers[i][j].writeUTF(file);
		    }
		}
				
		file.close();
	    } catch(IllegalArgumentException iae){
		System.out.println("Argument error:" +  iae);
	    }
	} catch(IOException ioe){	
	    System.out.println("File write error:" +  ioe);
	} catch(SecurityException se) { 
	    System.out.println("Security exception:" +  se);
	}
    }

    /****************************************************************
     *
     * Loads a previously learned network from UTF format.
     *
     ****************************************************************/
    public static Backpropagation loadNeuro(String path, String name){

	if(name == null || path == null) return null;
		
	try{
	    File rawfile = new File(path, name);
	    try{
		RandomAccessFile file = new RandomAccessFile(rawfile,"r");

		/* checking the file type */
		if(file.readUTF().compareTo("BPNN") != 0){
		    file.close();
		    System.out.println("Bad network description file!");
		    return null;
		}
		/* loading the network structure parameters and */
		/* creating the network structure */
		Backpropagation bpn = new Backpropagation();
		bpn.numberOfLayers = file.readInt();
		bpn.numberOfNeurons = new int[bpn.numberOfLayers];
		for( int i = 0; i < bpn.numberOfLayers; ++i ) {
		    bpn.numberOfNeurons[i] = file.readInt();
		}
		
		bpn.layers = new Neuron[bpn.numberOfLayers][];
		/* input layer is just implicity existing */
		for( int i = 1; i < bpn.numberOfLayers; ++i ) {
		    bpn.layers[i] = new Neuron[bpn.numberOfNeurons[i]];
		    for( int j = 0; j < bpn.numberOfNeurons[i]; ++j) {
			bpn.layers[i][j] = new Neuron(bpn,
						      bpn.numberOfNeurons[i - 1]);
		    }
		}
		
		/* loading the network parameters */
		bpn.learningRate = file.readDouble();
		bpn.theta = file.readDouble();
		bpn.elasticity = file.readDouble();
		bpn.calmingRate = file.readDouble();
		bpn.momentum = file.readDouble();
		bpn.elasticityRate = file.readDouble();
				
		/* saving the network weights */
		for( int i = 1; i < bpn.numberOfLayers; ++i ) {
		    for( int j = 0; j < bpn.numberOfNeurons[i]; ++j ) {
			bpn.layers[i][j].readUTF(file);
		    }
		}
								
		file.close();

		return bpn;

	    } catch(IllegalArgumentException iae){
		System.out.println("Argument error:" +  iae);
	    }
	} catch(IOException ioe){	
	    System.out.println("File read error:" +  ioe);
	} catch(SecurityException se) { 
	    System.out.println("Security exception:" +  se);
	}

	return null;
    }

    /****************************************************************
     *
     * String representation of the network.
     *
     ****************************************************************/	
    public String toString() {
        String s = new String();
	s += "learningRate=" + learningRate + "\n" + 
	     "theta=" + theta + "\n" + 
	     "elasticity=" + elasticity + "\n" + 
	     "calmingRate=" + calmingRate + "\n" + 
	     "momentum=" + momentum + "\n" + 
	     "elasticityRate=" + elasticityRate + "\n";
        for( int i = 1; i < numberOfLayers; ++i ) {
            s += "layer[" + i + "]:\n"; 
            for( int j = 0; j < numberOfNeurons[i]; ++j ) {
                s += "neuron[" + j + "]:\n"; 
                for( int k = 0; k < numberOfNeurons[i - 1] + 1; ++k ) {
                    s += layers[i][j].weights[k] + ", ";
                }
                s += "\n";
            }
            s += "\n";
        }
        return s;
    }

    /****************************************************************
     *
     * Returns network output of a given neuron.
     *
     ****************************************************************/
    public double output(int i){
        return layers[numberOfLayers - 1][i].output;
    }

   
    /****************************************************************
     *
     * Returns the output value of the jth hidden neuron in the ith layer. 
     * This function is necessary for the example of Mitchell.
     *
     ****************************************************************/
    public double hidden(int i, int j){
        return layers[i][j].output;
    }

    /****************************************************************
     *
     * Returns the actual learning rate.
     *
     ****************************************************************/
    public double getLearningRate(){
        return learningRate;
    }

    /****************************************************************
     *
     * Sets the learning rate.
     *
     ****************************************************************/
    public void setLearningRate(double learningRate){
        this.learningRate = learningRate;
    }


    /****************************************************************
     *
     * Returns the theta.
     *
     ****************************************************************/
    public double getTheta(){
        return theta;
    }

    /****************************************************************
     *
     * Sets the theta.
     *
     ****************************************************************/
    public void setTheta(double theta){
        this.theta = theta;
    }

    /****************************************************************
     *
     * Returns the actual elasticity.
     *
     ****************************************************************/
    public double getElasticity(){
        return elasticity;
    }

    /****************************************************************
     *
     * Sets the elasticity.
     *
     ****************************************************************/
    public void setElasticity(double elasticity){
        this.elasticity = elasticity;
    }

    /****************************************************************
     *
     * Returns the calming rate.
     *
     ****************************************************************/
    public double getCalmingRate(){
        return calmingRate;
    }

    /****************************************************************
     *
     * Sets the calming rate.
     *
     ****************************************************************/
    public void setCalmingRate(double calmingRate){
        this.calmingRate = calmingRate;
    }

    /****************************************************************
     *
     * Returns the momentum.
     *
     ****************************************************************/
    public double getMomentum(){
        return momentum;
    }

    /****************************************************************
     *
     * Sets the momentum.
     *
     ****************************************************************/
    public void setMomentum(double momentum){
        this.momentum = momentum;
    }

    /****************************************************************
     *
     * Returns the elasticity rate.
     *
     ****************************************************************/
    public double getElasticityRate(){
        return elasticityRate;
    }

    /****************************************************************
     *
     * Sets the elasticity rate.
     *
     ****************************************************************/
    public void setElasticityRate(double elasticityRate){
        this.elasticityRate = elasticityRate;
    }

    /****************************************************************
     *
     * Hidden constructor of the class to be used in the loadNeuro 
     * static method.
     *
     ****************************************************************/	
    protected Backpropagation() {
    }

    /****************************************************************
     *
     * Sigmoid function working in the (-1,1) interval.
     *
     ****************************************************************/	
    protected double sigmoid(double x){
	double r;
	if( x == Double.POSITIVE_INFINITY ) {
	    return 1.0;	    
	} else if( x == Double.NEGATIVE_INFINITY ) {
	    return -1.0;	    
	} else if ( x == Double.NaN || x == Float.NaN ) {
	    r = Math.exp(theta);
	} else {
	    r = Math.exp(-1.0 * elasticity * x + theta);
	    if( r == Double.NaN ) {  // x was small, around 0
		r = 1.0;
	    } else if( r == Double.POSITIVE_INFINITY ) {
		return -1.0;
	    } else if( x == Double.NEGATIVE_INFINITY ) {
		return -1.0;
	    }
	}
        return 2.0 / (1.0 + r) - 1;
    }

    /****************************************************************
     *
     * First derivative of the sigmoid function.
     *
     ****************************************************************/
    protected double d1sigmoid(double x){ 
	double r;
	if( x == Double.POSITIVE_INFINITY ) {
	    return 0.0;	    
	} else if( x == Double.NEGATIVE_INFINITY ) {
	    return 0.5 * elasticity;	    
	} else if ( x == Double.NaN || x == Float.NaN ) {
	    r = Math.exp(theta);	    
	} else {
	    r = Math.exp(-1.0 * elasticity * x + theta);
	    if( r == Double.NaN ) {  // x was small, around 0
		r = 1.0;
	    } else if( r == Double.POSITIVE_INFINITY ) {
		return 0.0;
	    } else if( x == Double.NEGATIVE_INFINITY ) {
		return 0.0;
	    }
	}
	return (2.0 * elasticity * r) /((r + 1) * (r + 1));
    }

    /****************************************************************
     *
     * Initialization of the network with random weights.
     *
     ****************************************************************/	
    protected void init(){
	Random random = new Random();

        for( int i = 1; i < numberOfLayers; ++i ) {
            for( int j = 0; j < numberOfNeurons[i]; ++j ) {
                layers[i][j].init(random);
            }
        }
    }
}
