// Loads XSLT to process transcript XML data

function loadXMLDoc(dname) {
		if (window.XMLHttpRequest) {
			xhttp=new XMLHttpRequest();
		} else {
			xhttp=new ActiveXObject("Microsoft.XMLHTTP");
		}
		xhttp.open("GET",dname,false);
		xhttp.send("");
		return xhttp.responseXML;
	}

	function displayResult(transcriptXML) {
		var path = "http://cjf.dhinitiative.org/sites/all/modules/dhi_videoplayer/";
	
		// Link to testdata transcript in proper format -- uncomment to test
		// xml=loadXMLDoc(path+"testdata/"+transcriptXML);
		xml = loadXMLDoc(transcriptXML);
		
		xsl=loadXMLDoc(path+"js/transcript.xsl");
		
		// code for IE
		if (window.ActiveXObject) {
			ex=xml.transformNode(xsl);
			document.getElementById("rolling_transcript").innerHTML=ex;
		}

		// code for Mozilla, Firefox, Opera, etc.
		else if (document.implementation && document.implementation.createDocument)	{
			xsltProcessor=new XSLTProcessor();
			xsltProcessor.importStylesheet(xsl);
			resultDocument = xsltProcessor.transformToFragment(xml,document);
			document.getElementById("rolling_transcript").appendChild(resultDocument);
		}

	// After parsing XML, provide click function for each transcript lineGroup
	$(".transcriptLineGroup").click(function() {
		
		var timeInMs = this.getAttribute("data-time-in-ms");		
		var video = document.getElementsByTagName("video")[0];
		video.currentTime = timeInMs; 
		video.play();
		//alert(timeInMs + " sec");
	});

}