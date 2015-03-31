/*-------------------------------------------------------------------------------------
 * Menu
 ------------------------------------------------------------------------------------*/

/*---------------------------------------------------
 * Slide menu out of view
 ---------------------------------------------------*/
function menuHide() {
	$("#menuTitle, #search, #moduleList, .moduleListButton, #moduleListUp, #moduleListDown").fadeOut(400);
	
	$("#menuSideTitle").hide().fadeIn(400);
		
	$("#menuSide").animate({
		left: "-400px"
	}, 400);
	
	$("#main").animate({
		opacity: 1
	}, 500);
}

/*---------------------------------------------------
 * Slide menu into view
 ---------------------------------------------------*/
function menuShow() {
	$("#main").animate({
		opacity: 0.4
	}, 500);
	
	$("#menuSideTitle").hide();
		
	$("#menuSide").animate({
		left: "0px"
	}, 200);
	
	$("#menuTitle, #search, #moduleList").fadeIn(1000);
	$("#moduleList").delay(300).fadeIn(500);
	
	$("#moduleListUp").fadeIn(200);
	$(".moduleListButton").each(function(x) {
		$(this).delay(x*100).fadeIn(200);
	});
	$("#moduleListDown").delay( ( Math.floor( (window.innerHeight - 280) / 60) )*100).fadeIn(200);
}

/*---------------------------------------------------
 * Hide or show  menu; dim  main
 ---------------------------------------------------*/
function menuButtonClick() {
	$("#menuButton, .menuButtonBG").css("fill", "#111111");
	$("#menuButton, .menuButtonHz").css("fill", "#4568E2");
	$("#menuButton, .menuButtonBG").css("stroke", "#4568e2");
	setTimeout(function () {
		$("#menuButton, .menuButtonHz").css("fill", "#FFFFFF");
	}, 100);
	
	var menuState = $("#menuSide").css("left");
	
	if (menuState == "-400px") {
		menuShow();
	} else {
		menuHide();		
	}
}

/*---------------------------------------------------
 * Hide menu if click is outside
 ---------------------------------------------------*/
$(document).mouseup(function (e)
{
    var container = $("#menuSide");

    if (!container.is(e.target) // if the target of the click isn't the container...
        && container.has(e.target).length === 0) // ... nor a descendant of the container
    {
        var menuState = $("#menuSide").css("left");
	
		if (menuState != "-400px") {
			menuHide();	
		}
    }
});

/*-------------------------------------------------------------------------------------
 * Buttons
 ------------------------------------------------------------------------------------*/

/*---------------------------------------------------
 * Menu button
 ---------------------------------------------------*/
function menuButtonOver() {
	$("#menuButton, .menuButtonBG").css("fill", "#111111");
	$("#menuButton, .menuButtonHz").css("fill", "#FFFFFF");
	$("#menuButton, .menuButtonBG").css("stroke", "#4568e2");
	$("#menuSideTitle").css("color", "#EEEEEE");
}

function menuButtonOut() {
	$("#menuButton, .menuButtonBG").css("fill", "#666666");
	$("#menuButton, .menuButtonHz").css("fill", "#CCCCCC");
	$("#menuButton, .menuButtonBG").css("stroke", "none");
	$("#menuSideTitle").css("color", "#888888");
}

/*---------------------------------------------------
 * List scroll buttons
 ---------------------------------------------------*/
function moduleListUpOver() {
	if ($("#moduleList").height() < ( $(".moduleListButton").size() * 60) ) {
		$("#moduleListUp").css("border-right", "1px solid #FFFFFF");
		$("#moduleListUp").css("background-color", "#444444");
		$("#moduleListUpArrow").css("fill","#CCCCCC");
	}
}

function moduleListUpOut() {
	if ($("#moduleList").height() < ( $(".moduleListButton").size() * 60) ) {
		$("#moduleListUp").css("border-right", "1px solid #666666");
		$("#moduleListUp").css("background-color", "#191919");
		$("#moduleListUpArrow").css("fill","#555555");
	}
}

function moduleListUpDown() {
	if ($("#moduleList").height() < ( $(".moduleListButton").size() * 60) ) {
		$("#moduleListUp").css("border-right", "1px solid #FFFFFF");
		$("#moduleListUp").css("background-color", "#4568e2");
		$("#moduleListUpArrow").css("fill","#FFFFFF");
	}
}

