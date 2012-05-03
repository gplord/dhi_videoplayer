<?php

class DHiVideoPlayer {

  function __construct($pid = '') {
    /*module_load_include('inc', 'fedora_repository', 'api/fedora_item');
    module_load_include('inc', 'fedora_repository', 'CollectionClass');*/

   if (!empty($pid)) {
    	  $this->pid = $pid;
      	//$this->item = new Fedora_Item($pid);
    }

    module_load_include('object.inc', 'islandora_fedora_api');

  }

  function showVideoPlayer() {
    global $base_url;
    $path = drupal_get_path('module', 'dhi_videoplayer');
    $fullPath = $path . '/';

	$html = "";

	// Construct Sparql Query
	$query_file_name = drupal_get_path('module', 'dhi_videoplayer') . '/lowres_query.txt';
	//$html .= "<p>".$query_file_name."</p>\n";
	$query_file_handle = fopen($query_file_name, "r");
	$query = fread($query_file_handle, filesize($query_file_name));
	fclose($query_file_handle);
	$limit = -1;
	$offset = 0 ;
	$type = 'sparql';
	$query_url = variable_get('fedora_repository_url', 'http://localhost:8080/fedora/risearch');
	
	// Inject collection value into the query
	$query = preg_replace('/\$collection_object/', '<info:fedora/' . $this->pid . '>', $query);
	
	// Run Sparql Query
	$query_url .= "?type=tuples&flush=TRUE&format=Sparql" . (($limit > 0)?("&limit=$limit"):("")) . "&offset=$offset&lang=$type&query=" . urlencode($query);
	$query_return_string = do_curl($query_url);
	if ($query_return_string) {
		$query_return_dom = DOMDocument::loadXML($query_return_string);
	}
	
	// Format query results for use (xml->array)
	$result_nodes = $query_return_dom->getElementsByTagName('result');
	$result_nodes_length = $result_nodes->length;
	
	
	// Load Javascript / jQuery scripts
	// ------------------------------------------------------------------------------------------ //
	
	drupal_add_js(drupal_get_path('module', 'dhi_videoplayer') . '/js/jquery-1.5.2.min.js');
	drupal_add_js(drupal_get_path('module', 'dhi_videoplayer') . '/js/jquery.lightbox-0.5.min.js');
	
	drupal_add_js(drupal_get_path('module', 'dhi_videoplayer') . '/js/video.js');
	drupal_add_js(drupal_get_path('module', 'dhi_videoplayer') . '/js/popcorn/popcorn.js');
	drupal_add_js(drupal_get_path('module', 'dhi_videoplayer') . '/js/popcorn/config.js');
	drupal_add_js(drupal_get_path('module', 'dhi_videoplayer') . '/js/popcorn/plugins/transcript/popcorn.transcript.js');
	drupal_add_js(drupal_get_path('module', 'dhi_videoplayer') . '/js/parsers/parserXML/popcorn.parseXML.js');
	
	drupal_add_js(drupal_get_path('module', 'dhi_videoplayer') . '/js/dhi.xsltParseTranscript.js');
	drupal_add_js(drupal_get_path('module', 'dhi_videoplayer') . '/js/dhi.transcriptHighlight.js');
	
	drupal_add_css(drupal_get_path('module', 'dhi_videoplayer') . '/css/jquery.lightbox-0.5.css');
	
	
	// DEBUG: Echo out PID, Query, # of Query Results
	//$html .= "<p><strong>PID:</strong> $this->pid</p>\n";
	//$html .= "<p><strong>Query:</strong><br /><pre>".$query."</pre></p>\n";
	//$html .= "<p><strong>Results: </strong> {$result_nodes_length} Results</p>\n";
	

	if ($result_nodes_length > 0) {
		for ($i=0; $i < $result_nodes_length; $i++) {
			$result_node = $result_nodes->item($i);
			$result_values = $result_node->childNodes;

			$label_element = $result_node->getElementsByTagName('label')->item(0);
			$clip_index_element = $result_node->getElementsByTagName('clip_index')->item(0);
			$subbed_clip_element = $result_node->getElementsByTagName('subbed_clip')->item(0);
			$transcript_element = $result_node->getElementsByTagName('transcript')->item(0);

			$subbed_clip_object = $subbed_clip_element->getAttribute('uri');
			$subbed_clip_object = substr($subbed_clip_object, 12, strlen($subbed_clip_object));//need to extract the pid

			$transcript_object = $transcript_element->getAttribute('uri');
			$transcript_object = substr($transcript_object, 12, strlen($transcript_object)); // extract PID

			$subbed_clip_data[$subbed_clip_object] = array();

			if ($label_element) {
				$label = $label_element->nodeValue;
				if ($label) {	//the label may not be populated
					$subbed_clip_data[$subbed_clip_object]['label'] = $label;
				}
			}

			if ($clip_index_element) {
				$clip_index = $clip_index_element->nodeValue;
				if ($clip_index) {
					$subbed_clip_data[$subbed_clip_object]['clip_index'] = $clip_index;
				}
			}

			if ($transcript_object) {
				$subbed_clip_data[$subbed_clip_object]['transcript'] = $transcript_object;
			}

			// DEBUG: Echo out Query Results
			//$html .= "<p>Result {$i}<br />
			//<strong>Label:</strong> {$label}<br />
			//<strong>Clip Index:</strong> {$clip_index}<br />
			//<strong>URI:</strong> {$subbed_clip_object}<br />
			//<strong>Transcript:</strong> {$transcript_object}</p>\n";

		}

		// Create video player div using Subbed Clip PID
		// TO-DO: Replace hard-coded PID with Subbed Clip PID (derived from above)
		$subbed_clip_object = "HamiltonJapaneseComparativeFilm:15";
		$fedora_base_url = variable_get('fedora_base_url', 'http://localhost:8080/fedora');
		$access_url = $fedora_base_url . "/objects/" . $subbed_clip_object . "/datastreams/LOWRES/content";
		
		// TO-DO: Add this block code/style to CJF theme's CSS files
		//$html .= "<div><video id=\"video\" class=\"video-js\" width=\"600\" height=\"450\" controls=\"controls\" preload=\"auto\" data-timeline-sources=\"datatest.xml\"><source src=\"$access_url\"></div>\n";
		$html .= "<div><video id=\"video\" class=\"video-js\" width=\"600\" height=\"450\" controls=\"controls\" preload=\"auto\" data-timeline-sources=\"".$base_url."/fedora/repository/HamiltonJapaneseComparativeFilm:74/TimeSyncedTranscript/POPCORN\"><source src=\"$access_url\"></div>\n";
		

		module_load_include('object.inc', 'islandora_fedora_api'); //for getting an object
		$object= new FedoraObject($subbed_clip_object);
		$object_datastreams=$object->get_datastreams();
		
		/*if (!isset($object_datastreams['lowres'])) {
			$html .= "<p>Found Datastream \"LOWRES\"</p>\n";
		}*/
		
		// Transcript Block
		// TO-DO: Move transcript CSS rules from theme into module CSS
		$html .= "<div id=\"rolling_transcript\"></div>\n\n";
		
		// TO-DO: Replace hard-coded PID here with transcript PID (derived from above)
		$transcript_pid = "HamiltonJapaneseComparativeFilm:74";
		drupal_add_js('jQuery(document).ready(function () { displayResult("http://cjf.dhinitiative.org/fedora/repository/'.$transcript_pid.'/TimeSyncedTranscript/POPCORN") });', 'inline');
	}
	
	
	/*
	// Placeholder Metadata Datastream access:
	// TO-DO: Replace this expensive method of deriving this info with something simpler...
	$xml = simplexml_load_file('http://cjf.dhinitiative.org:8080/fedora/objects/'.$subbed_clip_object.'/datastreams/MODS/content');
	
	// Placeholder Table for Metadata
	// TO-DO: Refine this XML crawler to return all relevant metadata
	// For now: finds anything with a Display Label, displays that as a <th>, and its contents as a <td>
	$html .= "<h4>Movie Metadata</h4>\n";
	$html .= "<table id=\"metadata-table\" cellspacing=\"0\" cellpadding=\"0\">\n";
	foreach($xml->children() as $item) {
	
		if ($item->attributes()->displayLabel) {
			$html .= "<tr>\n";
			$html .= "<th>".$item->attributes()->displayLabel . "</th>\n";
			$childrenArray = array();
			
			foreach ($item->children() as $child) {
			
				foreach ($child->children() as $grandchild) {
					$childrenArray[] = $grandchild;
				}
			
				if (trim($child) != "") { $childrenArray[] = $child; }
			}
			
			$children = implode(", ", $childrenArray);
			$html .= "<td>".$children."</td>\n";
		}
		
	}
	$html .= "</table>\n";
	*/
	
	
	
	// BEGIN Still Image Section ---------------------------------------------------------------- //
	
	/*
	// Construct Sparql Query
	$query_file_name = drupal_get_path('module', 'dhi_videoplayer') . '/stills_query.txt';
	$query_file_handle = fopen($query_file_name, "r");
	$query = fread($query_file_handle, filesize($query_file_name));
	fclose($query_file_handle);
	$limit = -1;
	$offset = 0 ;
	$type = 'sparql';
	$query_url = variable_get('fedora_repository_url', 'http://localhost:8080/fedora/risearch');
	
	// Inject collection value into the query
	$query = preg_replace('/\$collection_object/', '<info:fedora/' . $this->pid . '>', $query);
	
	// Run Sparql Query
	$query_url .= "?type=tuples&flush=TRUE&format=Sparql" . (($limit > 0)?("&limit=$limit"):("")) . "&offset=$offset&lang=$type&query=" . urlencode($query);
	$query_return_string = do_curl($query_url);
	if ($query_return_string) {
		$query_return_dom = DOMDocument::loadXML($query_return_string);
	}
	
	// Format query results for use (xml->array)
	$result_nodes = $query_return_dom->getElementsByTagName('result');
	$result_nodes_length = $result_nodes->length;
	
	if ($result_nodes_length > 0) {

		// TO-DO: Add this block code/style to CJF theme's CSS files
		
		$html .= "<script type=\"text/javascript\"> $(function() { $('a.still_force_thumbnail').lightBox(); }); </script>\n";
		
		$html .= "<h4>Related Still Images</h4>\n";
		$html .= "<div id=\"still_images\" style=\"border: 1px solid #ccc; width: 600px; padding: .5em 1em; margin: 1em 0\">\n";
	
		for ($i=0; $i < $result_nodes_length; $i++) {
			$result_node = $result_nodes->item($i);
			$result_values = $result_node->childNodes;

			$label_element = $result_node->getElementsByTagName('label')->item(0);
			$still_element = $result_node->getElementsByTagName('still')->item(0);

			$still_object = $still_element->getAttribute('uri');
			$still_object = substr($still_object, 12, strlen($still_object)); // extract PID

			if ($label_element) {
				$label = $label_element->nodeValue;
				if ($label) {	//the label may not be populated
					$subbed_clip_data[$subbed_clip_object]['label'] = $label;
				}
			}

			// DEBUG: Echo out Query Results
			
			//$html .= "<p>Result {$i}<br />
			//<strong>Label:</strong> {$label}<br />
			//<strong>Still:</strong> {$still_object}</p>\n";
			
			$html .= "<a class='still_force_thumbnail' href='".$base_url.":8080/fedora/objects/".$still_object."/datastreams/PNG/content'><img src='".$base_url.":8080/fedora/objects/".$still_object."/datastreams/PNG/content' alt='Still Image' /></a>\n";
			

		}
		
		$html .= "<div class=\"clear\"></div>\n";
		$html .= "</div>\n\n";
		
	}
	*/
	// END Still Image Section ------------------------------------------------------------------ //

	return $html;

  }


/* DHi Video Player Tabs  -- _gpl */

function showVideoStreams() {

	// Show the video player block
	$viewer=$this->showVideoPlayer();
	$metadata=$this->showMetadata();
	
	// Set up tabs
	$tabs = array();
	$tabs = array('#type' => 'tabset');
	
	//get metadata info
	//$metadata = $this->getMetaData();
	
	//populate tabs
	/*
	$tabs['Description'] = array(
	'#type' => 'tabpage',
	'#title' => 'Description',
	'#content' => $metadata,
	'#weight' => 2
	);
	*/
	
	$tabs['View'] = array(
		'#type' => 'tabpage',
		'#title' => 'View',
		'#content' => $viewer,
		'#weight' => -1,
		'#selected' => true
	);
	$tabs['Metadata'] = array(
		'#type' => 'tabpage',
		'#title' => 'Metadata',
		'#content' => $metadata,
		'#weight' => 0
	);
	
	//return the tabs for display
	return $tabs;
}

function showClipTabs() {

	$viewer = $this->showVideoClip();
	$metadata = $this->showMetadata();
	
	$clip_tabs = array();
	$clip_tabs = array('#type' => 'tabset');
	
	$clip_tabs['Test'] = array(
		'#type' => 'tabpage',
		'#title' => 'View',
		'#content' => $viewer,
		'#weight' => -1,
		'#selected' => true
	);
	$clip_tabs['Metadata'] = array(
		'#type' => 'tabpage',
		'#title' => 'Metadata',
		'#content' => $metadata,
		'#weight' => 0
	);
	
	return $clip_tabs;

}

function showMetadata() {

	$pid = $this->pid;
	
	// Placeholder Metadata Datastream access:
	// TO-DO: Replace this expensive method of deriving this info with something simpler...
	$xml = simplexml_load_file('http://cjf.dhinitiative.org:8080/fedora/objects/'.$pid.'/datastreams/MODS/content');

	// Placeholder Table for Metadata
	// TO-DO: Refine this XML crawler to return all relevant metadata
	// For now: finds anything with a Display Label, displays that as a <th>, and its contents as a <td>
	$html .= "<h4>Object Metadata</h4>\n";
	$html .= "<table id=\"metadata-table\" cellspacing=\"0\" cellpadding=\"0\">\n";
	foreach($xml->children() as $item) {
	
		if ($item->attributes()->displayLabel) {
			$html .= "<tr>\n";
			$html .= "<th>".$item->attributes()->displayLabel . "</th>\n";
			$childrenArray = array();
			$attributes = array();
			
			foreach ($item->children() as $child) {
			
				foreach ($child->children() as $grandchild) {
					if ($grandchild->getName() == "roleTerm") {
						$attributes = $grandchild->attributes();
						if ($attributes[type] == "text") {						
							$childrenArray[] = $grandchild;
						}
					} else {
						$childrenArray[] = $grandchild;
					}
				}
			
				if (trim($child) != "") {
					if ($child->getName() == "languageTerm") {
						$attributes = $child->attributes();
						if ($attributes[type] == "text") {						
							$childrenArray[] = $child;
						}
					} else {
						$childrenArray[] = $child;
					}
				}
			}
			if ($item->count() < 1) {
				$childrenArray[] = $item;
			}
			
			$children = implode(", ", $childrenArray);
			$html .= "<td>".$children."</td>\n";
			
			// DEBUG: Show number of children for matching items as third column of table
			//$html .= "<td>".($item->count())."</td>\n";
			
		}
		
	}
	$html .= "</table>\n";
	
	return $html;

}


function showVideoClip() {

	global $base_url;

    $path = drupal_get_path('module', 'dhi_videoplayer');
    $fullPath = $path . '/';

	// Load Javascript / jQuery scripts
	// ------------------------------------------------------------------------------------------ //
	
	drupal_add_js(drupal_get_path('module', 'dhi_videoplayer') . '/js/jquery-1.5.2.min.js');
	drupal_add_js(drupal_get_path('module', 'dhi_videoplayer') . '/js/jquery.lightbox-0.5.min.js');
	
	drupal_add_js(drupal_get_path('module', 'dhi_videoplayer') . '/js/video.js');
	drupal_add_js(drupal_get_path('module', 'dhi_videoplayer') . '/js/popcorn/popcorn.js');
	drupal_add_js(drupal_get_path('module', 'dhi_videoplayer') . '/js/popcorn/plugins/transcript/popcorn.transcript.js');
	drupal_add_js(drupal_get_path('module', 'dhi_videoplayer') . '/js/parsers/parserXML/popcorn.parseXML.js');
	drupal_add_js(drupal_get_path('module', 'dhi_videoplayer') . '/js/popcorn/config.js');
	
	drupal_add_js(drupal_get_path('module', 'dhi_videoplayer') . '/js/dhi.xsltParseTranscript.js');
	drupal_add_js(drupal_get_path('module', 'dhi_videoplayer') . '/js/dhi.transcriptHighlight.js');
		
	drupal_add_css(drupal_get_path('module', 'dhi_videoplayer') . '/css/jquery.lightbox-0.5.css');
		
	// Create video player div using Subbed Clip PID
	// TO-DO: Replace hard-coded PID with Subbed Clip PID (derived from above)
	$subbed_clip_object = $this->pid;
	$fedora_base_url = variable_get('fedora_base_url', 'http://localhost:8080/fedora');
	$access_url = $fedora_base_url . "/objects/" . $subbed_clip_object . "/datastreams/LOWRES/content";	
		
	// Construct Sparql Query
	$query_file_name = drupal_get_path('module', 'dhi_videoplayer') . '/clip_transcript_query.txt';
	$query_file_handle = fopen($query_file_name, "r");
	$query = fread($query_file_handle, filesize($query_file_name));
	fclose($query_file_handle);
	$limit = -1;
	$offset = 0 ;
	$type = 'sparql';
	$query_url = variable_get('fedora_repository_url', 'http://localhost:8080/fedora/risearch');
	
	// Inject collection value into the query
	$query = preg_replace('/\$collection_object/', '<info:fedora/' . $this->pid . '>', $query);
	
	// Run Sparql Query
	$query_url .= "?type=tuples&flush=TRUE&format=Sparql" . (($limit > 0)?("&limit=$limit"):("")) . "&offset=$offset&lang=$type&query=" . urlencode($query);
	$query_return_string = do_curl($query_url);
	if ($query_return_string) {
		$query_return_dom = DOMDocument::loadXML($query_return_string);
	}
	
	// Format query results for use (xml->array)
	$result_nodes = $query_return_dom->getElementsByTagName('result');
	$result_nodes_length = $result_nodes->length;
	
	
	// All transcript logic placed inside conditional statement, to make sure we got results above
	if ($result_nodes_length > 0) {
		for ($i=0; $i < $result_nodes_length; $i++) {
			$result_node = $result_nodes->item($i);
			$result_values = $result_node->childNodes;

			$label_element = $result_node->getElementsByTagName('label')->item(0);
			$transcript_element = $result_node->getElementsByTagName('transcript')->item(0);
			$movie_element = $result_node->getElementsByTagName('movie')->item(0);

			$transcript_object = $transcript_element->getAttribute('uri');
			$transcript_object = substr($transcript_object, 12, strlen($transcript_object)); // extract PID
			
			$movie_object = $movie_element->getAttribute('uri');
			$movie_object = substr($movie_object, 12, strlen($movie_object)); // extract PID

			if ($label_element) {
				$label = $label_element->nodeValue;
				if ($label) {	//the label may not be populated
					$subbed_clip_data[$subbed_clip_object]['label'] = $label;
				}
			}

			if ($transcript_object) {
				$subbed_clip_data[$subbed_clip_object]['transcript'] = $transcript_object;
			}

			// DEBUG: Echo out Query Results
			/*
			$html .= "<p>Result {$i}<br />
			<strong>Label:</strong> {$label}<br />
			<strong>Transcript:</strong> {$transcript_object}<br />
			<strong>Movie:</strong> {$movie_object}</p>\n";
			*/

		}
			
		// TO-DO: Add this block code/style to CJF theme's CSS files
		$html .= "<div><video id=\"video\" class=\"video-js\" width=\"600\" height=\"450\" controls=\"controls\" preload=\"auto\" data-timeline-sources=\"".$base_url."/fedora/repository/".$transcript_object."/TimeSyncedTranscript/POPCORN\"><source src=\"$access_url\"></div>\n";
		
		// Transcript Block
		// TO-DO: Move transcript CSS rules from theme into module CSS
		$html .= "<div id=\"rolling_transcript\"></div>\n\n";
		
		drupal_add_js('jQuery(document).ready(function () { displayResult("'.$base_url.'/fedora/repository/'.$transcript_object.'/TimeSyncedTranscript/POPCORN"); });', 'inline');
	
		$html .= "<hr />\n";

		$html .= "<h4>Transcript PDF</h4>\n";
		$transcript_url = $fedora_base_url . "/objects/" . $transcript_object . "/datastreams/PDF/content";	
		$html .= "<p><a class=\"button_link\" href=\"".$transcript_url."\">Download Transcript (PDF) &raquo;</a>\n";

		$html .= "<hr />\n";

		// Extract parent movie's title from Fedora movie object's MODS data (XML)
		// TO-DO: Replace this expensive method of deriving this info with something simpler...		
		$html .= "<h4>Clip Of</h4>\n";
		$xml = simplexml_load_file($fedora_base_url.'/objects/'.$movie_object.'/datastreams/MODS/content');
		$xml->registerXPathNamespace('c', 'http://www.loc.gov/mods/v3');
		$result = $xml->xpath('//c:titleInfo[@lang="eng"]/c:title');
		$html .= '<a class="button_link" href="'.$base_url . '/fedora/repository/'.$movie_object.'">'.$result[0]->{0}." &raquo;</a>\n";
		
	} else {
		
		// Show the video player WITHOUT the transcript data-timeline-sources -- no popcorn functionality
		$html .= "<div><video id=\"video\" class=\"video-js\" width=\"600\" height=\"450\" controls=\"controls\" preload=\"auto\"><source src=\"$access_url\"></div>\n";
	
	}
	
	
	
	
	
	
	
	// ------------------------------------------------------------------ //
	// English Transcript Only
	
	
	
	// Construct Sparql Query
	$query_file_name = drupal_get_path('module', 'dhi_videoplayer') . '/clip_transcript_query_eng.txt';
	$query_file_handle = fopen($query_file_name, "r");
	$query = fread($query_file_handle, filesize($query_file_name));
	fclose($query_file_handle);
	$limit = -1;
	$offset = 0 ;
	$type = 'sparql';
	$query_url = variable_get('fedora_repository_url', 'http://localhost:8080/fedora/risearch');
	
	// Inject collection value into the query
	$query = preg_replace('/\$collection_object/', '<info:fedora/' . $this->pid . '>', $query);
	
	// Run Sparql Query
	$query_url .= "?type=tuples&flush=TRUE&format=Sparql" . (($limit > 0)?("&limit=$limit"):("")) . "&offset=$offset&lang=$type&query=" . urlencode($query);
	$query_return_string = do_curl($query_url);
	if ($query_return_string) {
		$query_return_dom = DOMDocument::loadXML($query_return_string);
	}
	
	// Format query results for use (xml->array)
	$result_nodes = $query_return_dom->getElementsByTagName('result');
	$result_nodes_length = $result_nodes->length;
	
	
	// All transcript logic placed inside conditional statement, to make sure we got results above
	if ($result_nodes_length > 0) {

		for ($i=0; $i < $result_nodes_length; $i++) {
			$result_node = $result_nodes->item($i);
			$result_values = $result_node->childNodes;

			$label_element = $result_node->getElementsByTagName('label')->item(0);
			$transcript_element = $result_node->getElementsByTagName('transcript')->item(0);

			$transcript_object = $transcript_element->getAttribute('uri');
			$transcript_object = substr($transcript_object, 12, strlen($transcript_object)); // extract PID

			if ($label_element) {
				$label = $label_element->nodeValue;
				if ($label) {	//the label may not be populated
					$subbed_clip_data[$subbed_clip_object]['label'] = $label;
				}
			}

			if ($transcript_object) {
				$subbed_clip_data[$subbed_clip_object]['transcript'] = $transcript_object;
			}

		}

		$html .= "<hr />\n";
		
		$html .= "<h4>Single-Language Transcripts</h4>\n";
		$transcript_url = $fedora_base_url . "/objects/" . $transcript_object . "/datastreams/PDF/content";	
		$html .= "<p><a class=\"button_link\" href=\"".$transcript_url."\">English Transcript (PDF) &raquo;</a>\n";

	}
	
	// ------------------------------------------------------------------ //
	// Japanese Transcript Only
	
	
	
	// Construct Sparql Query
	$query_file_name = drupal_get_path('module', 'dhi_videoplayer') . '/clip_transcript_query_jpn.txt';
	$query_file_handle = fopen($query_file_name, "r");
	$query = fread($query_file_handle, filesize($query_file_name));
	fclose($query_file_handle);
	$limit = -1;
	$offset = 0 ;
	$type = 'sparql';
	$query_url = variable_get('fedora_repository_url', 'http://localhost:8080/fedora/risearch');
	
	// Inject collection value into the query
	$query = preg_replace('/\$collection_object/', '<info:fedora/' . $this->pid . '>', $query);
	
	// Run Sparql Query
	$query_url .= "?type=tuples&flush=TRUE&format=Sparql" . (($limit > 0)?("&limit=$limit"):("")) . "&offset=$offset&lang=$type&query=" . urlencode($query);
	$query_return_string = do_curl($query_url);
	if ($query_return_string) {
		$query_return_dom = DOMDocument::loadXML($query_return_string);
	}
	
	// Format query results for use (xml->array)
	$result_nodes = $query_return_dom->getElementsByTagName('result');
	$result_nodes_length = $result_nodes->length;
	
	
	// All transcript logic placed inside conditional statement, to make sure we got results above
	if ($result_nodes_length > 0) {

		for ($i=0; $i < $result_nodes_length; $i++) {
			$result_node = $result_nodes->item($i);
			$result_values = $result_node->childNodes;

			$label_element = $result_node->getElementsByTagName('label')->item(0);
			$transcript_element = $result_node->getElementsByTagName('transcript')->item(0);

			$transcript_object = $transcript_element->getAttribute('uri');
			$transcript_object = substr($transcript_object, 12, strlen($transcript_object)); // extract PID

			if ($label_element) {
				$label = $label_element->nodeValue;
				if ($label) {	//the label may not be populated
					$subbed_clip_data[$subbed_clip_object]['label'] = $label;
				}
			}

			if ($transcript_object) {
				$subbed_clip_data[$subbed_clip_object]['transcript'] = $transcript_object;
			}

		}
		
		$transcript_url = $fedora_base_url . "/objects/" . $transcript_object . "/datastreams/PDF/content";	
		$html .= "<p><a class=\"button_link\" href=\"".$transcript_url."\">Japanese Transcript (PDF) &raquo;</a>\n";

	}
	
	
	// ----------------------------------------------------------------- //
	
	
	
	
	/*
	// Placeholder Metadata Datastream access:
	// TO-DO: Replace this expensive method of deriving this info with something simpler...
	$xml = simplexml_load_file('http://cjf.dhinitiative.org:8080/fedora/objects/'.$subbed_clip_object.'/datastreams/MODS/content');
	
	$html .= "<hr />\n";
	
	// Placeholder Table for Metadata
	// TO-DO: Refine this XML crawler to return all relevant metadata
	// For now: finds anything with a Display Label, displays that as a <th>, and its contents as a <td>
	$html .= "<h4>Video Clip Metadata</h4>\n";
	$html .= "<table id=\"metadata-table\" cellspacing=\"0\" cellpadding=\"0\">\n";
	foreach($xml->children() as $item) {
	
		if ($item->attributes()->displayLabel) {
			$html .= "<tr>\n";
			$html .= "<th>".$item->attributes()->displayLabel . "</th>\n";
			$childrenArray = array();
			$attributes = array();
			
			foreach ($item->children() as $child) {
			
				foreach ($child->children() as $grandchild) {
					if ($grandchild->getName() == "roleTerm") {
						$attributes = $grandchild->attributes();
						if ($attributes[type] == "text") {						
							$childrenArray[] = $grandchild;
						}
					} else {
						$childrenArray[] = $grandchild;
					}
				}
			
				if (trim($child) != "") {
					if ($child->getName() == "languageTerm") {
						$attributes = $child->attributes();
						if ($attributes[type] == "text") {						
							$childrenArray[] = $child;
						}
					} else {
						$childrenArray[] = $child;
					}
				}
			}
			if ($item->count() < 1) {
				$childrenArray[] = $item;
			}
			
			$children = implode(", ", $childrenArray);
			$html .= "<td>".$children."</td>\n";
			
			// DEBUG: Show number of children for matching items as third column of table
			//$html .= "<td>".($item->count())."</td>\n";
			
		}
		
	}
	$html .= "</table>\n";
	
	*/
	
	
	
	
	
	// BEGIN Still Image Section ---------------------------------------------------------------- //
	
	///*
	// Construct Sparql Query
	$query_file_name = drupal_get_path('module', 'dhi_videoplayer') . '/stills_query.txt';
	$query_file_handle = fopen($query_file_name, "r");
	$query = fread($query_file_handle, filesize($query_file_name));
	fclose($query_file_handle);
	$limit = -1;
	$offset = 0 ;
	$type = 'sparql';
	$query_url = variable_get('fedora_repository_url', 'http://localhost:8080/fedora/risearch');
	
	// Inject collection value into the query
	$query = preg_replace('/\$collection_object/', '<info:fedora/' . $this->pid . '>', $query);
	
	// Run Sparql Query
	$query_url .= "?type=tuples&flush=TRUE&format=Sparql" . (($limit > 0)?("&limit=$limit"):("")) . "&offset=$offset&lang=$type&query=" . urlencode($query);
	$query_return_string = do_curl($query_url);
	if ($query_return_string) {
		$query_return_dom = DOMDocument::loadXML($query_return_string);
	}
	
	// Format query results for use (xml->array)
	$result_nodes = $query_return_dom->getElementsByTagName('result');
	$result_nodes_length = $result_nodes->length;
	
	if ($result_nodes_length > 0) {

		// TO-DO: Add this block code/style to CJF theme's CSS files
		
		$html .= "<script type=\"text/javascript\"> $(function() { $('a.still_force_thumbnail').lightBox(); }); </script>\n";
		
		$html .= "<h4>Related Still Images</h4>\n";
		$html .= "<div id=\"still_images\" style=\"border: 1px solid #ccc; width: 600px; padding: .5em 1em; margin: 1em 0\">\n";
	
		for ($i=0; $i < $result_nodes_length; $i++) {
			$result_node = $result_nodes->item($i);
			$result_values = $result_node->childNodes;

			$label_element = $result_node->getElementsByTagName('label')->item(0);
			$still_element = $result_node->getElementsByTagName('still')->item(0);

			$still_object = $still_element->getAttribute('uri');
			$still_object = substr($still_object, 12, strlen($still_object)); // extract PID

			if ($label_element) {
				$label = $label_element->nodeValue;
				if ($label) {	//the label may not be populated
					$subbed_clip_data[$subbed_clip_object]['label'] = $label;
				}
			}

			// DEBUG: Echo out Query Results
			
			//$html .= "<p>Result {$i}<br />
			//<strong>Label:</strong> {$label}<br />
			//<strong>Still:</strong> {$still_object}</p>\n";
			
			$html .= "<a class='still_force_thumbnail' href='".$base_url.":8080/fedora/objects/".$still_object."/datastreams/PNG/content'><img src='".$base_url.":8080/fedora/objects/".$still_object."/datastreams/PNG/content' alt='Still Image' /></a>\n";
			

		}
		
		$html .= "<div class=\"clear\"></div>\n";
		$html .= "</div>\n\n";
		
	}
	//*/
	// END Still Image Section ------------------------------------------------------------------ //
	
	
	
	return $html;

}
  



function showStillImageTabs() {

	$viewer = $this->showStillImage();
	$metadata = $this->showMetadata();
	
	$image_tabs = array();
	$image_tabs = array('#type' => 'tabset');
	
	$image_tabs['Test'] = array(
		'#type' => 'tabpage',
		'#title' => 'View',
		'#content' => $viewer,
		'#weight' => -1,
		'#selected' => true
	);
	$image_tabs['Metadata'] = array(
		'#type' => 'tabpage',
		'#title' => 'Metadata',
		'#content' => $metadata,
		'#weight' => 0
	);
	
	return $image_tabs;

}
  
  
function showStillImage() {

	global $base_url;

    $path = drupal_get_path('module', 'dhi_videoplayer');
    $fullPath = $path . '/';
		
	// Create video player div using Subbed Clip PID
	// TO-DO: Replace hard-coded PID with Subbed Clip PID (derived from above)
	$pid = $this->pid;
	$fedora_base_url = variable_get('fedora_base_url', 'http://localhost:8080/fedora');
	$access_url = $fedora_base_url . "/objects/" . $pid . "/datastreams/PNG/content";	
		
	$html = "<h4>Still Image</h4>\n";	
	$html .= "<img class=\"stillImage_full\" src=\"".$access_url."\" alt=\"Still Image\" />\n";	
	$html .= "<p><a href=\"".$access_url."\">View Full Size &raquo;</a></p>\n";
	
	return $html;

}
  
  
  
  
  
