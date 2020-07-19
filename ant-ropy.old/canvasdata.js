"use strict";

function CanvasData (canvasid) {
    this.realcanvas = document.getElementById(canvasid);
    this.ctx = this.realcanvas.getContext("2d");
    
    this.offCanvas = document.createElement("canvas");
    this.offCanvas.width = this.realcanvas.width;
    this.offCanvas.height = this.realcanvas.height;
    this.offctx = this.offCanvas.getContext("2d");
}

CanvasData.prototype = {
    get context() {
	return this.offctx;
    },
    get width() {
	return this.realcanvas.width;
    },
    get height() {
	return this.realcanvas.height;
    }
}

CanvasData.prototype.clear = function() {
    this.ctx.clearRect(0, 0, this.realcanvas.width, this.realcanvas.height);
    this.offctx.clearRect(0, 0, this.realcanvas.width, this.realcanvas.height);
}

CanvasData.prototype.paint = function() {
    this.ctx.drawImage(this.offCanvas,0,0);
}
