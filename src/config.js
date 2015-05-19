var isPebbleTime;
var options = {};
var customData = {};

Pebble.addEventListener("ready", function(e) {
	console.log("PebbleKit JS is ready!");
	if (Pebble.getActiveWatchInfo) {
		var info = Pebble.getActiveWatchInfo();
		//console.log("DEBUG: Pebble Model: " + info.model);
		isPebbleTime = (info.model.indexOf("basalt") != -1 || info.model.indexOf("time") != -1 ) ? true : false;
		//console.log("DEBUG: isPebbleTime: " + isPebbleTime);
	} else {
		console.log("Pebble model not avialable!");
	}
});

Pebble.addEventListener("showConfiguration", function(e) {
	console.log("Showing Configuration Page!");
	Pebble.openURL('http://flashback2k14.github.io/hackoclock/public/index_min.html?' + encodeURIComponent(JSON.stringify(options)));
	//console.log("DEBUG: Options: " + JSON.stringify(options));
	//console.log("DEBUG: encode Options: " + encodeURIComponent(JSON.stringify(options)));
});

Pebble.addEventListener("webviewclosed", function(e) {
	console.log("Configuration Page closed!");
	if (e.response.charAt(0) === "{" && e.response.slice(-1) === "}" && e.response.length > 5) {
		options = JSON.parse(decodeURIComponent(e.response));
		//console.log("DEBUG: Options: " + JSON.stringify(options));
		
		if (isPebbleTime) {
			customData = {
				"KEY_SHOW_ALWAYS_TIME" : options.selectAlwaysTime,
				"KEY_CUSTOM_NEEDED_TAPS" : options.inputNeededTaps,
				"KEY_BG_BEER"  : options.bgBeer,
				"KEY_BG_HACK" : options.bgHack,
				"KEY_BG_OCLOCK" : options.bgOclock,
				"KEY_BG_TIME" : options.bgTime,
				"KEY_BG_WINBG" : options.bgWbg,
				"KEY_TC_BEER"  : options.tcBeer,
				"KEY_TC_HACK"  : options.tcHack,
				"KEY_TC_OCLOCK" : options.tcOclock,
				"KEY_TC_TIME" : options.tcTime
			};
		} else {
			customData = {
				"KEY_SHOW_ALWAYS_TIME" : options.selectAlwaysTime,
				"KEY_CUSTOM_NEEDED_TAPS" : options.inputNeededTaps,
				"KEY_BG_PBL_BEER"  : options.bgPblBeer,
				"KEY_BG_PBL_HACK" : options.bgPblHack,
				"KEY_BG_PBL_OCLOCK" : options.bgPblOclock,
				"KEY_BG_PBL_TIME" : options.bgPblTime,
				"KEY_BG_PBL_WINBG" : options.bgPblWbg,
				"KEY_TC_PBL_BEER"  : options.tcPblBeer,
				"KEY_TC_PBL_HACK"  : options.tcPblHack,
				"KEY_TC_PBL_OCLOCK" : options.tcPblOclock,
				"KEY_TC_PBL_TIME" : options.tcPblTime
			};
		}
		//console.log("DEBUG: CustomData: " + JSON.stringify(customData));
		
		Pebble.sendAppMessage(customData, 
		function(e) {console.log("Sending customisation data to pebble...");},
		function(e) {console.log("Sending customisation data to pebble failed!");});
		
	} else {
		console.log("Configuration Page cancelled!");
	}
});