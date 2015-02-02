/*---------------------------------------------------
 * Global variables
 ---------------------------------------------------*/
var nodeScreen = 0;	// (0) Home (1) Current (2) History (3) Config

/*---------------------------------------------------
 * set loading divs hidden, dynamic content divs visible
 * when all dynamic content is done loading
 ---------------------------------------------------*/
function loadDone(loadIntvl) {
	clearInterval(loadIntvl);		// destroy interval so it only happens once
	document.getElementById("listWrapper").style.visibility = "visible";	// set node list visible
	document.getElementById("homeScreen").style.visibility = "visible";		// set node module visible
	document.getElementById("overlay").style.visibility = "hidden";			// hide loading overlay and box
	document.getElementById("nodeName").style.visibility = "hidden";		// hide node name for home screen
	document.getElementById("nodeMenuWrapper").style.visibility = "hidden"; // hide menu for home screen
}

/*---------------------------------------------------
 * show elements
 ---------------------------------------------------*/

function showHome() {
	document.getElementById("homeScreen").style.visibility = "visible";
}

function showNodeModule() {
	document.getElementById("nodeModule").style.visibility = "visible";
}

function showNodeModuleHistory() {
	document.getElementById("nodeModuleHistory").style.visibility = "visible";
}

function showNodeModuleConfig() {
	document.getElementById("nodeModuleConfig").style.visibility = "visible";
}

function showNodeName() {
	document.getElementById("nodeName").style.visibility = "visible";
}

function showNodeID() {
	document.getElementById("nodeIDWrapper").style.visibility = "visible";
}

function showNodeModuleLoader() {
	document.getElementById("nodeModuleLoader").style.visibility = "visible";
}

function showNodeList() {
	document.getElementById("listWrapper").style.visibility = "visible";
}

function showPageLoader() {
	document.getElementById("overlay").style.visibility = "visible";
}

function showNodeMenu() {
	document.getElementById("nodeMenuWrapper").style.visibility = "visible";
}

/*---------------------------------------------------
 * hide elements
 ---------------------------------------------------*/

function hideHome() {
	document.getElementById("homeScreen").style.visibility = "hidden";
}

function hideNodeModule() {
	document.getElementById("nodeModule").style.visibility = "hidden";
}

function hideNodeModuleHistory() {
	document.getElementById("nodeModuleHistory").style.visibility = "hidden";
}

function hideNodeModuleConfig() {
	document.getElementById("nodeModuleConfig").style.visibility = "hidden";
}

function hideNodeName() {
	document.getElementById("nodeName").style.visibility = "hidden";
}

function hideNodeID() {
	document.getElementById("nodeIDWrapper").style.visibility = "hidden";
}

function hideNodeModuleLoader() {
	document.getElementById("nodeModuleLoader").style.visibility = "hidden";
}

function hideNodeList() {
	document.getElementById("listWrapper").style.visibility = "hidden";
}

function hidePageLoader() {
	document.getElementById("overlay").style.visibility = "hidden";
}

function hideNodeMenu() {
	document.getElementById("nodeMenuWrapper").style.visibility = "hidden";
}

/*---------------------------------------------------
 * resize list
 ---------------------------------------------------*/
function nodeListSize() {
	// set height of node list so it contains no nodes that are cut off height-wise
	var newH = Math.floor((window.innerHeight - 300)/60) * 60;
	if (newH < 60) {
		newH = 60;
	}
	document.getElementById("listWrapper").style.height = newH + "px";
}

/*---------------------------------------------------
 * set bg color of selected button
 ---------------------------------------------------*/
function colorSelection(bttnGroup,id) {
	if (bttnGroup == "menu") {
		var menuBttns = document.getElementsByClassName("menuButton");
		for (var i=0; i<menuBttns.length;i++) {
			menuBttns[i].style.backgroundColor = "#222222";
		}
		if (id != "home") {
			document.getElementById(id).style.backgroundColor = "#4568e2";
		}
	} else {
		
	}
}

/*---------------------------------------------------
 * change node screen: Current, History or Config
 ---------------------------------------------------*/
function changeScreen(id) {
	//showNodeModuleLoader();
	if (id == "currentBttn") {
		nodeScreen = 1;
		showNodeModule();
		hideNodeModuleHistory();
		hideNodeModuleConfig();
		// load content
		//hideNodeModuleLoader();
	} else if (id == "historyBttn") {
		nodeScreen = 2;
		hideNodeModule();
		showNodeModuleHistory();
		hideNodeModuleConfig();
		// load content
		//hideNodeModuleLoader();
	} else {
		nodeScreen = 3;
		hideNodeModule();
		hideNodeModuleHistory();
		showNodeModuleConfig();
		// load content
		//hideNodeModuleLoader();
	}
	hideHome();
	showNodeName();
	colorSelection("menu",id);
}