 /**
   * @author: William Panting
   *
   * @date: 19/05/2011
   *
   * @return output: the html of data to display
   *
   * Small modification of UoM's exif plugin
   */
  function getMetaData()
  {
    $output='';
    $MODS = $this->item->get_datastream_dissemination('MODS');
    if (trim($MODS) != '')
    {
      $MODSDom = DOMDocument::loadXML($this->item->get_datastream_dissemination('MODS'));
      if ($MODSDom != NULL)
      {
        $description = $MODSDom->getElementsByTagName('*');
        if ($description->length > 0)
        {
          $description=$description->item(0);
          $output .= '<div class="fedora_technical_metadata"><ul>';
          for ($i=0;$i<$description->childNodes->length;$i++)
          {
            $name=$description->childNodes->item($i)->nodeName;
            $value=$description->childNodes->item($i)->nodeValue;
            if ($name != '#text'  && !preg_match('/^System\:.*$/',$name) && trim($value) != '')
            {
              list($type,$name) = preg_split('/\:/',$name);
              $name = trim(preg_replace('/(?<!^)([A-Z][a-z]|(?<=[a-z])[A-Z])/'," $1", $name));
              $output .= '<li><b>'.$name. '</b>:  '. $value .' </li>';
            }
          }
          $output.='</ul></div>';
        }
      }
    }
    return $output;
  }
}

?>