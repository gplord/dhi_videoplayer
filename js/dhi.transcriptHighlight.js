// Transcript highlighting functions
// Customized to DHi Video Player environment -- GPL
// --------------------------------------------------- //

var highlightedLine = 0;

function goToLine(num) {
	//alert(num);
	$("#tLine"+highlightedLine).toggleClass('highlighted');
	highlightedLine = num;
	$("#tLine"+highlightedLine).toggleClass('highlighted');
	$("#rolling_transcript").scrollTop(0);
	
	var transcriptOffset = $("#rolling_transcript").offset().top;
	var lineOffset = $("#tLine"+num).offset().top;
	var scrollPos = lineOffset - transcriptOffset;
	
	$("#testOutput").text("Working!");
	
	$("#rolling_transcript").scrollTop(scrollPos);
}

/*$(function() {
	$('a.lightbox').lightBox(); // Select all links with lightbox class
});*/