function moduleListUpUp() {
	$("#moduleListUp").css("background-color", "#444444");
	$("#moduleListUpArrow").css("fill","#CCCCCC");
	if (($("#moduleList").scrollTop() - 60) < 0) {	// if list is at the top
		$("#moduleList").scrollTop(0);
	} else {	// else scroll
		newPos = (Math.floor($("#moduleList").scrollTop() / 60) * 60) - 60;	// calc scroll position to next interval of 60px
		$("#moduleList").animate({
			scrollTop: newPos+"px"
		}, 150);
	}
}

function moduleListDownOver() {
	if ($("#moduleList").height() < ( $(".moduleListButton").size() * 60) ) {
		$("#moduleListDown").css("border-right", "1px solid #FFFFFF");
		$("#moduleListDown").css("background-color", "#444444");
		$("#moduleListDownArrow").css("fill","#CCCCCC");
	}
}

function moduleListDownOut() {
	if ($("#moduleList").height() < ( $(".moduleListButton").size() * 60) ) {
		$("#moduleListDown").css("border-right", "1px solid #666666");
		$("#moduleListDown").css("background-color", "#191919");
		$("#moduleListDownArrow").css("fill","#555555");
	}
}

function moduleListDownDown() {
	if ($("#moduleList").height() < ( $(".moduleListButton").size() * 60) ) {
		$("#moduleListDown").css("border-right", "1px solid #FFFFFF");
		$("#moduleListDown").css("background-color", "#4568e2");
		$("#moduleListDownArrow").css("fill","#FFFFFF");
	}
}

function moduleListDownUp() {
	$("#moduleListDown").css("background-color", "#444444");
	$("#moduleListDownArrow").css("fill","#CCCCCC");
	if (( ( $("#moduleList").scrollTop() + 60 ) > $("#moduleList").height()) < 0) {	// if list is at the top
		$("#moduleList").scrollTop($("#moduleList").height());
	} else {	// else scroll
		newPos = (Math.floor($("#moduleList").scrollTop() / 60) * 60) + 60;	// calc scroll position to next interval of 60px
		$("#moduleList").animate({
			scrollTop: newPos+"px"
		}, 150);
	}
}

/*---------------------------------------------------
 * Refresh button
 ---------------------------------------------------*/
function mainRefreshButtonOver() {
	$("#mainRefreshButtonSymbol .arrow").css("fill","#FFFFFF");
}

function mainRefreshButtonOut() {
	$("#mainRefreshButtonSymbol .arrow").css("fill","#999999");
}

function mainRefreshButtonDown() {
	$("#mainRefreshButtonSymbol .arrow").css("fill","#FFFFFF");
}

function mainRefreshButtonUp() {
	$("#mainRefreshButtonSymbol .arrow").css("fill","#FFFFFF");
}

/*---------------------------------------------------
 * List buttons
 ---------------------------------------------------*/
function moduleListButtonOver(id) {
	$("#"+id).children(".moduleListButtonDetails").css("color","#CCCCCC");
}

function moduleListButtonOut(id) {
	$("#"+id).children(".moduleListButtonDetails").css("color","#888888");
}

function moduleListButtonDown(id) {
	$("#"+id).children(".moduleListButtonDetails").css("color","#EEEEEE");
}

function moduleListButtonUp(id,buttonName) {
	$(".moduleListButton").removeClass("active");
	$("#"+id).addClass("active");
	$("#"+id).children(".moduleListButtonDetails").css("color","#EEEEEE");
	
	selectedID(id, buttonName);
}

/*---------------------------------------------------
 * Main buttons
 ---------------------------------------------------*/
function mainModuleButtonOver(id) {
	if ( ( id == "mainCurrent" || id == "mainHistory" || id == "mainConfig" ) && window.currentModuleID != 0 ) {
		$("#"+id).css("background-color","#444444");
		$("#"+id).css("border-right","1px solid #FFFFFF");
		$("#"+id).css("color","#FFFFFF");
	} else if (id == "mainHome" || id == "mainSystem") {
		$("#"+id).css("background-color","#444444");
		$("#"+id).css("border-right","1px solid #FFFFFF");
		$("#"+id).css("color","#FFFFFF");
	}
}

function mainModuleButtonOut(id) {
	if ( ( id == "mainCurrent" || id == "mainHistory" || id == "mainConfig" ) && window.currentModuleID != 0 ) {
		$("#"+id).css("background-color","#141414");
		$("#"+id).css("border-right","1px solid #444444");
		$("#"+id).css("color","#AAAAAA");
	} else if (id == "mainHome" || id == "mainSystem") {
		$("#"+id).css("background-color","#141414");
		$("#"+id).css("border-right","1px solid #444444");
		$("#"+id).css("color","#AAAAAA");
	}
}

