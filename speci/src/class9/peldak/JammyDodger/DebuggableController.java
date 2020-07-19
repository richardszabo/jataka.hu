/*****************************************************************************/
/* File:         DebuggableController.java                                   */
/*                                                                           */
/* Language:     Blackdown Java 1.2.2 RC4, native threads                    */
/* APIs:         Webots ALife 2.0.3                                          */
/* Description:  A general Controller superclass implementing the debuggable */
/*               interface.                                                  */
/*                                                                           */
/* Author:       Torbjorn S. Dahl, Machine Learning Research Group           */
/*               Department of Computer Science, University of Bristol, UK   */
/*****************************************************************************/

import KheperaController;
import uk.ac.bris.cs.tdahl.plancs.Debuggable;

public class DebuggableController 
    extends KheperaController 
    implements Debuggable {
    
    private boolean debugging=false;

    public void debuggingOn(){
	debugging=true;
    }

    public void debuggingOff(){
	debugging=false;
    }

    public void print(String s){
	if(debugging){
	    System.out.print(s);
	    System.out.flush();
	}
    }

    public void println(String s){
	if(debugging){
	    System.out.print(s+"\n");
	    System.out.flush();
	}
    }

    public void dprint(String s){
	if(debugging){
	    System.out.print(toString()+": "+s);
	    System.out.flush();
	}
    }

    public void dprintln(String s){
	if(debugging){
	    System.out.print(toString()+": "+s+"\n");
	    System.out.flush();
	}
    }

} // Ends public class DebuggableController



