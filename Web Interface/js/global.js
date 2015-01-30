function pageLoaded() {
	var newH = Math.floor((window.innerHeight - 360)/60) * 60;
	if (newH < 60) {
		newH = 60;
	}
	document.getElementById("listWrapper").style.height = newH + "px";
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
		console.log("top");
	} else {
		console.log("going up");
		newPos = (Math.floor(scrollPos / 60) * 60) - 60;
		dir = -1;
		intvl = setInterval(function(){ scrollUp() }, 10);
	}
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
		console.log("bottom");
	} else {
		console.log("going down");
		newPos = (Math.floor(scrollPos / 60) * 60) + 60;
		dir = 1;
		intvl = setInterval(function(){ scrollDown() }, 10);
	}
}

function mouseOverScrollBttn(id) {
	document.getElementById(id).querySelector(".polygon").style.fill="#CCCCCC";
}

function mouseLeaveScrollBttn(id) {
	document.getElementById(id).querySelector(".polygon").style.fill="#555555";
}
