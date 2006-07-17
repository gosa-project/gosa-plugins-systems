<?php

function smarty_block_render($params, $text, &$smarty)
{
	$text = stripslashes($text);

	if (isset($params['acl'])) {
		$acl = $params['acl'];
		unset($params['acl']);
	}

	if(preg_match("/w/i",$acl)){
		
		/* Read / Write*/

	}elseif(preg_match("/r/i",$acl)){
		
		/* Read only */
		$from 	=  array("/name=/i");
		$to 	=  array("disabled name=");
		$text = preg_replace($from,$to,$text);

	}elseif(empty($acl)){
		
		/* No rights */
		$text= "&nbsp;";
		
	}else{
		// unknown
		$text ="&nbsp;";
	}
		

	return $text;
}

?>
