package gridworld;

import rl.*;

public class GridTest {

    public static final int size = 5;

    public static void main(String args[]) {
	GridWorld gw;
	MonteCarlo mc;

	gw = new GridWorld(size);
	mc = new MonteCarlo(gw);
	try {
	    mc.setEpsilon(0.2);
	} catch(InvalidProbabilityException ipe) {
	    System.out.println(ipe);
	}
	//mc.setMaxNumberOfEpisodes(10);
	mc.setEpisodeLength(5);
	showAllInfo(mc);
	for( int i = 0; i < 2000; ++i ) {
	    //System.out.println("------------------------");	    
	    mc.episode();
	    //showAllInfo(mc);
	    String s = new String();
	    for( int j = 0; j < mc.getLastEpisodeStates().length; ++j ) {
		s += "," + mc.getLastEpisodeStates()[j];
	    }	    
	    System.out.println("st:" + s);

	    /*String a = new String();
	    for( int k = 0; k < mc.getLastEpisodeActions().length; ++k ) {
		a += "," + mc.getLastEpisodeActions()[k];
	    }
	    System.out.println("a:" + a);
	    System.out.println("rev:" + mc.getLastEpisodeReward().getReward());*/
	}
	showAllInfo(mc);
    }

    public static void showAllInfo(MonteCarlo mc) {
	// show results
	showActionValueFunction(mc.getActionValueFunction());
	showStateValueFunction(mc.getActionValueFunction());
	showDecisionFunction(mc.getDecisionFunction().getProbabilities());
    }

    public static void showDecisionFunction(Probability[][] probs) {
	String s = new String();
	for(int i = 0; i < size*size; ++i ) {
	    if( i % size == 0 ) {
		s+= "\n";
	    }

	    double p = 0;
	    int pos = -1;
	    for(int j = 0; j < 4; ++j ) {
		if( p < probs[i][j].getProbability() ) {
		    p = probs[i][j].getProbability();
		    pos = j;
		}
	    }
	    if( pos == GridConsts.UP ) {
		s+= "^";
	    } else if( pos == GridConsts.RIGHT ) {
		s+= ">";
	    } else if( pos == GridConsts.DOWN ) {
		s+= ".";
	    } else if( pos == GridConsts.LEFT ) {
		s+= "<";
	    } else {
		s+= "*";
	    }
	}
	System.out.println(s);	
    }

    public static void showActionValueFunction(ActionValueFunction q) {
	String s = new String();
	for(int i = 0; i < size*size; ++i ) {
	    if( i % size == 0 ) {
		s+= "\n";
	    }
	    if( q.argmax(i) == GridConsts.UP ) {
		s+= "^";
	    } else if( q.argmax(i) == GridConsts.RIGHT ) {
		s+= ">";
	    } else if( q.argmax(i) == GridConsts.DOWN ) {
		s+= ".";
	    } else if( q.argmax(i) == GridConsts.LEFT ) {
		s+= "<";
	    } else {
		s+= "*";
	    }
	}
	System.out.println(s);	
    }

    public static void showStateValueFunction(ActionValueFunction q) {
	String s = new String();
	for(int i = 0; i < size*size; ++i ) {
	    if( i % size == 0 ) {
		s+= "\n";
	    }
	    s += q.stateValue(i).getReward() + ",";
	}
	System.out.println(s);	
    }
}
