/*---------------------------------------------------
 * set loading divs hidden, dynamic content divs visible
 * when all dynamic content is done loading
 ---------------------------------------------------*/
function loadDone(loadIntvl) {
	clearInterval(loadIntvl);		// destroy interval so it only happens once
	document.getElementById("listWrapper").style.visibility = "visible";	// set node list visible
	document.getElementById("nodeModule").style.visibility = "visible";		// set node module visible
	document.getElementById("nodeIDWrapper").style.visibility = "visible";	// set node variables visible
	document.getElementById("overlay").style.visibility = "hidden";			// hide loading overlay and box
}

function loadNodeDone(loadNodeIntvl) {
	clearInterval(loadNodeIntvl);		// destroy interval so it only happens once
	document.getElementById("nodeModule").style.visibility = "visible";
	document.getElementById("nodeIDWrapper").style.visibility = "visible";
	document.getElementById("nodeName").style.visibility = "visible";
	document.getElementById("loadNodeModule").style.visibility = "hidden";
}

function loadNodeStart() {
	document.getElementById("nodeModule").style.visibility = "hidden";
	document.getElementById("nodeIDWrapper").style.visibility = "hidden";
	document.getElementById("nodeName").style.visibility = "hidden";
	document.getElementById("loadNodeModule").style.visibility = "visible";
    var loadNodeIntvl;
	loadNodeIntvl = setInterval(function(){ loadNodeDone(loadNodeIntvl); }, 1500);
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
		listBttns[i].addEventListener("click", function(){ loadNodeStart(); });
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
	if (window.innerHeight <= 652) {
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
