
import java.util.Random;
import java.util.GregorianCalendar;
import java.io.File;
import java.io.IOException;
import java.io.FileNotFoundException;

import java.io.FileInputStream;
import java.awt.image.BufferedImage;
import java.awt.image.SampleModel;
import java.awt.image.ColorModel;

import com.sun.image.codec.jpeg.*;  // for jpeg format handling

import backpropagation.*; // for learning

public class CornerLearning {

    // picture subdirectory names
    final static int upperLeft = 0;
    final static int upperRight = 1;
    final static int lowerLeft = 2;
    final static int lowerRight = 3;

    final static String upperLeftDir  = "ul";
    final static String upperRightDir = "ur";
    final static String lowerLeftDir  = "ll";
    final static String lowerRightDir = "lr";

    // the network outputs for the cornered subdirectories
    final static double directionOutput[][] = {{ 1.0,-1.0,-1.0,-1.0},
					       {-1.0, 1.0,-1.0,-1.0},
					       {-1.0,-1.0, 1.0,-1.0},
					       {-1.0,-1.0,-1.0, 1.0}};

    //final static long numberOfTrainings = 2000; // for 5x5 image
    //final static long numberOfTrainings = 4000; // for 10x10 image
    //final static long numberOfTrainings = 10000;  // for 50x50 image
    //final static long numberOfTrainings = 100;  // for 256x256 image
    final static long numberOfTrainings = 1000;  // for faces
    final static long numberOfTests = 10;

    // the trained network
    static Backpropagation bpn = null;
    // filename
    final static String networkFile = "corner_learning.utf";

    // photo file names
    static String pictures[][];

    static Random r = null;

    public static void main(String args[]) {

	int i,j;
     
	if( args.length != 2 ) { // argument checking
	    System.out.println("Usage: CornerLearning <network specification file> <jpeg directory>");
	    System.out.println("The directory should contain .jpg files in four subdirectory:");
	    System.out.println(upperLeftDir + ", " + 
			       upperRightDir + ", " + 
			       lowerLeftDir + ", " + 
			       lowerRightDir);
	    return;
	}

	// getting and checking the network file name
	String networkStr = args[0];
	File networkStructure = new File(networkStr);

	if( networkStructure == null && !networkStructure.exists() || !networkStructure.isFile() ) {
	    System.out.println("Invalid network filename.");
	    return;
	}	  

	// creating the backprop network from specification file
	try {
	    bpn = Backpropagation.loadStructure(networkStr);
	} catch(FileNotFoundException fnfe) {
	    System.out.println("Invalid network filename:" + fnfe);
	    return;
	} catch(IOException ioe) {
	    System.out.println("Invalid network file:" + ioe);
	    return;
	}

	// getting and checking the directory
	String dirStr = args[1];
	File dir = new File(dirStr);
	
	if( dir == null && !dir.exists() || !dir.isDirectory() ) {
	    System.out.println("Invalid picture directory name.");
	    return;
	}	  

	// reading four subdirectories
	String filedirs[] = dir.list();
	if( filedirs == null ) {
	    System.out.println("Invalid directory or I/O error.");
	    return;
	}

	// header information
	System.out.println("network:" + networkStr);
	System.out.println("directory:" + dirStr);
	System.out.println("starting date&time:" + new GregorianCalendar().getTime());

	// creating the subdirectory lists in a 4D array 
	pictures = new String[4][];
	
	for( i = 0; i < filedirs.length; ++i ) {
	    if( upperLeftDir.equals(filedirs[i]) ) {
		File f = new File(dirStr + "/" + filedirs[i]);
		if( f!= null && f.exists() && f.isDirectory() ) {			    
		    pictures[upperLeft] = f.list();  // getting the filelist in the subdirectorry
		    for( j = 0; j < pictures[upperLeft].length; ++j ) { // creating full pathes
			pictures[upperLeft][j] = dirStr + "/" + filedirs[i] + "/" + pictures[upperLeft][j];
		    }
		}
	    }
	    if( upperRightDir.equals(filedirs[i]) ) {
		File f = new File(dirStr + "/" + filedirs[i]);
		if( f!= null && f.exists() && f.isDirectory() ) {		
		    pictures[upperRight] = f.list(); // getting the filelist in the subdirectorry
		    for( j = 0; j < pictures[upperRight].length; ++j ) { // creating full pathes
			pictures[upperRight][j] = dirStr + "/" + filedirs[i] + "/" +  pictures[upperRight][j];
		    }
		}
	    }
	    if( lowerLeftDir.equals(filedirs[i]) ) {
		File f = new File(dirStr + "/" + filedirs[i]);
		if( f!= null && f.exists() && f.isDirectory() ) {		
		    pictures[lowerLeft] = f.list(); // getting the filelist in the subdirectorry
		    for( j = 0; j < pictures[lowerLeft].length; ++j ) { // creating full pathes
			pictures[lowerLeft][j] = dirStr + "/" + filedirs[i] + "/" + pictures[lowerLeft][j];
		    }
		}
	    }
	    if( lowerRightDir.equals(filedirs[i]) ) {
		File f = new File(dirStr + "/" + filedirs[i]);
		if( f!= null && f.exists() && f.isDirectory() ) {		
		    pictures[lowerRight] = f.list(); // getting the filelist in the subdirectorry
		    for( j = 0; j < pictures[lowerRight].length; ++j ) { // creating full pathes
			pictures[lowerRight][j] = dirStr + "/" + filedirs[i] + "/" + pictures[lowerRight][j];
		    }
		}
	    }
	}

	if( pictures[upperLeft] == null ||
	    pictures[upperRight] == null ||
	    pictures[lowerLeft] == null ||
	    pictures[lowerRight] == null ) {
	    System.out.println("At least one of the four subdirectories is missing.");
	    return;
	}	

        r = new Random();


	System.out.println("train");
	// training with one file
	for( i = 0; i < numberOfTrainings; ++i ) {
	    System.out.println("train:" + i);
	    train();
	    //test();
	}

	System.out.println("test");
	// test 
	for( i = 0; i < numberOfTests; ++i ) {
	    test();
	}
    
	// saving the network structure specification
	try {
	    bpn.saveStructure("res.bps");
	    System.out.println(bpn.absoluteError);
	} catch(IOException ioe) {
	    System.out.println(ioe);
	}

	// saving the trained network
	bpn.saveNeuro(".",networkFile);
	System.out.println("finishing date&time:" + new GregorianCalendar().getTime());
    }

