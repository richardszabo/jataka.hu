package gridworld;

public class SpecGridState extends GridState {
    
    public SpecGridState(int gridSize, int x, int y,
			 GridState nextState, double reward) {
	super(gridSize,x,y);
	
	actions[GridConsts.UP] = new SpecGridAction(GridConsts.UP,true,nextState,reward);	
	actions[GridConsts.RIGHT] = new SpecGridAction(GridConsts.RIGHT,true,nextState,reward);	
	actions[GridConsts.DOWN] = new SpecGridAction(GridConsts.DOWN,true,nextState,reward);	
	actions[GridConsts.LEFT] = new SpecGridAction(GridConsts.LEFT,true,nextState,reward);	
    }
}
