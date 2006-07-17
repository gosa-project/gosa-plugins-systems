<?php

function smarty_block_render($params, $text, &$smarty)
{
	$text = stripslashes($text);
	$acl = "";

	if (isset($params['acl'])) {
		$acl = $params['acl'];
		unset($params['acl']);
	}

	/* Read / Write*/
	if(preg_match("/w/i",$acl)){
		return ($text);
	}

	$from 	=  array("/name=/i");
	$to 	=  array("disabled name=");
	$text = preg_replace($from,$to,$text);
		
	/* Read only */
	if(preg_match("/r/i",$acl)){
		return($text);	
	}

	/* No acls */	
	if(preg_match("/type['\"= ].*submit/",$text)){
		$text = preg_replace("/submit/","button",$text);
	}else{
		$text = preg_replace("/value=['\" ].*['\" ]/","",$text);
	}

	return $text;
}

?>
