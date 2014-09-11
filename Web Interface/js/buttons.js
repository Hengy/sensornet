$(document).ready(function() {
	
	// mouse over button "rollover"
	$("img.imgBttn").mouseenter(
		function() { this.src = this.src.replace("_off", "_over");
	});
	
	// mouse not over button
	$("img.imgBttn").mouseleave(
		function() {
			this.src = this.src.replace("_over", "_off");
			this.src = this.src.replace("_down", "_off");
	});
	
	// mouse click
	$("img.imgBttn").mousedown(
		function() { this.src = this.src.replace("_over", "_down");
	});
	
	// mouse unclick
	$("img.imgBttn").mouseup(
		function() { this.src = this.src.replace("_down", "_over");
	});
	
	// toggle button. used for fav button
	$("img.tgglBttn").click(
		function() { 
			var state = this.src.indexOf("_fill"); // find button state
			
			// change state
			if (state > 0) {
				this.src = this.src.replace("_fill", "_nofill");
			} else {
				this.src = this.src.replace("_nofill", "_fill");
			}
	});
}); 