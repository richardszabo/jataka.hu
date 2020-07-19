// -----------------------------------------------------------------------------------------------------
// auxiliary functions
define(function () {
        return function() {
            // http://math.stackexchange.com/questions/484395/how-to-generate-a-cauchy-random-variable
            return Math.tan(Math.PI*(Math.random()-1.0/2));
        }
});