/*---------------------------------------------------
 * go to home screen
 ---------------------------------------------------*/
function goHome() {
	nodeScreen = 0;
	hideNodeModule();
	hideNodeModuleHistory();
	hideNodeModuleConfig();
	hideNodeID();
	hideNodeName();
	hideNodeMenu();
	showHome();	
	colorSelection("menu","home");
}

/*---------------------------------------------------
 * execute when page is done loading
 ---------------------------------------------------*/
function pageLoaded() {
	nodeListSize();
	
	// execute after delay
	var loadIntvl;
	loadIntvl = setInterval(function(){ loadDone(loadIntvl); }, 1500);
	
	// add listener to node list buttons
	var listBttns = document.getElementsByClassName("listButton");
	for (var i=0; i<listBttns.length;i++) {
		listBttns[i].addEventListener("click", function(){
			nodeScreen = 1;
			hideHome();
			hideNodeModuleHistory();
			hideNodeModuleConfig();
			showNodeMenu();
			showNodeModule();
			showNodeID();
			showNodeName();
			colorSelection("menu","currentBttn");
		});
	}
}

/*---------------------------------------------------
 * create event listener for when page is resized
 ---------------------------------------------------*/
window.addEventListener('resize', function(event){
	nodeListSize();

	// if window width is too small, scroll page
	if (window.innerWidth <= 1120) {
		document.getElementsByTagName("body")[0].style.overflowX = "scroll";
	} else { // else hide overflow
		document.getElementsByTagName("body")[0].style.overflowX = "hidden";
	}
	// if window height is too small, scroll page
	if (window.innerHeight <= 650) {
		document.getElementsByTagName("body")[0].style.overflowY = "scroll";
	} else { // else hide overflow
		document.getElementsByTagName("body")[0].style.overflowY = "hidden";
	}
});


/*---------------------------------------------------
 * global variables for node list scroll buttons
 ---------------------------------------------------*/
var intvl, newPos;

/*---------------------------------------------------
 * node list scroll up animation
 ---------------------------------------------------*/
function scrollList(dir) {
	if ((document.getElementById("listWrapper").scrollTop - newPos)*dir >= 0) {
		clearInterval(intvl);	// if scrolling is done, destroy interval
	} else {
		document.getElementById("listWrapper").scrollTop += 5*dir;	// move 5px at a time
	}
}

/*---------------------------------------------------
 * if node list scroll up button is pressed
 ---------------------------------------------------*/
function scrollBttnUp() {
	var scrollPos = document.getElementById("listWrapper").scrollTop;	// get current scroll position
	
	clearInterval(intvl);	// if list is scrolling down, stop
	
	if ((scrollPos - 60) < 0) {	// if list is at the top
		document.getElementById("listWrapper").scrollTop = 0;
	} else {	// else scroll
		newPos = (Math.floor(scrollPos / 60) * 60) - 60;	// calc scroll position to next interval of 60px
		intvl = setInterval(function(){ scrollList(-1) }, 10);
	}
	
	document.getElementById("arrowUp").querySelector(".polygon").style.fill="#CCCCCC";
}

/*---------------------------------------------------
 * if node list scroll down button is pressed
 ---------------------------------------------------*/
function scrollBttnDown() {	
	var scrollPos = document.getElementById("listWrapper").scrollTop;	// get current scroll position
	var scrollMax = document.getElementById("listWrapper").style.height;// get max scroll position
	
	clearInterval(intvl);	// if list is scrolling up, stop
	
	if ((scrollPos + 60) > scrollMax) {	// if list is at the top
		document.getElementById("listWrapper").scrollTop = scrollMax;
	} else {	// else scroll
		newPos = (Math.floor(scrollPos / 60) * 60) + 60;	// calc scroll position to next interval of 60px
		intvl = setInterval(function(){ scrollList(1) }, 10);
	}
	
	document.getElementById("arrowDown").querySelector(".polygon").style.fill="#CCCCCC";
}

function mouseOverScrollBttn(id) {
	document.getElementById(id).querySelector(".polygon").style.fill="#CCCCCC";
}

function mouseLeaveScrollBttn(id) {
	document.getElementById(id).querySelector(".polygon").style.fill="#555555";
}

function scrollBttnMouseDown(id) {
	document.getElementById(id).querySelector(".polygon").style.fill="#4568e2";
}
