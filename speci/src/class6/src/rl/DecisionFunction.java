package rl;

import java.util.Random;

//pi
public class DecisionFunction {
    private State states[];
    private Probability probs[][];
    private Random random;
    private Probability epsilon;

    public DecisionFunction(State s[]) {
	states = s;
	random = new Random();

	try {
	    epsilon = new Probability(0.0);
	    probs = new Probability[s.length][];
	    for( int i=0; i < s.length; ++i ) {
		int alen = s[i].getActions().length;
		probs[i] = new Probability[alen];
		for( int j=0; j < alen; ++j ) {
		    Probability p = new Probability(1.0/alen);
		    probs[i][j] = p;  // sum of probs must be 1
		}
	    }
	} catch(InvalidProbabilityException ipe) {
	    System.out.println("Misconfiguration:" + ipe);
	}
    }

    /**
     * Returns the next action according to the non-deterministic decision function.
     */
    public int getNextAction(int actState) {
	double bet = random.nextDouble();
	double d;
	int i;
	
	for( i = 0, d = probs[actState][0].getProbability(); 
	     i < probs[actState].length - 1 && d < bet; 
	     ++i, d += probs[actState][i].getProbability());
	return i;
    }

    /**
     * Sets the action-selection according to an epsilon-greedy policy.
     */
    public void setNextAction(int actState, int action) {
	/*System.out.println("st:" + actState + "a:" + action);
	String s = new String();
	for( int j = 0; j < probs[actState].length; ++j ) {
	    s += "," + probs[actState][j].getProbability();
	}	    
	System.out.println("before:" + s);*/

	for( int i = 0; i < probs[actState].length; ++i ) {
	    try {
		if( i == action ) {
		    /*  System.out.println("high" + (1.0 - epsilon.getProbability() + 
			epsilon.getProbability()/probs[actState].length));*/
		    probs[actState][i].setProbability(1.0 - epsilon.getProbability() + 
				   epsilon.getProbability()/probs[actState].length);
		} else {
		    //System.out.println("low" + (epsilon.getProbability()/probs[actState].length));
		    probs[actState][i].setProbability(
				   epsilon.getProbability()/probs[actState].length);
		}
	    } catch(InvalidProbabilityException ipe) {
		System.out.println("Misconfiguration:" + ipe);
	    }
	}
	
        /*s = new String();
	for( int j = 0; j < probs[actState].length; ++j ) {
	    s += "," + probs[actState][j].getProbability();
	}	    
	System.out.println("after:" + s);*/
	
    }

    /**
     * Sets the uncertainity of an action selection.
     */
    public void setEpsilon(double d) throws InvalidProbabilityException {
	epsilon.setProbability(d);
    }

    /**
     * Returns the uncertainity of an action selection.
     */
    public double getEpsilon() {
	return epsilon.getProbability();
    }

    /**
     * Returns probabilities.
     */
    public Probability[][] getProbabilities() {
	return probs;
    }
}


