package rl;

import java.util.Vector;
import java.util.Random;

public class ActionValueFunction {
    private State states[];
    private Returns rewards[][];  // set of rewards for states and actions 
    private Random random;

    public ActionValueFunction(State s[]) {
	states = s;
	
	random = new Random();
	//Reward r0 = new Reward(0.0);
	rewards = new Returns[s.length][];
	for( int i=0; i < s.length; ++i ) {
	    int alen = s[i].getActions().length;
	    rewards[i] = new Returns[alen];
	    for( int j=0; j < alen; ++j ) {
		rewards[i][j] = new Returns();
	    }
	}
    }

    /**
     * Returns one estimated reward of the action-value function.
     */
    public Reward getReward(int actState, int action) {
	return rewards[actState][action].getAverage();
    }

    /**
     * Adds one estimated reward of the action-value function.
     */
    public void addReward(int actState, int action, Reward r) {
	rewards[actState][action].addReward(r);
    }

    /**
     * Returns the index of the action with the highest estimated reward.
     */
    public int argmax(int actState) {
	int maxp;
	Reward max;

	maxp = 0;
	max = rewards[actState][0].getAverage();	
	//String s = "avf:" + actState + ":";
	//s += max.getReward() + ",";
	for( int i = 1; i < rewards[actState].length; ++i ) {
	    Reward act = rewards[actState][i].getAverage();
	    //s += act.getReward() + ",";
	    if( max.compareTo(act) < 0 || 
		   max.compareTo(act) == 0 && random.nextDouble() > 0.5 ) {
		maxp = i;
		max = act;
	    }	    
	}

	//System.out.println(s);

	return maxp;
    }

    /**
     * Returns the value given by the state value function.
     */
    public Reward stateValue(int actState) {
	int maxp = this.argmax(actState);
	return rewards[actState][maxp].getAverage();		
    }
}

class Returns {
    Vector v;
    Reward average;
    
    Returns() {
	v = new Vector();
	average = new Reward(0.0);
    }

    void addReward(Reward r) {
	v.addElement(r);
	// recalculation of the average
	average.setReward(((v.size() - 1) * average.getReward() + r.getReward()) 
			   / v.size());
    }

    Reward getAverage() {
	return average;
    }
}