function mainModuleButtonDown(id) {
	if ( ( id == "mainCurrent" || id == "mainHistory" || id == "mainConfig" ) && window.currentModuleID != 0 ) {
		$("#"+id).css("background-color","#4568e2");
		$("#"+id).css("border-right","1px solid #FFFFFF");
		$("#"+id).css("color","#FFFFFF");
	} else if (id == "mainHome" || id == "mainSystem") {
		$("#"+id).css("background-color","#4568e2");
		$("#"+id).css("border-right","1px solid #FFFFFF");
		$("#"+id).css("color","#FFFFFF");
	}
}

function mainModuleButtonUp(id, buttonName) {
	if ( ( id == "mainCurrent" || id == "mainHistory" || id == "mainConfig" ) && window.currentModuleID != 0 ) {
		$(".mainModuleButton").removeClass("active");
		$("#"+id).addClass("active");
		$("#"+id).css("border-right","1px solid #FFFFFF");
		$("#"+id).css("color","#AAAAAA");
	} else if (id == "mainHome" || id == "mainSystem") {
		$(".mainModuleButton").removeClass("active");
		$("#"+id).addClass("active");
		$("#"+id).css("border-right","1px solid #FFFFFF");
		$("#"+id).css("color","#AAAAAA");
	}
	
	switchContent("currentID",id, buttonName);
}

/*---------------------------------------------------
 * Card buttons
 ---------------------------------------------------*/
function cardUp(homeCardid, buttonName) {
	var id = homeCardid.replace("homeCard","");
		
	selectedID(id, buttonName);
}

/*-------------------------------------------------------------------------------------
 * Page resize
 ------------------------------------------------------------------------------------*/

/*---------------------------------------------------
 * resize module list
 ---------------------------------------------------*/
function nodeListSize() {
	// set height of node list so it contains no nodes that are cut off height-wise
	var newH = Math.floor((window.innerHeight - 346)/60) * 60;
	if (newH < 60) {
		newH = 60;
	}
	$("#moduleList").css("height", newH);
		
	if (newH >= ( $(".moduleListButton").size() * 60) ) {
		$("#moduleListUp, #moduleListDown").css("opacity", "0.2");
		$("#moduleListUp, #moduleListDown").css("cursor", "default");
	} else {
		$("#moduleListUp, #moduleListDown").css("opacity", "1");
		$("#moduleListUp, #moduleListDown").css("cursor", "pointer");
	}
}

/*---------------------------------------------------
 * resize card list
 ---------------------------------------------------*/
function cardListSize() {
	// set height and width of fav list so that does not clip cards
	// at smallest, should be 1x2 cards
	// at 1080p, should be 2x4 cards

	// width
	if ( $("#homeCardBox").width() < 1000 ) {
		$(".homeCard").css("width","calc(50% - 48px)");
		$(".homeCard").css("margin-left","0px");
		$(".homeCard:nth-child(2n+1)").css("margin-left","14px");
		widthCount = 2;
	} else if ( $("#homeCardBox").width() < 1350 ) {
		$(".homeCard").css("width","calc(33% - 42px)");
		$(".homeCard").css("margin-left","0px");
		$(".homeCard:nth-child(3n+1)").css("margin-left","14px");
		widthCount = 3;
	} else {
		$(".homeCard").css("width","calc(25% - 44px)");
		$(".homeCard").css("margin-left","0px");
		$(".homeCard:nth-child(4n+1)").css("margin-left","14px");
	}
	
	// height
	if ( $("#homeCardBox").height() < 500 ) {
		$(".homeCard").css("height","calc(100% - 36px");
	} else {
		$(".homeCard").css("height","calc(50% - 36px)");
	}
	
	$(".homeCard:first-child").css("margin-left","14px");
}

/*---------------------------------------------------
 * Event listener for when page is resized
 ---------------------------------------------------*/
window.addEventListener('resize', function(event) {
	nodeListSize();
	cardListSize();
});

/*-------------------------------------------------------------------------------------
 * Loading sceen
 ------------------------------------------------------------------------------------*/

/*---------------------------------------------------
 * Shows loading screen elements
 ---------------------------------------------------*/
function showLoading() {
	$("#pageLoading").show();
	$("#overlayLoading").show();
}

/*---------------------------------------------------
 * Hides loading screen elements
 ---------------------------------------------------*/
function hideLoading() {
	$("#pageLoading").fadeOut(300);
	$("#overlayLoading").fadeOut(400);
}