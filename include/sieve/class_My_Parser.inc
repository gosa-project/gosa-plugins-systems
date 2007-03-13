<?php

define("SIEVE_INDENT_TAB","  ");

/* This class is inherited from the original 'Parser'
 *  class written by Heiko Hund
 */
class My_Parser extends Parser 
{

	function execute()
	{


		/* Add Element requested */
		if(isset($_POST['Add_Element'])){
			$this->tree_->Add_Element();
			echo "Move this into Management";
		}

		/* Create dump of current sieve script */
		if(isset($_POST['Save_Copy'])){
			echo "Move this into Management";
		
			/* force download dialog */
			header("Content-type: application/tiff\n");
			if (preg_match('/MSIE 5.5/', $HTTP_USER_AGENT) ||
					preg_match('/MSIE 6.0/', $HTTP_USER_AGENT)) {
				header('Content-Disposition: filename="dump.txt"');
			} else {
				header('Content-Disposition: attachment; filename="dump.txt"');
			}
			header("Content-transfer-encoding: binary\n");
			header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
			header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
			header("Cache-Control: no-cache");
			header("Pragma: no-cache");
			header("Cache-Control: post-check=0, pre-check=0");
			echo $this->get_sieve_script();	
			exit();	
		}
	
		$ret = $this->dumpParseTree();
		return($ret);
	}
	

	/* Check if there are errors, collect them and return them */
	function check()
	{
		return($this->tree_->check());
	}
	

	

	/* Initiate parser, but use some other 
     *  classes, that are rewritten.
     */
	function parse($script) 
	{
        $this->status_text = "incomplete";
        $this->script_ = $script;
        $this->tree_ = new My_Tree(@Scanner::scriptStart());
        $this->tree_->setDumpFunc(array(&$this, 'dumpToken_'));
        $this->scanner_ = new Scanner($this->script_);
        $this->scanner_->setCommentFunc(array($this, 'comment_'));

        if ($this->commands_($this->tree_->getRoot()) &&
            $this->scanner_->nextTokenIs('script-end'))
        {
            return $this->success_('success');
        }

        return $this->status_;
	}

	
	function get_sieve_script()
	{
		return($this->tree_->get_sieve_script());
	}		

	
	function save_object()
	{
		$this->tree_->save_object();
	}


	/* Should be obsolete in the end. */
	function dumpToken_(&$token)
	{
		if (is_array($token))
		{
			$str = "<" . $token['text'] . "> ";
			foreach ($token as $k => $v)
			{
				$str .= " $k:$v";
			}
			return $str;
		}
		return strval($token);
	}
}
?>
