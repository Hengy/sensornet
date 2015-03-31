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
	
	var tempValue = currentModuleInfo.values[0];
	$("#current_module_temp").append(tempValue.toFixed(1) + '<div class="current_module_unit">&deg;C</div>');
}

function load_current_description() {
	$("#current_module_description_text").append(currentModuleInfo.description);
}
