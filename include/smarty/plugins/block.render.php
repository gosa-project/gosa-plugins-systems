<?php

function smarty_block_render($params, $text, &$smarty)
{
	$text = stripslashes($text);
	$acl = "";

	/* Skip closing tag </render> */	
	if(empty($text)) {
		return("");
	}

	/* Get acl parameter */
	if (isset($params['acl'])) {
		$acl = $params['acl'];
		unset($params['acl']);
	}

//	echo "<font color='blue' size='2'>".$acl."</font>";

	/* Read / Write*/
	if(preg_match("/w/i",$acl)){
		return ($text);
	}

	/* Disable objects, but keep those active that have mode=read_active */
	if(!(isset($params['mode']) && $params['mode']=='readable')){
		$from 	=  array("/name=/i");
		$to 	=  array("disabled name=");
		$text = preg_replace($from,$to,$text);

		/* Replace picture if object is disabled */
		if(isset($params['disable_picture'])){
			$syn = "/src=['\"][a-z0-9\/.]*['\"]/i";
			$new = "src=\"".$params['disable_picture']."\"";
			$text = preg_replace($syn,$new,$text);
		}

	}		

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
