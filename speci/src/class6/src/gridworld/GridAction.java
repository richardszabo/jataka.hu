package gridworld;

import rl.*;

public class GridAction extends Action {
    protected int type;
    protected boolean isBorder;
    protected static Probability p0;
    protected static Probability p1;
    protected static Reward r0 = new Reward(0.0);
    protected static Reward r1 = new Reward(-1.0);
    protected Probability p;
    protected Reward r;

    static {
	try {
	    p0 = new Probability(0.0);
	    p1 = new Probability(1.0);
	} catch(InvalidProbabilityException ipe) {
	    System.out.println(ipe);
	}
    }

    public GridAction(int type, boolean isBorder) {
	this.type = type;
	this.isBorder = isBorder;	
    }

    protected void calc(State actS,State nextS) {
	if( actS instanceof GridState && nextS instanceof GridState ) {
	    GridState act = (GridState)actS;
	    GridState next = (GridState)nextS;
	    if( type == GridConsts.UP && !isBorder && act.getX() - 1 == next.getX() && 
		                            act.getY() == next.getY() ) {
		p = p1;
		r = r0;
	    } else if( type == GridConsts.UP && isBorder && act.getX() == next.getX() && 
		                           act.getY() == next.getY() ) {
		p = p1;
		r = r1;
	    } else if( type == GridConsts.RIGHT && !isBorder && act.getX() == next.getX() && 
		                            act.getY() + 1== next.getY() ) {
		p = p1;
		r = r0;
	    } else if( type == GridConsts.RIGHT && isBorder && act.getX() == next.getX() && 
		                           act.getY() == next.getY() ) {
		p = p1;
		r = r1;
	    } else if( type == GridConsts.DOWN && !isBorder && act.getX() + 1 == next.getX() && 
		                            act.getY() == next.getY() ) {
		p = p1;
		r = r0;
	    } else if( type == GridConsts.DOWN && isBorder && act.getX() == next.getX() && 
		                           act.getY() == next.getY() ) {
		p = p1;
		r = r1;
	    } else if( type == GridConsts.LEFT && !isBorder && act.getX() == next.getX() && 
		                            act.getY() - 1== next.getY() ) {
		p = p1;
		r = r0;
	    } else if( type == GridConsts.LEFT && isBorder && act.getX() == next.getX() && 
		                           act.getY() == next.getY() ) {
		p = p1;
		r = r1;
	    } else {
		p = p0;
		r = r0;
	    }
	} else {
	    throw new ClassCastException();
	}
    }

    public Probability a(State actS,State nextS) {
	calc(actS,nextS);
	return p;
    }

    public Reward r(State actS,State nextS) {
	calc(actS,nextS);
	return r;
    }
}
