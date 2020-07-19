"use strict";

function Ants (antropy) {
    this.antropy = antropy;
    this.ant = [];
    this.carryingFood = 0;
    this.antNumber = this.antropy.antNumber;
    for(var i = 0; i < this.antNumber; ++i ) {
	     this.ant[i] = new Ant(this.antropy,i);
    }
    this.selected_ant_id = 0;
 }

Ants.ANT_SIZE = 3;
// neighbours (order is important!)
Ants.NEIGHBOURS = [[0,-1],[-1,-1],[-1,0],[-1,1],[0,1],[1,1],[1,0],[1,-1]];
Ants.NO_HEADINGS = Ants.NEIGHBOURS.length;
// number of possible directions to move
Ants.STEPS_AHEAD = 3;
Ants.STARTING_PHEROMONE = 100;
Ants.PHEROMONE_DECREASE = 2;

Ants.MODE_SEARCH = 1;
Ants.MODE_HOME = 2;

Ants.MOVE_RANDOM = 0.1; /* random moving probability */
Ants.ATTRACTION_LEVEL = 1;    /* pheromone level to change to attracted mode */

Ants.prototype.draw = function(ctx) {
    ctx.fillStyle = "red";
    ctx.strokeStyle = "red";
    // wondering ants with red border
    for(var i = 0; i < this.antNumber; ++i ) {
        if( this.selected_ant_id !== i && this.ant[i].carriedFood === null ) {
            this.ant[i].draw(ctx);
        }
    }
    ctx.strokeStyle = "green";
    // carrying ants with green border
    for(var i = 0; i < this.antNumber; ++i ) {
        if( this.selected_ant_id !== i && this.ant[i].carriedFood !== null ) {
            this.ant[i].draw(ctx);
        }
    }
    // selected ant in yellow
    if( this.selected_ant_id !== null ) {
        ctx.fillStyle = "yellow";
        this.ant[this.selected_ant_id].draw(ctx);
        if( this.ant[this.selected_ant_id].carriedFood === null ) {
            ctx.strokeStyle = "red";
        }
    }
}

Ants.prototype.step = function() {
    this.carryingFood = 0;
    for(var i = 0; i < this.antNumber; ++i ) {
	     this.ant[i].step();
    }
}

Ants.prototype.getAntAt = function(pos) {
    var found = null;
    for(var i = 0; i < this.antNumber && !found; ++i ) {
        if( this.ant[i].x === pos.x && this.ant[i].y === pos.y ) {
	       found = i;
	    }
    }
    return found;
}

Ants.prototype.selectAnt = function(point) {
    this.selected_ant_id = antropy.ants.getAntAt(point);
}

Ant.prototype = Object.create(Particle.prototype);
Ant.prototype.constructor = Ant;

function Ant(antropy,id) {
    this.id = id;
    this.antropy = antropy;
    this.x = AntSpace.center().x; //Math.floor(Math.random() * AntSpace.spaceSize);
    this.y = AntSpace.center().y; //Math.floor(Math.random() * AntSpace.spaceSize);
    this.heading = Math.floor(Math.random() * Ants.NEIGHBOURS.length);
    this.pheromoneHive = 0;
    this.pheromoneFood = 0;
    this.carriedFood = null; // id of the food [1,foodNumber]
    this.mode = Ants.MODE_SEARCH;
}

Ant.prototype.draw = function(ctx) {
    Particle.prototype.draw.call(this,ctx,Ants.ANT_SIZE);
    var canvaspoint = this.canvasPos2D;
    ctx.beginPath();
    ctx.moveTo(canvaspoint.x,canvaspoint.y);
    ctx.lineTo(canvaspoint.x+5*Ants.NEIGHBOURS[this.heading][0],
	       canvaspoint.y+5*Ants.NEIGHBOURS[this.heading][1]);
    ctx.stroke();
    if( this.carriedFood ) {
        ctx.beginPath();
        ctx.arc(canvaspoint.x,canvaspoint.y,Ants.ANT_SIZE,0,2*Math.PI);
        ctx.stroke();
    }
}