    private static void train() {
	int d = r.nextInt(4);
	int which = r.nextInt(pictures[d].length);
	double input[] = getInput(pictures[d][which]);
	// training
	bpn.learn(input,directionOutput[d]);
    }

    private static void test() {
	int d = r.nextInt(4);
	int which = r.nextInt(pictures[d].length);
	double input[] = getInput(pictures[d][which]);
	// training
	bpn.propagate(input);
	System.out.println("out:"+ 
			   bpn.output(0) + "," + 
			   bpn.output(1) + "," + 
			   bpn.output(2) + "," + 
			   bpn.output(3));
	System.out.println("real:" + 
			   directionOutput[d][0] + "," +
			   directionOutput[d][1] + "," +
			   directionOutput[d][2] + "," +
			   directionOutput[d][3]);
    }

    /**
     * Creates network input from picture file.
     */
    protected static double[] getInput(String filename) {

	//System.out.println("Getting " + filename);
	int i,j,k;

	try {
	    // opening the jpeg file
	    FileInputStream fis = new FileInputStream(filename);
	    // getting the jpeg producer
	    JPEGImageDecoder jid = JPEGCodec.createJPEGDecoder(fis);   
	    // getting jpeg data
	    BufferedImage bi = jid.decodeAsBufferedImage(); // getting jpeg data
	    SampleModel sm = bi.getSampleModel();
	    ColorModel cm = bi.getColorModel();

	    // getting the number of bands
	    int bands = sm.getNumBands();

	    // getting the samples by band
	    int samples[][] = new int[bands][];
	    for( i = 0; i < bands; ++i ){
		samples[i] = sm.getSamples(0,0,bi.getWidth(),bi.getHeight(),i,(int [])null,bi.getRaster().getDataBuffer());
	    }

	    // creating the input vector from the bytes with averaging pixel bands
	    double input[] = new double[bi.getWidth() * bi.getHeight()];
	    for( i = 0; i < input.length; ++i ){
		k = 0;
		for( j = 0; j < bands; ++j ){
		    k += samples[j][i];
		}		
		input[i] = 1.0 - (double)k/bands / 256.0;  // to have the inputs between 0 and 1
		//System.out.print(input[i] + ",");
	    }
	    //System.out.println("");

	    // closing the file
	    fis.close();

	    return input;

	} catch( FileNotFoundException fnfe ) {
	    System.out.println(fnfe);
	} catch( IOException ioe ) {
	    System.out.println(ioe);	    
	}

	return null;
    }
}
