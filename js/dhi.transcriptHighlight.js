// Transcript highlighting functions
// Customized to DHi Video Player environment -- GPL
// --------------------------------------------------- //

//alert ("test222 dhi.transcriptHighlight.js");
var highlightedLine = 0;

function goToLine(num) {
	alert(num);
	$("#tLine"+highlightedLine).toggleClass('highlighted');
	highlightedLine = num;
	$("#tLine"+highlightedLine).toggleClass('highlighted');
	$("#rolling_transcript").scrollTop(0);
	var pos = $("#tLine"+num).position().top;
	$("#rolling_transcript").scrollTop(pos);
}

/*$(function() {
	$('a.lightbox').lightBox(); // Select all links with lightbox class
});*/