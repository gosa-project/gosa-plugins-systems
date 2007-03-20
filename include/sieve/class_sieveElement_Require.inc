<?php

class sieve_require 
{
  var $data = array();
  var $object_id = -1;
  var $parent = NULL;  

  function sieve_require($data,$object_id,$parent)
  {
    $this->parent = $parent;
    $this->object_id = $object_id;
    if($data != NULL){

      foreach($data['ELEMENTS'] as $node ){
        if(in_array($node['class'],array("quoted-string","text"))){
          $this->data[] = preg_replace("/\"/","",$node['text']);
        }
      }
    }
  }

  /* Add a new require statement and ensure 
   *  that it is not specified twice 
   */
  function Add_Require($str)
  {
    $current = array();

    foreach($this->data as $dat){
      $current[] = preg_replace("/\"/","",$dat);
    }
    if(!in_array($str,$current)){
      $this->data[] = $str;
    }
    $this->data = array_unique($this->data);;
  }

  function save_object()
  {
    /* Get the values should check for, they are seperated by , */
    if(isset($_POST['require_'.$this->object_id])){
      $vls = stripslashes($_POST['require_'.$this->object_id]);
      $tmp = array();

      $tmp2 = split(",",$vls);
      foreach($tmp2 as $val){
        
        $val = trim(preg_replace("/\"/","",$val));
    
        if(empty($val)) continue;        
  
        $tmp[] = $val;
      }
      $this->data = $tmp;
    }
  }

  function check()
  {
    $msgs = array();
  
    if(!count($this->data)){
      $msgs[] = _("Please specify at least one valid requirement.");
    }
    return($msgs);
  }

  function get_sieve_script_part()
  {
    $tmp = sieve_create_strings($this->data);
    return("require ".$tmp.";\n");
  } 
    
  function execute()
  {
    $Require = "";
    foreach($this->data as $key){
      $Require .= "\"".$key."\"".", ";
    }
    $Require = preg_replace("/,$/","",trim($Require));

    $smarty = get_smarty();
    $smarty->assign("Require",$Require);
    $tmp = $this->check();
    $smarty->assign("LastError",$tmp);
    $smarty->assign("LastErrorCnt",count($tmp));
    $smarty->assign("ID", $this->object_id);
    $object_container = $smarty->fetch(get_template_path("templates/object_container.tpl",TRUE,dirname(__FILE__)));
    $object= $smarty->fetch(get_template_path("templates/element_require.tpl",TRUE,dirname(__FILE__)));
    $str = preg_replace("/%%OBJECT_CONTENT%%/",$object,$object_container);
    return($str);
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
