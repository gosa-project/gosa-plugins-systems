<?php

class sieveElement 
{
	
	var $name 			= "Unknown";
	var $responsible_for= array();	

  var $data ="";

	function sieveElement()
	{
	
	}

  function resolve_to_object($data)
  {
    $this->data = @$data->dumpParseTree();
  }

  function execute()
  {
    return($this->data);
  }
}	



// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