Ant.prototype.step = function() {
    if( this.foodCheck() ) {
	       return;
    }
    this.hiveCheck();
    if( Math.random() > Ants.MOVE_RANDOM ) {
	       this.maxMove();
    } else {
	       this.randomWalkMode();
    }
    if( this.carriedFood ) {
	       this.antropy.foods.setFoodPos(this.carriedFood,this.pos2D);
	          ++this.antropy.ants.carryingFood;
    }
}

Ant.prototype.randomWalkMode = function() {
    var dir = gauss_random();
    var change = sign(dir)*Math.min(Math.floor(Math.abs(dir)),Math.floor(Ants.STEPS_AHEAD/2));
    this.heading = Math.floor(this.heading + change + Ants.NO_HEADINGS) % Ants.NO_HEADINGS;
    this.x += Ants.NEIGHBOURS[this.heading][0];
    this.y += Ants.NEIGHBOURS[this.heading][1];
}

Ant.prototype.maxMove = function() {
    var max = 0;
    var maxX = this.x;
    var maxY = this.y;
    var ph = 0;
    var pos;

    pos = [];
    for( var i = -Math.floor(Ants.STEPS_AHEAD/2); i <= Math.floor(Ants.STEPS_AHEAD/2); ++i ) {
        var px = this.x+Ants.NEIGHBOURS[Math.floor((this.heading + i + Ants.NO_HEADINGS) % Ants.NO_HEADINGS)][0];
	    px = AntSpace.crop2Space(px);
        var py = this.y+Ants.NEIGHBOURS[Math.floor((this.heading + i + Ants.NO_HEADINGS) % Ants.NO_HEADINGS)][1];
	    py = AntSpace.crop2Space(py);
	if( this.mode === Ants.MODE_SEARCH ) {
	    ph = this.antropy.pheromone.getFoodAt(px,py);
	} else if ( this.mode === Ants.MODE_HOME ) {
	    ph = this.antropy.pheromone.getHiveAt(px,py);
	}
	// the pheromone concentration is high enough?
	if( ph > max ) {
	    max = ph;
	    maxX = px;
	    maxY = py;
	    pos = [];
	}
	// collecting info of the maximal values
	if( ph >= max ) {
	    pos[pos.length] = i;
	    pos[pos.length] = px;
	    pos[pos.length] = py;
	    if( i === 0 ) {
		// doubling ahead probability
		pos[pos.length] = i;
		pos[pos.length] = px;
		pos[pos.length] = py;
	    }
	}
    }

    if( max > Ants.ATTRACTION_LEVEL ) {
        var dir = Math.floor(Math.random() * pos.length / 3);
        this.x = pos[3*dir+1];
	    this.y = pos[3*dir+2];
        this.heading = Math.floor((this.heading + pos[3*dir] + Ants.NO_HEADINGS) % Ants.NO_HEADINGS);
    } else {
	    // sometimes no pheromone found, so only random walk is possible
	    this.randomWalkMode();
    }
}

Ant.prototype.foodCheck = function() {
    if( this.carriedFood ) {
	return false;
    }
    var food = this.antropy.foods.getFoodAt(this.pos2D);
    if( food ) {
	this.carriedFood = food;
	this.mode = Ants.MODE_HOME;
	this.pheromoneFood = Ants.STARTING_PHEROMONE;
	return true;
    }
    return false;
}

Ant.prototype.hiveCheck = function() {
   if( this.antropy.hive.isIn(this.pos2D) ) {
       this.pheromoneHive = Ants.STARTING_PHEROMONE;
       if( this.carriedFood ) {
	   this.antropy.hive.stock();
	   this.carriedFood = null;
	   this.mode = Ants.MODE_SEARCH;
	   return true;
       }
    }
    return false;
}

Ant.prototype.getEmittedPheromoneHive = function() {
    var ph = this.pheromoneHive;
    this.pheromoneHive = Math.max(0,this.pheromoneHive - Ants.PHEROMONE_DECREASE);
    return ph;
}

Ant.prototype.getEmittedPheromoneFood = function() {
    if( this.carriedFood ) {
	var pf = this.pheromoneFood;
	this.pheromoneFood = Math.max(0,this.pheromoneFood - Ants.PHEROMONE_DECREASE);
	return pf;
    }
    return 0;
}
