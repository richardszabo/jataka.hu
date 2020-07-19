function WhereAmIMap() {
    this.locationIdx = 0;
    this.hits = 0;
    this.misses = 0;
    this.level = 0;
    this.streetViewService = new google.maps.StreetViewService();
    this.panorama = new google.maps.StreetViewPanorama(
      document.getElementById('pano'), {
          position: new google.maps.LatLng(0,0),
        pov: {
          heading: 0,
          pitch: 0
        },
        visible: true,
        addressControl: false,
        linksControl: false,
        zoomControl: true,
        panControl: true,
        enableCloseButton: false
      });
/*      this.onPositionChange = this.refreshPanorama;
      this.panorama.addListener('position_changed', this.onPositionChange.bind(this));
      this.onPovChange = this.refreshPanoramaDir;
      this.panorama.addListener('pov_changed', this.onPovChange.bind(this));*/
  }

WhereAmIMap.CHOICES = 4;

WhereAmIMap.prototype.setPlaces = function(places_) {
    this.places = places_;
    // lookup creation based on http://stackoverflow.com/a/7364247/21047
    this.places_lookup = {};
    this.places_population = 0;
    for (var i = 0, len = this.places.length; i < len; i++) {
        this.places_lookup[this.places[i].id] = i;
        this.places_population += Number(this.places[i].pop);
    }
}

WhereAmIMap.prototype.getPlaceByPopulationRandom = function(rand) {
    var i = 0;
    for (var pop = 0, len = this.places.length; i < len && pop < rand; i++) {
        pop += Number(this.places[i].pop);
    }
    return i;
}

WhereAmIMap.prototype.setLevel = function(level_) {
    this.level = level_;
}

WhereAmIMap.prototype.setPossibleLocations = function() {
    this.locations = [];
    for (var i = 0, len = this.all_locations.length; i < len; i++) {
    	if( this.all_locations[i].level <= this.level ) {
    	    this.locations.push(this.all_locations[i]);
    	}
    }
}

WhereAmIMap.prototype.setLocations = function(locations_) {
    this.all_locations = locations_;
    this.setPossibleLocations();
}

WhereAmIMap.prototype.init = function() {
    this.initControl();
    this.initMap();
}

WhereAmIMap.prototype.initMap = function() {
    if( !this.locations || this.locations.length === 0 ) {
	       this.setPossibleLocations();
    }
    this.locationIdx = Math.floor(Math.random() * this.locations.length % this.locations.length);
    var level = document.getElementById("level");
    var levelStr = "";
    switch( this.level ) {
    case "0":
	levelStr = "Könnyű";
	break;
    case "1":
	levelStr = "Normál";
	break;
    case "2":
	levelStr = "Nehéz";
	break;
    }
    level.innerHTML = levelStr;
    this.lastPosition = new google.maps.LatLng(this.locations[this.locationIdx].pos[0],this.locations[this.locationIdx].pos[1])
    this.streetViewService.getPanoramaByLocation(this.lastPosition, 50, function(panoramaData, status) {
        if (status == google.maps.StreetViewStatus.OK) {
            var panoOptions = {
                position: panoramaData.location.latLng
            }
            this.panorama.setOptions(panoOptions);
            this.initPlaces();
            //this.panorama.setVisible(true);
        } else {
            this.initMap();
        }
    }.bind(this));
    //this.panorama.setPosition(this.lastPosition);
    //this.initPlaces();
    //this.panorama.setVisible(true);
}

/*WhereAmIMap.prototype.refreshPanorama = function() {
    console.log("panorama:"+this.panorama.getPosition());
    if( this.panorama.getPosition() !== this.lastPosition ) {
        console.log("equal");
        this.panorama.setPov({
            heading: Number(this.locations[this.locationIdx].pos[2]),
            pitch: 0
        });
    } else {
        console.log("not equal");
    }
}

WhereAmIMap.prototype.refreshPanoramaDir = function() {
    this.panorama.setVisible(true);
}*/

WhereAmIMap.prototype.initControl = function() {
    this.option_buttons = document.getElementById("option_buttons");
    if( !this.option_buttons.children.length ) {
    	for( var i = 0; i < WhereAmIMap.CHOICES; ++i ) {
    	    var button = document.createElement("button");
    	    button.id = "option" + i;
            var div = document.createElement("div");
    	    this.option_buttons.appendChild(div).appendChild(button);
    	}
    }
}

WhereAmIMap.prototype.initPlaceButton = function(button, id) {
    button.innerHTML = this.places[this.places_lookup[id]].name;
    button.value = id;
    button.onclick = this.next.bind(this, button);
}

WhereAmIMap.prototype.initPlaces = function() {
    if( this.hits + this.misses === 50 ) {
	this.endGame();
    }
    var selected = Math.floor(Math.random() * WhereAmIMap.CHOICES % WhereAmIMap.CHOICES);
    var button = document.getElementById("option" + selected);
    this.initPlaceButton(button,this.locations[this.locationIdx].id);
    var usedPlacesById = [Number(this.locations[this.locationIdx].id)];
    for( var i = 0; i < WhereAmIMap.CHOICES; ++i ) {
    	if( i !== selected ) {
    	    button = document.getElementById("option" + i);
            var randId;
            do {
    	        var rand = Math.ceil(Math.random() * this.places_population % this.places_population);
                randId = this.getPlaceByPopulationRandom(rand);
            } while ( usedPlacesById.indexOf(randId) !== -1 );
            this.initPlaceButton(button,randId);
            usedPlacesById.push(randId);
        }
    }
}

WhereAmIMap.prototype.next = function(button) {
    if( button.value === this.locations[this.locationIdx].id ) {
	    ++this.hits;
	    document.getElementById("hits").innerHTML = this.hits;
    } else {
	    ++this.misses;
	    document.getElementById("misses").innerHTML = this.misses;
    }
    this.locations.splice(this.locationIdx,1);
    //this.panorama.setVisible(false);
    this.initMap();
}

WhereAmIMap.prototype.endGame = function() {
    document.getElementById("control").style.display = "none";
    document.getElementById("endgame").style.display = "block";
    document.getElementById("maxpoint").innerHTML = this.hits + this.misses;
    document.getElementById("point").innerHTML = this.hits;
}
