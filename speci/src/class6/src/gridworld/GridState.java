package gridworld;

import rl.State;

public class GridState extends State {
    int gridSize; // size of the grid
    int x,y; // positions in the grid

    public GridState(int gridSize, int x, int y) {
	this.gridSize = gridSize;
	this.x = x;
	this.y = y;	
	actions = new GridAction[4];
	actions[GridConsts.UP] = new GridAction(GridConsts.UP,x == 0);
	actions[GridConsts.RIGHT] = new GridAction(GridConsts.RIGHT, y == gridSize - 1);
	actions[GridConsts.DOWN] = new GridAction(GridConsts.DOWN, x == gridSize - 1);
	actions[GridConsts.LEFT] = new GridAction(GridConsts.LEFT, y == 0);
    }

    public int getX() {
	return x;
    }

    public int getY() {
	return y;
    }

    public void setX(int x) {
	this.x = x;
    }

    public void setY(int y) {
	this.y = y;
    }
}
