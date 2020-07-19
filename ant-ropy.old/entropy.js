/*
  entropy calculation based on ants article
*/
function getDistance(a,b) {
    var d = Math.abs(a - b);
    var w = AntSpace.spaceSize;
    if (d > w/2) {
        d = w - d;
    }
    return d;
}

function getDistance2(f,g) {
    var dX = getDistance(f.x, g.x);
    var dY = getDistance(f.y, g.y);
    return Math.sqrt(dX*dX + dY*dY);
}

// calculates average distance among particles (ants, food)
function getEntropy(particles) {
    var size = particles.length;
    if( size < 2 ) {
        return 1.0;
    }

    var sum = 0;
    for (var i = 0; i < size; i++) {
        var f = particles[i];
        for (var j = i + 1; j < size; j++) {
            var g = particles[j];
            sum += getDistance2(f,g);
        }
    }
    return sum /(size*(size-1)/2);
}
