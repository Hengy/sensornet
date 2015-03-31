/*---------------------------------------------------
 * Display raw info in current
 ---------------------------------------------------*/
function displayModuleInfo(success, msg) {
	
	if (success) {	
		var list = '';
		
		$.each(currentModuleInfo, function (key, value) {
	            list += '<div style="padding: 5px 0px 0px 20px; color: #999999; font-size:14px;">' + key + ': </div><div style="padding-left:30px; font-size: 18px; color: #FFFFFF;">' + value + ' </div>';
	        });
		
		$("#contentCurrent").html(list);
	} else {
		$("#contentCurrent").html('<div style="padding: 30px; font-size: 32px; color:#FF0000;">'+ msg +'</div>');
	}
}

/*---------------------------------------------------
 * Display name of module
 ---------------------------------------------------*/
function displayName(id, buttonName) {
	
	if (buttonName == "Home" || buttonName == "System") {	// Home or System button?
		
		$("#mainTitle").html(buttonName);
		
	} else if (buttonName == "moduleList" || buttonName == "cardList") { // button from module or card list?
		
		var name = "";
		if (buttonName == "moduleList") {
			name = $("#"+id+" .moduleListButtonName").html();
		} else {
			name = $("#"+id+" .homeCardName").html();
		}
		$("#mainTitle").html(name + buttonName);
		
	} else { // must have been Current, History, or Config
		
		name = $("#"+id+" .moduleListButtonName").html();
		
	}
}

/*---------------------------------------------------
 * Switch content pane
 ---------------------------------------------------*/
function switchContent(context, id, buttonName) {
	// fade out all content
	$("#contentHome, #contentCurrent, #contentHistory, #contentConfig, #contentSystem").fadeOut(100);
	$(".homeCard").hide();
	
	if (context == "currentID") { 										// if main menu button was pressed
		
		//refresh button
		if (id == "mainConfig" || id == "mainSystem") {					// if "Config" or "System" was pressed
			$("#mainRefreshButton").fadeOut(100);						// fade out refresh button
		} else {														// otherwise
			if ($("#mainRefreshButton").css("display") == "none") {		// if refresh button is not already displayed
				$("#mainRefreshButton").hide().fadeIn(200);				// fade it in
			}
		}

		// content panel
		var contentID = id.replace("main", "content");		// replace "main" in ID with "content" to fade in selected panel
		$("#"+contentID).hide().delay(101).fadeIn(300);
		
		if (id == "mainHome") {
			displayCards(200);
		}
		
	} else {												// otherwise a list button was pressed
		$("#contentCurrent").hide().delay(101).fadeIn(300);	// switch to current panel
	}
	
	displayName(id,buttonName);	// change title of current panel to name of module
}

/*---------------------------------------------------
 * Display fav cards
 ---------------------------------------------------*/
function displayStats() {
	$("#homeContentStats").fadeIn(100);
	$("#homeContentWarnings").fadeIn(100);
}

/*---------------------------------------------------
 * Display stats
 ---------------------------------------------------*/
function displayCards(delay) {
	$("#homeCardBox").fadeIn(100);
	$(".homeCard").each(function(x) {
		$(this).delay(x*delay).fadeIn(200);
	});
}

/*---------------------------------------------------
 * new ID selected. load ID info and enable buttons
 ---------------------------------------------------*/
function selectedID(id) {
	menuHide();
	switchContent("newID",id);
	$(".mainModuleButton").removeClass("active");
	$("#mainCurrent").addClass("active");
	
	// Enable Current, History and Config buttons
	$("#mainCurrent, #mainHistory, #mainConfig").css("color","#AAAAAA");
	$("#mainCurrent, #mainHistory, #mainConfig").css("cursor","pointer");
	
	window.currentModuleInfo.moduleID = id;
	window.currentModuleID = id;
	
	getModuleInfo(id);
}

/*---------------------------------------------------
 * Load data and hide "Scanning..." when done
 ---------------------------------------------------*/
function loadData() {
	getModuleList();	// Get list of modules
	
	getCardList();		// get list of favs
	
	getStats();			// get stats
	
	// get time of page load
	var d = new Date();
	var endLoadTime = d.getTime();
	
	if ((endLoadTime - window.startLoadTime) < 2000) {
		window.setTimeout( hideLoading, 2000 - (endLoadTime - window.startLoadTime) );
	} else {
		hideLoading();
	}
}

/*---------------------------------------------------
 * execute when page is done loading
 ---------------------------------------------------*/
function pageLoaded() {	
	// show and reload loading gif
	$("#gif").css("visibility", "visible");
	$("#gif").src = "/static/images/loader.gif";
	
	// Constants
	window.REFRESH = "Refresh";
	window.HOME = "Home";
	window.CURRENT = "Current";
	window.HISTORY = "History";
	window.CONFIG = "Configuration";
	window.SYSTEM = "System";
	window.CARD = "Card";
	window.LIST = "List";
	
	window.currentModuleID = 0;			// ID of current module in mainCurrent panel; 0 if no module prev selected
	window.currentScreen = "mainHome";	// Name of current viewable main panel; default is mainHome panel
	window.currentModuleInfo = {		// Dictionary holding values for current module
		"ID": 0,				// Node ID
		"nodeStatus": 0,		// Node Status
		"moduleID": 0,			// Module ID
		"name": "",				// Module Name
		"description": "",		// Module Description
		"moduleType": 0,		// Module Type
		"icon": "default",		// Module Icon
		"updateInterval": 0,	// Update Interval
		"timestamp": "",		// Timestamp of last value
		"moduleStatus": 0,		// Module Status
		"moduleFile": "",		// Module File Name
		"commands": [],			// List of commands in Module
		"dataLens": [],			// Data Lengths of commands
		"values": [],			// Values of commands
		"types": [],			// Types of commands
		"status": [],			// Command Statuses
		"archiveData": [],		// Archive Data for commands
		"criticalBounds": [],	// Critical Bounds of commands
		"warningBounds": []	// Warning Bounds of commands
	};
	
	// Disable buttons when there is no current ModuleID
	$("#mainCurrent, #mainHistory, #mainConfig").css("color","#333333");
	$("#mainCurrent, #mainHistory, #mainConfig").css("cursor","default");
	
	loadData();
	
	nodeListSize();
	cardListSize();
}