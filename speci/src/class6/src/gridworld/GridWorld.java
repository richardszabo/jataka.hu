package gridworld;

import rl.*;

public class GridWorld extends TaskSpecification {

    public GridWorld(int size) {
	super();
	states = new GridState[size*size];
	for(int i = 0; i < size*size; ++i ) {
	    states[i] = new GridState(size,i/size,i%size);
	}
        int spec = 1;
	int goal = 21;
	states[spec] = new SpecGridState(size,spec/size,spec%size,(GridState)states[goal],10.0);
        spec = 3;
	goal = 13;
	states[spec] = new SpecGridState(size,spec/size,spec%size,(GridState)states[goal],5.0);
    }
}
