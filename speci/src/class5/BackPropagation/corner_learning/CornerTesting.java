
import java.io.File;

import com.sun.image.codec.jpeg.*;  // for jpeg format handling

import backpropagation.*; // for testing the learned network

public class CornerTesting {

    // the trained network
    static Backpropagation bpn = null;

    public static void main(String args[]) {

	if( args.length != 2 ) { // argument checking
	    System.out.println("Usage: CornerTesting <network file from the same directory> <jpeg file>");
	    return;
	}

	// getting and checking the network file name
	String networkStr = args[0];
	File networkFile= new File(networkStr);

	if( networkFile == null && !networkFile.exists() || !networkFile.isFile() ) {
	    System.out.println("Invalid network filename.");
	    return;
	}	  

	String jpegStr = args[1];
	File jpegFile = new File(jpegStr);
	if( jpegFile == null && !jpegFile.exists() || !jpegFile.isFile() ) {
	    System.out.println("Invalid jpeg filename.");
	    return;
	}	  

	// creating the backprop network from trained network file
	bpn = Backpropagation.loadNeuro(".",networkStr);
	if( bpn == null ) {
	    System.out.println("Invalid network filename:" + networkStr);
	    return;
	}

	double input[] = CornerLearning.getInput(jpegStr);
	bpn.propagate(input);
	System.out.println("output - "+ 
			   CornerLearning.upperLeftDir + ":" + bpn.output(0) + ", " + 
			   CornerLearning.upperRightDir + ":" + bpn.output(1) + "," + 
			   CornerLearning.lowerLeftDir + ":" + bpn.output(2) + "," + 
			   CornerLearning.lowerRightDir + ":" + bpn.output(3));  
	// the network output size should be calculated here 
	// instead of the prewired four output neurons
	
    }
}
