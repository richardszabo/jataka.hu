"use strict";

function Antropy () {
    this.inited = false;
    this.antNumber;
    this.foodSource;
    this.foodNumber;
    this.stepNumber;
    this.run = null;
}

Antropy.prototype.setCanvases = function(p_canvasid, p_pheromone_canvasid) {
    this.canvasid = p_canvasid;
    this.pheromone_canvasid = p_pheromone_canvasid;
}

Antropy.prototype.setEntropyCanvases = function(p_entropy_canvasid, p_plot_canvasid) {
    this.entropy_canvasid = p_entropy_canvasid;
    this.plot_canvasid = p_plot_canvasid;
}

Antropy.prototype.setWindow = function(p_window) {
    this.window = p_window;
}

Antropy.prototype.setButton = function(p_button) {
    this.button = p_button;
}

Antropy.prototype.reset = function() {
    this.seed = document.getElementById("seed").value;
    this.antNumber = document.getElementById("antnum").value;
    this.foodSource = document.getElementById("foodsource").value;
    this.foodNumber = document.getElementById("foodnum").value;
    Math.seedrandom(this.seed);
    this.inited = false;
    this.run = null;
    this.init();
}

Antropy.prototype.init = function() {
    //Canvas stuff
    this.canvas = new CanvasData(this.canvasid);
    this.canvas.realcanvas.addEventListener("mousedown", this.showCellData, false);
    this.canvas.realcanvas.ownParam = this;

    this.pheromone_canvas = new CanvasData(this.pheromone_canvasid);

    this.entropy_canvas = new CanvasData(this.entropy_canvasid);
    this.plot_canvas = new CanvasData(this.plot_canvasid);

    this.antSpace = new AntSpace(this.canvas.width,this.canvas.height);

    this.hive = new Hive();
    this.foods = new Foods(this);
    this.pheromone = new Pheromone(this);
    this.ants = new Ants(this);
    this.antEntropy = 0;
    this.foodEntropy = 0;
    this.graphHandler = new GraphHandler(this);

    this.stepNumber = 0;
    this.inited = true;
    this.draw();
}

Antropy.prototype.step = function() {
    var start = +new Date(); // log start timestamp
    if( !this.inited ) {
        this.reset();
    }
    this.pheromone.step();
    this.ants.step();

    if( this.hive.getFood() == this.foods.maxFood && this.run ) {
        this.toggle();
    }

    this.draw();
    var end =  +new Date();  // log end timestamp
    var diff = end - start;
    document.getElementById("speed").value = diff;
    document.getElementById("food").value = this.hive.getFood() + "/" + this.foods.maxFood + (this.hive.getFood() == this.foods.maxFood ? " ALL COLLECTED" : "");
    document.getElementById("stepnum").value = ++this.stepNumber;
    document.getElementById("ant_food").value = this.ants.carryingFood;
    document.getElementById("ant_search").value = this.ants.antNumber - this.ants.carryingFood;
    this.antEntropy = Math.round(getEntropy(this.ants.ant)*10000)/10000;
    document.getElementById("ant_entropy").value = this.antEntropy;
    this.foodEntropy = Math.round(getEntropy(this.foods.food)*10000)/10000;
    document.getElementById("food_entropy").value = this.foodEntropy;
    if( this.ants.selected_ant_id !== null ) {
        document.getElementById("ant_id").value = this.ants.selected_ant_id;
        var selected_ant = this.ants.ant[this.ants.selected_ant_id];
        document.getElementById("ant_x").value = selected_ant.x;
        document.getElementById("ant_y").value = selected_ant.y;
        document.getElementById("ant_heading").value = selected_ant.heading;
        document.getElementById("ant_mode").value = selected_ant.mode === Ants.MODE_SEARCH ? "SEARCH" : "HOME";
        var foodStr = "";
        var hiveStr = "";
    }
}

Antropy.prototype.toggle = function() {
    if (this.run){
        this.run = this.window.clearInterval(this.run);
        this.button.innerHTML = 'Run';
    } else {
        this.run = this.window.setInterval(function(){ antropy.step(); },10);
        this.button.innerHTML = 'Stop';
    }
}

Antropy.prototype.draw = function() {
    this.canvas.clear();
    this.pheromone_canvas.clear();

    this.hive.draw(this.canvas.context);
    this.foods.draw(this.canvas.context);
    this.pheromone.draw(this.pheromone_canvas.context);
    this.ants.draw(this.canvas.context);

    this.canvas.paint();
    this.pheromone_canvas.paint();

    this.graphHandler.draw(this.antEntropy,this.foodEntropy);
}

Antropy.prototype.showCellData = function(event) {
    var point = AntSpace.canvas2Point(new Point(event.clientX - canvas.offsetLeft,event.clientY - canvas.offsetTop));
    var antropy = event.target.ownParam;
    var foodStr = "";
    var hiveStr = "";
    for( var i = -1; i <= 1; ++i ) {
	    for( var j = -1; j <= 1; ++j ) {
	        var food = Math.round(antropy.pheromone.getFoodAt(point.x+j,point.y+i)*100)/100;
	        foodStr += lpad(decimal_pad(food,2,"0")," ",5) + ", ";
	        var hive = Math.round(antropy.pheromone.getHiveAt(point.x+i,point.y+i)*100)/100;
	        hiveStr += lpad(decimal_pad(hive,2,"0")," ",5) + ", ";
	    }
	    foodStr += "\n";
	    hiveStr += "\n";
    }
    antropy.ants.selectAnt(point);
    //alert("x: " + point.x + "\ny: " + point.y + "\nfood:\n" + foodStr + "\nhive:\n" + hiveStr);
}
