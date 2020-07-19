package BackPropagation;

/********************************************************************
 *
 * Author: Richard Szabo
 * Last modification: 01/Jun/2000
 *
 ********************************************************************/

import java.util.Random;
import java.io.File;
import java.io.RandomAccessFile;
import java.io.IOException;

class Neuron {

    protected double output;
    protected double state;
    protected double weights[];
    protected double weightsChange[];
    protected int numberOfInputs;
    protected double input[];
    protected BackPropagation parent;

    /* constructor of a Neuron */
    protected Neuron(BackPropagation parent,int numberOfInputs) {
        this.parent = parent;
        this.numberOfInputs = numberOfInputs;
        weights = new double[numberOfInputs + 1];
        weightsChange = new double[numberOfInputs + 1];
        input = new double[numberOfInputs];
    }

    /* random weight values in the (-0.1,0.1) interval */
    protected void init(Random random) {
        for( int i = 0; i < numberOfInputs + 1; ++i ) {
	    weightsChange[i] = 0.0;
            weights[i] = 0.2 * (random.nextDouble() - 0.5);
        }		
    }

    /* alter weight values with a Gaussian */
    protected void alter(Random random) {
        for( int i = 0; i < numberOfInputs + 1; ++i ) {
            weights[i] += random.nextGaussian();
        }			
    }

    /* iteration of the neuron, forward phase */
    protected double propagate(double input[]) {
        state = 0.0;
	/* calculation of the inner product of the input with the weights */
  	for(int i = 0; i < numberOfInputs; ++i) {
            this.input[i] = input[i];
            state += weights[i] * input[i];
  	}		
        state += weights[numberOfInputs];
	/* using the sigmoid function */
        output = parent.sigmoid(state);
        return output;
    }

    /* teaching of the neuron, backward phase */
    protected void learn(double error) {
	/* modification of weights according to the error terms,
	   using the previous modifications as a momentum and 
           saving the actual modification for the next step */
       	for( int i = 0; i < numberOfInputs; ++i) {
	    weights[i] += parent.momentum * weightsChange[i];
	    weightsChange[i] = error * input[i] * parent.learningRate;
	    weights[i] += weightsChange[i];
	}
	weights[numberOfInputs] += parent.momentum * weightsChange[numberOfInputs];
	weightsChange[numberOfInputs] = error * parent.learningRate;
	weights[numberOfInputs] += weightsChange[numberOfInputs];
    }

    /* saving a Neuron in an UTF file */
    protected void writeUTF(RandomAccessFile file) throws IOException {
       	for( int i = 0; i < numberOfInputs + 1; ++i) {
	    file.writeDouble(weights[i]);
	}
    }

    /* loading a Neuron from an UTF file */
    protected void readUTF(RandomAccessFile file) throws IOException {
       	for( int i = 0; i < numberOfInputs + 1; ++i) {
	    weights[i] = file.readDouble();
	}
    }
}









