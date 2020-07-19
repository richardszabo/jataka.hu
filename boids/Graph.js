function Graph(context,plot_context,xmin,xmax,ymin,ymax,x0,y0,xwidth,ywidth) {    // VARIABLE DECLARATIONS    	// canvas context on which to draw graph instance            var ctx = context;            var pctx =  plot_context;    	// location of origin (in pixels) in parent document    	var x_orig;    	var y_orig;    	// overall width and height of graph in pixels    	var x_width;    	var y_width;    	// min and max of x and y relative to origin (in pixels)    	var x_min_rel;    	var x_max_rel;    	var y_min_rel;    	var y_max_rel;    	// obvious    	var x_tick_major;    	var x_tick_minor;    	var y_tick_major;    	var y_tick_minor;    	// scaling used in displaying values on the axes    	var x_displ_scal;    	var y_displ_scal;    	// width and height of textbox used for displaying values on the axes    	// this should not have to be tampered with (I hope)    	var tw=15;    	var th=20;    	// declarations for quantities to be used later    	var x_min;    	var x_max;    	var y_min;    	var y_max;    	var xx;    	var yy;    	var x_displ;    	var y_displ;    	var txpos;    	var typos;    // PARAMETER ASSIGNMENTS    	// assign parameter values based on specified arguments    	x_orig=x0;    	y_orig=y0;    	x_width=xwidth;    	y_width=ywidth;    	//    	x_displ_scal=(xmax-xmin)/xwidth;    	y_displ_scal=(ymax-ymin)/ywidth;    	//    	x_min_rel=xmin/x_displ_scal;    	x_max_rel=xmax/x_displ_scal;    	y_min_rel=ymin/y_displ_scal;    	y_max_rel=ymax/y_displ_scal;    	// convert to absolute coordinates    	x_min=x_min_rel + x_orig;    	x_max=x_max_rel + x_orig;    	y_min=y_orig - y_min_rel;    	y_max=y_orig - y_max_rel;    	txpos=x_orig - tw;    	typos=y_orig;    // METHODS    	// DRAW GRID: draw major, minor lines and display values    	this.drawgrid = function(xmajor,xminor,ymajor,yminor){    		x_tick_major=xmajor/x_displ_scal;    		x_tick_minor=xminor/x_displ_scal;    		y_tick_major=ymajor/y_displ_scal;    		y_tick_minor=yminor/y_displ_scal;    		// draw major grid lines    		ctx.strokeStyle = '#999999';    		ctx.lineWidth = 1;    		ctx.beginPath() ;    		yy=y_max;    		do {    			ctx.moveTo(x_min,yy);    			ctx.lineTo(x_max,yy);    			yy+= y_tick_major;    		} while (yy <= y_min);    		xx=x_min;    		do {    			ctx.moveTo(xx,y_min);    			ctx.lineTo(xx,y_max);    			xx+= x_tick_major;    		} while (xx <= x_max);    		ctx.stroke();    		// draw minor grid lines    		ctx.strokeStyle = '#cccccc';    		ctx.lineWidth = 1;    		ctx.beginPath() ;    		yy=y_max;    		do {    			ctx.moveTo(x_min,yy);    			ctx.lineTo(x_max,yy);    			yy+= y_tick_minor;    		} while (yy <= y_min);    		xx=x_min;    		do {    			ctx.moveTo(xx,y_min);    			ctx.lineTo(xx,y_max);    			xx+= x_tick_minor;    		} while (xx <= x_max);    		ctx.stroke();    		/*//display values    		ctx.font = "10pt Lato";    		ctx.fillStyle = '#000000';    		ctx.textAlign = "right";    		ctx.textBaseline = "top";    		yy=y_max;    		do {    		    y_displ=Math.round((y_orig - yy) * y_displ_scal);    			ctx.fillText(y_displ,txpos + 5,yy - th / 2);    			yy+= y_tick_major;    		} while (yy <= y_min);    		ctx.textAlign = "left";    		ctx.textBaseline = "top";    		xx=x_min;    		do {    		    x_displ=Math.round((xx - x_orig) * x_displ_scal);    			ctx.fillText(x_displ,xx - tw + 10,typos + 5);    			xx+= x_tick_major;    		} while (xx <= x_max);*/    	};    	// DRAW AXES: draw axes and labels    	this.drawaxes = function (xlabel,ylabel){    		if(typeof(xlabel)==='undefined') xlabel = 'x';    		if(typeof(ylabel)==='undefined') ylabel = 'y';    		ctx.strokeStyle = '#000000';    		ctx.lineWidth = 2;    		ctx.beginPath() ;    		ctx.moveTo(x_min,y_orig);    		ctx.lineTo(x_max,y_orig);    		ctx.moveTo(x_orig,y_min);    		ctx.lineTo(x_orig,y_max);    		ctx.stroke();    		/*//axis labels    		ctx.font = "12pt Lato";    		ctx.fillStyle = '#000000';    		ctx.textAlign = "left";    		ctx.textBaseline = "top";    		ctx.fillText(xlabel,x_max + 0.75 * tw,typos - th / 2);    		ctx.fillText(ylabel,txpos + tw / 2 +5,y_max - 1.5 * th);*/    	};        this.plot1data = function (x,y){    			xpos=x_orig+x/x_displ_scal;    			ypos=y_orig-y/y_displ_scal;    	                pctx.beginPath();    	    pctx.arc(xpos,ypos,1,0,2*Math.PI,true);    		        pctx.stroke();        };    	// PLOT DATA: plot data    	this.plot = function (xArr, pColor){    		// the last three arguments have default values    		if(typeof(pColor)==='undefined') pColor = '#0000ff';    		pctx.strokeStyle = pColor;    		pctx.lineWidth = 1;        	        for (var i in xArr){    		    this.plot1data(i,xArr[i]);    		}    	};            // plot 1 dot            this.plot1 = function (x, oldy, newy, pColor){    	   if(typeof(pColor)==='undefined') pColor = '#0000ff';    	    pctx.lineWidth = 1;    	    var gco = pctx.globalCompositeOperation;    	    pctx.globalCompositeOperation = "destination-out";                pctx.strokeStyle = "rgba(255,255,255,1.0)";                this.plot1data(x,oldy);    	    pctx.globalCompositeOperation = gco;     	    pctx.strokeStyle = pColor;                this.plot1data(x,newy);    	};    }