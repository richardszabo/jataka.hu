package gridworld;

import rl.*;

public class SpecGridAction extends GridAction {
    Reward specReward;
    GridState nextState;
    
    public SpecGridAction(int type, boolean isBorder,GridState nextState, double reward) {
	super(type,isBorder);
	this.nextState = nextState;
	specReward = new Reward(reward);
    }
    
    protected void calc(State actS,State nextS) {
	// new setting 
	if( actS instanceof GridState && nextS instanceof GridState ) {
	    GridState act = (GridState)actS;
	    GridState next = (GridState)nextS;
	    if( nextState == nextS ) {
		p = p1;
		r = specReward;
	    } else {
		p = p0;
		r = r0;
	    }
	} else {
	    throw new ClassCastException();
	}
    }
}
