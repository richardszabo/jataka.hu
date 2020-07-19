"use strict";

Foods.prototype = Object.create(Particle.prototype);
Foods.prototype.constructor = Foods;

function Foods (antropy) {
    this.antropy = antropy;
    this.foodSource = Math.max(this.antropy.foodSource,1);
    this.foodNumber = this.antropy.foodNumber;
    this.foodDeviation = 5;
    this.food = [];
    this.pos2D = [];
    for( var i = 0; i < this.foodSource; ++i ) {
        this.pos2D[i] = new Point(Math.floor(Math.random() * AntSpace.spaceSize),Math.floor(Math.random() * AntSpace.spaceSize));
        for(var j = 0; j < this.foodNumber/this.foodSource; ++j ) {
            var point = get2DGaussian(this.pos2D[i],this.foodDeviation);
            this.food[i*Math.floor(this.foodNumber/this.foodSource) + j] = new Food();
            this.food[i*Math.floor(this.foodNumber/this.foodSource) + j].pos2D = new Point(AntSpace.crop2Space(Math.floor(point.x)),AntSpace.crop2Space(Math.floor(point.y)));
        }
    }
    for(var j = 0; j < this.foodNumber - this.foodSource * Math.floor(this.foodNumber/this.foodSource); ++j ) {
        var point = get2DGaussian(this.pos2D[0],this.foodDeviation);
        this.food[this.foodSource * Math.floor(this.foodNumber/this.foodSource) + j] = new Food();
        this.food[this.foodSource * Math.floor(this.foodNumber/this.foodSource) + j].pos2D = new Point(AntSpace.crop2Space(Math.floor(point.x)),AntSpace.crop2Space(Math.floor(point.y)));
    }
}

Foods.prototype = {
    get maxFood() {
        return this.foodNumber;
    },
    set maxFood(fn) {
        this.foodNumber = fn;
    }
};

Foods.foodSize = 2;

Foods.prototype.getFoodAt = function(pos) {
    var found = 0;
    for(var i = 0; i < this.foodNumber && !found; ++i ) {
	if( !this.food[i].consumed && this.food[i].x === pos.x && this.food[i].y === pos.y ) {
	    found = i + 1;
	    this.food[i].consumed = true;
	}
    }
    return found;
}

Foods.prototype.setFoodPos = function(ii,pos) {
    if( ii > 0 && ii <= this.foodNumber ) {
	this.food[ii-1].pos2D = pos;
    }
}

Foods.prototype.draw = function(ctx) {
    ctx.fillStyle = "green";
    for(var i = 0; i < this.foodNumber; ++i ) {
	this.food[i].draw(ctx,Foods.foodSize);
    }
}

Food.prototype = Object.create(Particle.prototype);
Food.prototype.constructor = Food;

function Food () {
    this.x = 0;
    this.y = 0;
    this.consumed = false;
}
