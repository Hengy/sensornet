/*---------------------------------------------------
 * Get module list
 ---------------------------------------------------*/
function getModuleList() {
	$.ajax({
		
		url: "/getModuleList",				// URL of request
		
//		data: {							// name of list to get
//			"list": "moduleList"
//		},
		
		type: "GET",					// request type
		
		dataType: "json",				// type of data returned
		
		// if AJAX request was successful
		success: function(data) {
			//console.log("AJAX request success: moduleList");
			
			var list = '';
			$.each(data, function (i,x) {
				list += '<div class="moduleListButton" id="' + x[1] + '" onmouseover="moduleListButtonOver(this.id);" onmouseleave="moduleListButtonOut(this.id);" onmousedown="moduleListButtonDown(this.id);" onmouseup="moduleListButtonUp(this.id, LIST);">\n';
				list += '<div class="moduleListButtonIcon">' + '' + '</div>\n';
				list += '<div class="moduleListButtonName">' + x[3] + '</div>\n';
				list += '<div class="moduleListButtonDetails">' + x[2] + '</div>\n';
				list += '</div>\n';
			});
			
			//testing
			for (i = 0; i < 12; i++) {
				list += '<div class="moduleListButton" id="' + i + '" onmouseover="moduleListButtonOver(this.id);" onmouseleave="moduleListButtonOut(this.id);" onmousedown="moduleListButtonDown(this.id);" onmouseup="moduleListButtonUp(this.id, LIST);">\n';
				list += '<div class="moduleListButtonIcon">' + '' + '</div>\n';
				list += '<div class="moduleListButtonName">Test' + i + '</div>\n';
				list += '<div class="moduleListButtonDetails">Test' + i + ' Location</div>\n';
				list += '</div>\n';
			}
			
			$("#moduleList").append(list);
			
			nodeListSize();
		},
		
		// if AJAX request failed
		error: function(xhr,status,errorThrown) {
			console.log("Error: " + errorThrown);
			console.log("Status: " + status);
			console.log(xhr);
		}
		
	});
}

/*---------------------------------------------------
 * Get fav list
 ---------------------------------------------------*/
function getCardList() {
	$.ajax({
		
		url: "/getCardList",				// URL of request
		
//		data: {							// name of list to get
//			"list": "cardList"
//		},
		
		type: "GET",					// request type
		
		dataType: "json",				// type of data returned
		
		// if AJAX request was successful
		success: function(data) {
			//console.log("AJAX request success: cardList");
			//console.log(data);
			
			var list = '';
			$.each(data, function (i,x) {
				if (data[i][4] > 0 || data[i][8] > 0 || data[i][11] > 0) {
					if (data[i][4] > 1 || data[i][8] > 1 || data[i][11] > 1) {
						list += '<div class="homeCard cCritical" id="homeCard' + data[i][2] + '" onmouseup="cardUp(this.id, CARD);">\n';
					} else {
						list += '<div class="homeCard cWarning" id="homeCard' + data[i][2] + '" onmouseup="cardUp(this.id, CARD);">\n';
					}
				} else {
					list += '<div class="homeCard" id="homeCard' + data[i][2] + '" onmouseup="cardUp(this.id, CARD);">\n';
				}
				list += '\t<div class="homeCardName">' + data[i][6] + '</div>\n';
				list += '</div>\n';
			});
			$("#homeCardBox").append(list);
			
			cardListSize();
			displayCards(150);
		},
		
		// if AJAX request failed
		error: function(xhr,status,errorThrown) {
			console.log("Error: " + errorThrown);
			console.log("Status: " + status);
			console.log(xhr);
		}
		
	});
}

/*---------------------------------------------------
 * Get stats list
 ---------------------------------------------------*/
function getStats() {
	$.ajax({
		
		url: "/getStatsList",				// URL of request
		
//		data: {							// name of list to get
//			"list": "statsList"
//		},
		
		type: "GET",					// request type
		
		dataType: "json",				// type of data returned
		
		// if AJAX request was successful
		success: function(data) {
			//console.log("AJAX request success: favList");
			//console.log(data);
			
			var list = '';
			list += '<div class="homeContentStat">Nodes: ' + data[0][0] + '</div>\n';
			list += '<div class="homeContentStat">Modules: ' + data[0][1] + '</div>\n';
			list += '<div class="homeContentStat">Sensors: ' + data[0][2] + '</div>\n';
			list += '<div class="homeContentStat">Controls: ' + data[0][3] + '</div>\n';
			$("#homeContentStats").append(list);
			
			var list = '';
			list += '<div id="homeWarning" class="homeContentWarning">Warning: ' + data[0][4] + '</div>\n';
			list += '<div id="homeCritical" class="homeContentWarning">Critical: ' + data[0][5] + '</div>\n';
			
			$("#homeContentWarnings").append(list);
			
			if (data[0][4] > 0) {	// set color
				$("#homeWarning").addClass("warning");
			}
			
			if (data[0][5] > 0) {	// set color
				$("#homeCritical").addClass("critical");
			}
		},
		
		// if AJAX request failed
		error: function(xhr,status,errorThrown) {
			console.log("Error: " + errorThrown);
			console.log("Status: " + status);
			console.log(xhr);
		}
		
	});
}

