package rl;

public abstract class State {
    protected Action actions[];    

    /**
     * Returns the possible actions in the state.
     */
    public Action[] getActions() {
	return actions;
    }
}
