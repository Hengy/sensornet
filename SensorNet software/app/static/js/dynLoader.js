function loadModuleJS(path) {
	
	var fileref=document.createElement('script');	// create new DOM element
    fileref.setAttribute("type","text/javascript");
    fileref.setAttribute("src", path + "/module.js");
    
    document.getElementsByTagName("head")[0].appendChild(fileref);
}


function loadModuleCSS(path) {
	
	var fileref=document.createElement("link");	// create new DOM element
    fileref.setAttribute("rel", "stylesheet");
    fileref.setAttribute("type", "text/css");
    fileref.setAttribute("href", path + "/module.css");
	
	document.getElementsByTagName("head")[0].appendChild(fileref);
}


function loadModuleHTML(path) {
	$("#contentCurrent").load(path + "/current.html");
	$("#contentHistory").load(path + "/history.html");
	$("#contentConfig").load(path + "/config.html");
}