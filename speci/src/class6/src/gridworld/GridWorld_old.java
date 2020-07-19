package gridworld;

import rl.*;

public class GridWorld extends TaskSpecification {

    public GridWorld(int size) {
	super();
	states = new GridState[size*size];
	for(int i = 0; i < size*size; ++i ) {
	    states[i] = new GridState(size,i/size,i%size);
	}
        int spec = 16;
	int goal = 3;
	states[spec] = new SpecGridState(size,spec/size,spec%size,(GridState)states[goal],40.0);
        spec = 19;
	goal = 8;
	states[spec] = new SpecGridState(size,spec/size,spec%size,(GridState)states[goal],20.0);
    }
}
