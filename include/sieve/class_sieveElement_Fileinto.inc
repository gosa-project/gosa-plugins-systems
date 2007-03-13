<?php

class sieve_fileinto 
{
  var $data     = array();
  var $object_id= -1;
  var $options  = array();

  function save_object()
  {
    $mbs = $this->get_mail_boxes();
    
    if(isset($_POST['fileinto_'.$this->object_id])){
      $mb = $_POST['fileinto_'.$this->object_id];

      if(isset($mbs[$mb])) {
        $this->data[0] = $mb; 
      }
    }
  }

  function sieve_fileinto($data,$object_id)
  {
    $this->object_id = $object_id;
    foreach($data['ELEMENTS'] as $node ){
      if(in_array($node['class'],array("quoted-string","text"))){
        $this->data[] = preg_replace("/\"/","",$node['text']);
      }
    }
  }

  function get_sieve_script_part()
  {
    $tmp = "";
    foreach($this->data as $dat){
      $tmp.= "\"".$dat."\", ";
    }
    $tmp = preg_replace("/,$/","",trim($tmp));
    $tmp = preg_replace ("/\"\"/","\"",$tmp);
    return("fileinto ".$tmp.";\n");
  } 
    
  function execute()
  {
    $smarty = get_smarty();
    $smarty->assign("Selected",$this->data[0]);
    $smarty->assign("Boxes", $this->get_mail_boxes());
    $smarty->assign("ID", $this->object_id);
    $object_container = $smarty->fetch(get_template_path("templates/object_container.tpl",TRUE,dirname(__FILE__)));
    $object= $smarty->fetch(get_template_path("templates/element_fileinto.tpl",TRUE,dirname(__FILE__)));
    $str = preg_replace("/%%OBJECT_CONTENT%%/",$object,$object_container);

    return($str);
  }

  function check()
  {
    return(array());
  }

  function get_mail_boxes()
  {
    return(array("not"=>"not","impplemented/yet"=>"impplemented/yet"));
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
