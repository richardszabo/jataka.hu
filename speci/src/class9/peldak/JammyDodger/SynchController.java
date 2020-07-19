/*****************************************************************************/
/* File:         SynchController.java                                        */
/*                                                                           */
/* Language:     Blackdown Java 1.2.2 RC4, native threads                    */
/* APIs:         Webots ALife 2.0.3                                          */
/* Description:  Provides synchronized wait and notify methods for thread    */
/*               synchronization                                             */
/*                                                                           */
/* Author:       Torbjorn S. Dahl, Machine Learning Research Group           */
/*               Department of Computer Science, University of Bristol, UK   */
/*****************************************************************************/

import uk.ac.bris.cs.tdahl.plancs.Synchronizable;

public class SynchController 
    extends DebuggableController
    implements Synchronizable{

    // Instance Variables
    //====================
    protected int notifc=0;

    // Instance Methods
    //==================
    
    // synchWaitForAllNotifications
    public synchronized void synchWaitForAllNotifications(int notifs){
	while(notifc<notifs){
	    //dprint("Calling wait\n");
	    try{
		wait();
	    }catch(InterruptedException e){
	    }
	}
	//dprint("All "+notifs+" subscriptions in\n");
	notifc=0;
	//dprint("Set notification count to "+notifc+"\n");
    } // Ends synchWaitForAllNotifications
    
    public synchronized void synchNotify(){
	//dprint("Calling notify\n");
	notifc++;
	//dprint("Notification count: "+notifc+"\n");
	try{
	    notify();
	}catch(IllegalMonitorStateException e){
	}
	//dprint("Called notify\n");
    }

} // Ends class SynchController