/*---------------------------------------------------
 * Get module info
 ---------------------------------------------------*/
function getModuleInfo(moduleID) {
	$.ajax({
		
		url: "/getModuleInfo",				// URL of request
		
		data: {							// name of list to get
			"moduleID": moduleID
		},
		
		type: "GET",					// request type
		
		dataType: "json",				// type of data returned
		
		// if AJAX request was successful
		success: function(data) {
			console.log(data);
			
			if (data != 0) {
				
				$("#contentCurrent").hide();
				showLoading();
				
				currentModuleInfo.commands = [];
				currentModuleInfo.dataLens = [];
				currentModuleInfo.values = [];
				currentModuleInfo.types = [];
				currentModuleInfo.status = [];
				currentModuleInfo.archiveData = [];
				currentModuleInfo.criticalBounds = [];
				currentModuleInfo.warningBounds = [];
				
				querySuccess = true;
				
				$.each(currentModuleInfo, function (key, value) {
	            	value = null;
	        	});
				
				querySuccess = data['querySuccess'];
				
				if (querySuccess) {
					
					currentModuleInfo.ID = data['ID'];
					currentModuleInfo.nodeStatus = data['nodeStatus'];
					currentModuleInfo.name = data['updateInterval'];
					currentModuleInfo.description = data['description'];
					currentModuleInfo.moduleType = data['name'];
					currentModuleInfo.icon = data['icon'];
					currentModuleInfo.updateInterval = data['updateInterval'];
					currentModuleInfo.timestamp = data['timestamp'];
					currentModuleInfo.moduleStatus = data['moduleStatus'];
					currentModuleInfo.moduleFile = data['moduleFile'];
					currentModuleInfo.commands.push(data['commands']);
					currentModuleInfo.dataLens.push(data['dataLens']);
					currentModuleInfo.values.push(data['values']);
					currentModuleInfo.types.push(data['types']);
					currentModuleInfo.status.push(data['status']);
					currentModuleInfo.archiveData.push(data['archiveData']);
					currentModuleInfo.criticalBounds.push(data['criticalBounds']);
					currentModuleInfo.warningBounds.push(data['warningBounds']);
				}
				
				if (!querySuccess) {
					displayModuleInfo(querySuccess, "Data Error");
				} else {
					path = "../static/modules/"+ currentModuleInfo.moduleFile;
					loadModuleJS(path);
					loadModuleCSS(path);
					loadModuleHTML(path);
					
					//$.get(path + "/current.html")
					//   .done(function() {
					//        loadModuleJS(path);
					//		loadModuleCSS(path);
					//		loadModuleHTML(path);
					//    }).fail(function() { 
					//        displayModuleInfo(querySuccess, "Module folder does not exist");
					//    });
				}
				
				$("#contentCurrent").delay(1000).fadeIn(500);
				window.setTimeout(hideLoading, 1000);
			}
			
			//hideLoading();
		},
		
		// if AJAX request failed
		error: function(xhr,status,errorThrown) {
			console.log("Error: " + errorThrown);
			console.log("Status: " + status);
			console.log(xhr);
		}
		
	});
}

/*---------------------------------------------------
 * Update module value
 ---------------------------------------------------*/
function updateControlFast(newData) {
	$.ajax({
		
		url: "/updateControlFast",			// URL of request
		
		data: JSON.stringify({			// name of list to get
			"moduleID": newData[0],
			"commands": newData[1],
			"values": newData[2]
		}),
		
		type: "POST",					// request type
		
		contentType: "application/json; charset=utf-8",
		
		dataType: "json",				// type of data returned
		
		// if AJAX request was successful
		success: function(data) {
		},
		
		// if AJAX request failed
		error: function(xhr,status,errorThrown) {
			console.log("Error: " + errorThrown);
			console.log("Status: " + status);
			console.log(xhr);
		}
		
	});
}

/*---------------------------------------------------
 * Update module value
 ---------------------------------------------------*/
function updateControl(newData) {
	$.ajax({
		
		url: "/updateControl",			// URL of request
		
		data: JSON.stringify({			// name of list to get
			"moduleID": newData[0],
			"commands": newData[1],
			"values": newData[2]
		}),
		
		type: "POST",					// request type
		
		contentType: "application/json; charset=utf-8",
		
		dataType: "json",				// type of data returned
		
		// if AJAX request was successful
		success: function(data) {
		},
		
		// if AJAX request failed
		error: function(xhr,status,errorThrown) {
			console.log("Error: " + errorThrown);
			console.log("Status: " + status);
			console.log(xhr);
		}
		
	});
}

/*---------------------------------------------------
 * Update module value
 ---------------------------------------------------*/
function updateModule(data) {
	$.ajax({
		
		url: "/updateModule",			// URL of request
		
		data: JSON.stringify({			// name of list to get
			"moduleID": moduleID
		}),
		
		type: "POST",					// request type
		
		contentType: "application/json; charset=utf-8",
		
		dataType: "json",				// type of data returned
		
		// if AJAX request was successful
		success: function(data) {
			
		},
		
		// if AJAX request failed
		error: function(xhr,status,errorThrown) {
			console.log("Error: " + errorThrown);
			console.log("Status: " + status);
			console.log(xhr);
		}
		
	});
}