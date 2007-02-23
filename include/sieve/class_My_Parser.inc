<?php

class My_Parser extends Parser 
{
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
