/*****************************************************************************/
/* File:         NeuralController.java                                       */
/*                                                                           */
/* Language:     Blackdown Java 1.2.2 RC4, native threads                    */
/* APIs:         Webots ALife 2.0.3                                          */
/* Description:  Adds the NeurallyConnectable interface to a Controller.     */
/*                                                                           */
/* Author:       Torbjorn S. Dahl, Machine Learning Research Group           */
/*               Department of Computer Science, University of Bristol, UK   */
/*****************************************************************************/

import java.util.Vector;
import java.util.Enumeration;
import uk.ac.bris.cs.tdahl.plancs.NeurallyConnectable;

public abstract class NeuralController 
    extends SynchController
    implements NeurallyConnectable{

    // Instance Variables
    //====================
    
    // Step Counter
    protected int stepc = 0;

    // Inputs
    protected Vector inputs;

    // Outputs
    protected Vector outputs;

    // Class Methods
    //===============

    // Constructor
    //-------------
    public NeuralController(){
	inputs=new Vector();
	outputs=new Vector();
    } // Ends Constructor
    
    // Instance Methods
    //==================

    // init
    //------
    public void init(boolean s){
    } // Ends init

    // Step
    //------
    // overridden (hence public)
    public void step(long ms){
	println("");
	stepc++;
	dprint("Step "+stepc+", "+ms+"\n");
	// Notify all subscribers
	Enumeration outputsen=outputs.elements();
	NeurallyConnectable output=null;
	while(outputsen.hasMoreElements()){
	    output=(NeurallyConnectable)outputsen.nextElement();
	    output.synchNotify();
	}
	// Take intermediate action
	intermediateAction();
	// Wait for notifications from inputs
	int is=inputs.size();
	synchWaitForAllNotifications(is);
	dprintln("All "+is+" inputs received");
	// Activate khepera
	activateKhepera();
    } // Ends Step
    
    // intermediateAction
    protected void intermediateAction(){};
    // Ends intermediateAction
    
    // activateKhepera();
    protected abstract void activateKhepera();
    // Ends activateKhepera

    // addInput
    public void addInput(NeurallyConnectable input){
	inputs.add(input);
	input.addOutput(this);
    } // Ends addInput

    // addOutput
    public void addOutput(NeurallyConnectable output){
	outputs.add(output);
    } // Ends subscribe

} // Ends class NeuralController












