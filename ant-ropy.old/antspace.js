"use strict";

function AntSpace (canvasWidth,canvasHeight) {
    AntSpace.cellSize = Math.floor(Math.min(canvasWidth,canvasHeight) / AntSpace.spaceSize);
}

AntSpace.cellSize;
AntSpace.spaceSize = 100;

AntSpace.point2Canvas = function(spacepoint) {
    return new Point(spacepoint.x*AntSpace.cellSize,spacepoint.y*AntSpace.cellSize);
}

AntSpace.canvas2Point = function(point) {
    return new Point(Math.floor(point.x/AntSpace.cellSize),Math.floor(point.y/AntSpace.cellSize));
}

AntSpace.center = function() {
    return new Point(AntSpace.spaceSize/2,AntSpace.spaceSize/2);
}

AntSpace.crop2Space = function(x) {
    return (x + AntSpace.spaceSize) % AntSpace.spaceSize;
}

/*AntSpace.crop2Space = function(point) {
    return new Point((point.x + AntSpace.spaceSize) % AntSpace.spaceSize,(point.y + AntSpace.spaceSize) % AntSpace.spaceSize);
}*/
