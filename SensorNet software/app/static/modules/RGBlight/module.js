function module_onLoad() {
	load_current_stats();
	load_current_values();
	load_current_description();
}

function load_current_stats() {
	var list = '';
	list += '<div class="current_module_stat">Node ID: ' + '0x' + currentModuleInfo.ID.toString(16).toUpperCase() + '</div>\n';
	list += '<div class="current_module_stat">Module ID: ' + '0x' + parseInt(currentModuleInfo.moduleID).toString(16).toUpperCase() + '</div>\n';
	list += '<div class="current_module_stat">Sensors: ' + currentModuleInfo.commands.length + '</div>\n';
	
	$("#current_module_stats").append(list);
}

function load_current_values() {
	
	var tempValue = currentModuleInfo.values[0] / 10.0;
	$("#current_module_temp").append(tempValue.toFixed(1) + '<div class="current_module_unit">&deg;C</div>');
}

function load_current_description() {
	$("#current_module_description_text").append(currentModuleInfo.description);
}

function changeHue(values) {
	var commands = [16];
	// var values = [r,g,b];
	var data = [currentModuleInfo.moduleID, commands, values];
	updateControlFast(data);
}

function HSBtoRGB(hue) {
  	var base = 0;
  	var sat = 255;
  	var valb = 255;
  	var red,green,blue;
  
  	base = ((255 - sat) * valb) >> 8; // ((255 - sat) * val(B)) / 256
    
	switch(hue/60) {
	  case 0:
	    red = valb;
	    green = (((valb - base) * hue) / 60) + base;
	    blue = base;
	    break;
	  case 1:
	    red = (((valb - base) * (60 - (hue % 60))) / 60) + base;
	    green = valb;
	    blue = base;
	    break;
	  case 2:
	    red = base;
	    green = valb;
	    blue = (((valb - base) * (hue % 60)) / 60) + base;
	    break;
	  case 3:
	    red = base;
	    green = (((valb - base) * (60 - (hue % 60))) / 60) + base;
	    blue = valb;
	    break;
	  case 4:
	    red = (((valb - base) * (hue % 60)) / 60) + base;
	    green = base;
	    blue = valb;
	    break;
	  case 5:
	    red = valb;
	    green = base;
	    blue = (((valb - base) * (60 - (hue % 60))) / 60) + base;
	    break;
	}
	console.log(red);
	console.log(green);
	console.log(blue);
	rgb = String(red)+","+String(green)+","+String(blue);
	
	return rgb;
}
