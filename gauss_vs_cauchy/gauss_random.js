// -----------------------------------------------------------------------------------------------------
// auxiliary functions
define(function () {
    //
    // quasi-normal distribution standard form: [-1,1)
    return function() {
        // http://www.protonfish.com/random.shtml
        return (Math.random()*2-1)+(Math.random()*2-1)+(Math.random()*2-1);
        // formerly it was the function below but that was not close to standard, there was no value above 1, below -1
        // http://stackoverflow.com/a/20161247/21047 and http://jsfiddle.net/Guffa/tvt5K/
        //return ((Math.random() + Math.random() + Math.random() + Math.random() + Math.random() + Math.random()-3)) / 3;
    }
});
