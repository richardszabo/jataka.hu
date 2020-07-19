
"use strict";

function PheromoneMatrix() {
    this.read_matrix = create2DArray(AntSpace.spaceSize);
    this.write_matrix = create2DArray(AntSpace.spaceSize);
}

PheromoneMatrix.prototype.update = function() {
    this.read_matrix = [];
    copy2DArray(this.write_matrix, this.read_matrix);
}

function Pheromone(antropy) {
    this.antropy = antropy;
    this.hive_matrix = new PheromoneMatrix();
    this.food_matrix = new PheromoneMatrix();
    this.evap_rate = 0.01;
    this.diffusion_constant = 0.86;
}

Pheromone.prototype.step = function() {
    this.diffuse(this.hive_matrix);
    this.diffuse(this.food_matrix);
    this.calculate(this.hive_matrix,true);
    this.calculate(this.food_matrix,false);
    this.update();
}

Pheromone.prototype.draw = function(ctx) {
    // http://stackoverflow.com/a/13916313/21047
    for(var i = 0; i < this.hive_matrix.read_matrix.length; ++i ) {
	for(var j = 0; j < this.hive_matrix.read_matrix[i].length; ++j ) {
	    var hivefillValue = Math.min(Math.round(this.hive_matrix.read_matrix[i][j] || 0),255); // for overflow
	    var foodfillValue = Math.min(Math.round(this.food_matrix.read_matrix[i][j] || 0),255); // for overflow
		    ctx.fillStyle = "rgb(0," + foodfillValue + "," + hivefillValue + ")";
		var canvaspoint = AntSpace.point2Canvas(new Point(i,j));
		ctx.fillRect(canvaspoint.x-AntSpace.cellSize/2,canvaspoint.y-AntSpace.cellSize/2,AntSpace.cellSize,AntSpace.cellSize);
	}
    }
}

Pheromone.prototype.calculate = function(matrix,isHive) {
    for( var i = 0; i < this.antropy.ants.antNumber; ++i ) {
	matrix.write_matrix[this.antropy.ants.ant[i].x][this.antropy.ants.ant[i].y] =
	    (matrix.write_matrix[this.antropy.ants.ant[i].x][this.antropy.ants.ant[i].y] || 0) +
	    (isHive ? this.antropy.ants.ant[i].getEmittedPheromoneHive() : this.antropy.ants.ant[i].getEmittedPheromoneFood());
    }
}

Pheromone.prototype.diffuse = function(matrix) {
    // modifying write matrix according to the modified Diffuse2D source code in ant-ropy project
    for(var i = 0; i < matrix.write_matrix.length; ++i ) {
	for(var j = 0; j < matrix.write_matrix[i].length; ++j ) {
	    var avg = 0;
	    for( var k = 0; k < 8; ++k ) {
		     avg += matrix.read_matrix[AntSpace.crop2Space(i + Ants.NEIGHBOURS[k][0])]
		                            [AntSpace.crop2Space(j + Ants.NEIGHBOURS[k][1])] || 0;
	    }
	    /* value diffusing from neighbours */
        var d = avg/8 * (1 - this.diffusion_constant);
	    /* cell value losing some parts by diffusion */
	    var val = this.diffusion_constant * (matrix.read_matrix[i][j] || 0);
	    /* cell value getting the value of the neigbours */
	    val += d;
	    /* cell value after evaporation */
	    val *= (1 - this.evap_rate);
	    matrix.write_matrix[i][j] = val;
	}
    }
}

Pheromone.prototype.update = function() {
    this.hive_matrix.update();
    this.food_matrix.update();
}

Pheromone.prototype.getFoodAt = function(i,j) {
    return this.food_matrix.read_matrix[AntSpace.crop2Space(i)][AntSpace.crop2Space(j)];
}

Pheromone.prototype.getHiveAt = function(i,j) {
    return this.hive_matrix.read_matrix[AntSpace.crop2Space(i)][AntSpace.crop2Space(j)];
}
