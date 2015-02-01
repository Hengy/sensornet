function loadDone() {
	var loadIntvl;
	document.getElementById("listWrapper").style.visibility = "visible";
	document.getElementById("nodeModule").style.visibility = "visible";
	document.getElementById("nodeIDWrapper").style.visibility = "visible";
	document.getElementById("overlay").style.visibility = "hidden";
	clearInterval(loadIntvl);
}

function pageLoaded() {
	var newH = Math.floor((window.innerHeight - 360)/60) * 60;
	if (newH < 60) {
		newH = 60;
	}
	document.getElementById("listWrapper").style.height = newH + "px";
	
	loadIntvl = setInterval(function(){ loadDone(); }, 1500);
}

window.addEventListener('resize', function(event){
	var newH = Math.floor((window.innerHeight - 360)/60) * 60;
	if (newH < 60) {
		newH = 60;
	}
	document.getElementById("listWrapper").style.height = newH + "px";
});



var intvl, newPos, dir;

function scrollUp() {
	if ((document.getElementById("listWrapper").scrollTop - newPos) <= 0) {
		clearInterval(intvl);
	} else {
		document.getElementById("listWrapper").scrollTop -= 5;
	}
}

function scrollBttnUp() {
	var scrollPos = document.getElementById("listWrapper").scrollTop;
	
	clearInterval(intvl);
	
	if ((scrollPos - 60) < 0) {
		document.getElementById("listWrapper").scrollTop = 0;
	} else {
		newPos = (Math.floor(scrollPos / 60) * 60) - 60;
		dir = -1;
		intvl = setInterval(function(){ scrollUp() }, 10);
	}
	
	document.getElementById("arrowUp").querySelector(".polygon").style.fill="#CCCCCC";
}

function scrollDown(scrollInterval) {
	if ((document.getElementById("listWrapper").scrollTop - newPos) >= 0) {
		clearInterval(intvl);
	} else {
		document.getElementById("listWrapper").scrollTop += 5;
	}
}

function scrollBttnDown() {	
	var scrollPos = document.getElementById("listWrapper").scrollTop;
	var scrollMax = document.getElementById("listWrapper").style.height;
	
	clearInterval(intvl);
	
	if ((scrollPos + 60) > scrollMax) {
		document.getElementById("listWrapper").scrollTop = scrollMax;
	} else {
		newPos = (Math.floor(scrollPos / 60) * 60) + 60;
		dir = 1;
		intvl = setInterval(function(){ scrollDown() }, 10);
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
