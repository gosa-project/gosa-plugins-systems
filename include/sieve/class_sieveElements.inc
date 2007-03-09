<?php

/* Sieve else tag */
class sieve_elsif extends sieve_if 
{
  var $TYPE = "elsif";
}

class sieve_else 
{
  var $object_id = -1;

  function sieve_else($data,$object_id)
  {
    $this->object_id = $object_id;
  }

  function save_object()
  {
  }

  function execute()
  {
    $smarty = get_smarty();
    $smarty->assign("ID", $this->object_id);
    $object_container = $smarty->fetch(get_template_path("templates/object_container_clear.tpl",TRUE,dirname(__FILE__)));
    $object= $smarty->fetch(get_template_path("templates/element_else.tpl",TRUE,dirname(__FILE__)));
    $str = preg_replace("/%%OBJECT_CONTENT%%/",$object,$object_container);
    return($str);
  }

  function get_sieve_script_part()
  {
    return(" else ");
  } 
}


/* Sieve comment tag */
class sieve_comment 
{
  var $data = "";
  var $object_id= -1;

  function get_sieve_script_part()
  {
    return($this->data."\n");
  } 
    
  function sieve_comment($data,$object_id)
  {
    $this->object_id = $object_id;
    foreach($data['ELEMENTS'] as $node){
       $this->data .= $node['text'];
    }
  }

  function save_object()
  {
    if(isset($_POST['comment_'.$this->object_id])){
      $cm = $_POST['comment_'.$this->object_id];
      $this->data = "/*".$cm."*/";
    }
  }

  function execute()
  {
    $smarty = get_smarty();
    $smarty->assign("ID", $this->object_id);
    $object_container = $smarty->fetch(get_template_path("templates/object_container.tpl",TRUE,dirname(__FILE__)));
    $Comment = $this->data;

    /* Remove comment tags */
    if(preg_match("/^#/",$Comment)){
      $Comment = preg_replace("/^#/","",$Comment);
    }elseif(preg_match("#\/\*#",$Comment)){
      $Comment = preg_replace("#^\/\*#","",$Comment);
      $Comment = preg_replace("#\*\/$#","",$Comment);
    }
 
    /* Create html object */ 
    $smarty = get_smarty();
    $smarty->assign("Comment",$Comment);
    $smarty->assign("ID",$this->object_id);
    $object = $smarty->fetch(get_template_path("templates/element_comment.tpl",TRUE,dirname(__FILE__)));    
    $str = preg_replace("/%%OBJECT_CONTENT%%/",$object,$object_container);
    return($str);
  }
}


class sieve_require 
{
  var $data = array();
  var $object_id = -1;
  
  function sieve_require($data,$object_id)
  {
    $this->object_id = $object_id;
    foreach($data['ELEMENTS'] as $node ){
      if(in_array($node['class'],array("quoted-string","text"))){
        $this->data[] = preg_replace("/\"/","",$node['text']);
      }
    }
  }

  function save_object()
  {
    /* Get the values should check for, they are seperated by , */
    if(isset($_POST['require_'.$this->object_id])){
      $vls = stripslashes($_POST['require_'.$this->object_id]);
      $tmp = array();

      $tmp2 = split(",",$vls);
      foreach($tmp2 as $val){
        $tmp[] = "\"".trim(preg_replace("/\"/","",$val))."\"";
      }
      $this->data = $tmp;
    }
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
      $Require .= $key.", ";
    }
    $Require = preg_replace("/,$/","",trim($Require));
  
    $smarty = get_smarty();
    $smarty->assign("Require",$Require);
    $smarty->assign("ID",$this->object_id);
    return($smarty->fetch(get_template_path("templates/element_require.tpl",TRUE,dirname(__FILE__))));
  }
}

class sieve_discard 
{
  var $data = array();
  var $object_id = -1;

  function sieve_discard($data,$object_id)
  {
    $this->object_id = $object_id;
  }

  function get_sieve_script_part()
  {
    return("discard;\n");
  } 
    
  function save_object()
  {
  
  }

  function execute()
  {
    $smarty = get_smarty();
    $smarty->assign("ID", $this->object_id);
    $object_container = $smarty->fetch(get_template_path("templates/object_container.tpl",TRUE,dirname(__FILE__)));
    $object = $smarty->fetch(get_template_path("templates/element_discard.tpl",TRUE,dirname(__FILE__)));
    $str = preg_replace("/%%OBJECT_CONTENT%%/",$object,$object_container);
    return($str);
  }
}



class sieve_reject 
{
  var $data = array();
  var $object_id = -1;


  function save_object()
  {
    if(isset($_POST['reject_message_'.$this->object_id])){
      $msg = stripslashes($_POST['reject_message_'.$this->object_id]);

      $this->data = $msg;
    }
  }

  function sieve_reject($data,$object_id)
  {
    $this->object_id = $object_id;
    $str = "";
    foreach($data['ELEMENTS'] as $node ){
      if(in_array($node['class'],array("quoted-string","text","multi-line"))){

        if($node['class'] == "multi-line"){
          $str .= preg_replace("/^text:[ \n\r]*/","",$node['text']);
          $str =  preg_replace("/[  \n\r]*\.[  \n\r]*$/","",$str);
        }else{
          $str .= $node['text'];
        }
      }
    }
    $this->data = preg_replace("/\"/","",$str);
  }

  function get_sieve_script_part()
  {
    return("reject ".sieve_create_strings($this->data).";\n");
  } 
    
  function execute()
  {
    /* check if this will be a 
     *   - single string ""
     *   - or a multi line text: ... ; 
     */
    $Multiline = preg_match("/\n/",$this->data);

    $smarty = get_smarty();
    $smarty->assign("ID",$this->object_id);
    $smarty->assign("Multiline",$Multiline);
    $smarty->assign("Message",$this->data);
    return($smarty->fetch(get_template_path("templates/element_reject.tpl",TRUE,dirname(__FILE__))));    
  }
}

class sieve_redirect 
{
  var $data = array();
  var $object_id = -1;

  function save_object()
  {
    if(isset($_POST['redirect_to_'.$this->object_id])){
      $rt = stripslashes($_POST['redirect_to_'.$this->object_id]);

      $tmp = array();
      $tmp2 = split(",",$rt);
      foreach($tmp2 as $val){
        $tmp[] = "\"".trim(preg_replace("/\"/","",$val))."\"";
      }
      $this->data = $tmp;
    }
  }


  function sieve_redirect($data,$object_id)
  {
    foreach($data['ELEMENTS'] as $node ){
      if(in_array($node['class'],array("quoted-string","text"))){
        $this->data[] = $node['text'];
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
    return("redirect ".$tmp.";\n");
  } 
   
 
  function execute()
  {
    $values = "";
    foreach($this->data as $key){
      $values .= $key.", ";
    }
    $values = preg_replace("/,$/","",trim($values));

    $smarty = get_smarty();
    $smarty->assign("Destinations" , $values);
    $smarty->assign("ID" , $this->object_id);
    return($smarty->fetch(get_template_path("templates/element_redirect.tpl",TRUE,dirname(__FILE__))));
  }
}

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
    return($smarty->fetch(get_template_path("templates/element_fileinto.tpl",TRUE,dirname(__FILE__))));
  }

  function get_mail_boxes()
  {
    return(array("not","impplemented/yet"));
  }
}

class sieve_vacation 
{
  var $days     = FALSE;
  var $subject  = FALSE;
  var $from     = "";
  var $mime     = "";
  var $handle   = "";
  var $reason   = "";
  var $addresses= array();
  var $object_id= -1;
  var $Expert   = FALSE;

  function sieve_vacation($data,$object_id)
  {
    /* Usage:   vacation [":days" number] [":subject" string]
       [":from" string] [":addresses" string-list]
       [":mime"] [":handle" string] <reason: string> */

    /* Not all attribute types are supported by the sieve class right now */
    $known_attrs = array(":days",":subject",":from",":mime",":handle");

    /* Walk through elements */
    for($i = 0 ; $i < count($data['ELEMENTS']) ; $i ++){

      /* get current element */
      $node = $data['ELEMENTS'][$i];

      /* Check if tag is in the specified list of attributes */
      if($node['class'] == "tag" && in_array($node['text'],$known_attrs)){

        $var = preg_replace("/\:/","",$node['text']);
        $this->$var = $data['ELEMENTS'][$i+1]['text'];
        $i ++;
      }

      /* Check for addresses */
      if($node['class'] == "tag" && $node['text'] == ":addresses") {
        $this->addresses = array();
        $i ++;

        /* Multiple or single address given */
        if($data['ELEMENTS'][$i]['class'] == "left-bracket"){
          while($data['ELEMENTS'][$i]['class'] != "right-bracket" && ($i < count($data['ELEMENTS']))){
            $i ++;
            if($data['ELEMENTS'][$i]['class'] == "quoted-string"){
              $this->addresses[] = preg_replace("/\"/i","",$data['ELEMENTS'][$i]['text']);
            }
          }
        }else{
          $this->addresses[] = $data['ELEMENTS'][$i]['text'] ;
        }
      }

      /* Add the vacation message */
      if($node['class'] == "quoted-string"){
        $this->reason = $node['text'];
      }
    }
  }

  function get_sieve_script_part()
  {
    $str = "vacation ";
    if($this->days){
      $str.= ":days ".$this->days;
    }
    $str .= ":addresses ".sieve_create_strings($this->addresses);
    if($this->subject){
      $str.= ":subject ".sieve_create_strings($this->subject);
    }
    if($this->mime){
      $str.= ":mime ".sieve_create_strings($this->mime);
    }
    $str .= "\n ".sieve_create_strings($this->reason);
    return($str." ; \n");
  } 
    
  
  function save_object()
  {
    /* Get release date */
    if(isset($_POST['vacation_release_'.$this->object_id])){
      $this->days = $_POST['vacation_release_'.$this->object_id];
    }

    /* Check if we want to toggle the expert mode */
    if(isset($_POST['Toggle_Expert_'.$this->object_id])){
      $this->Expert = !$this->Expert;
    }

    /* Get release date */
    if(isset($_POST['vacation_receiver_'.$this->object_id])){
      $vr = stripslashes ($_POST['vacation_receiver_'.$this->object_id]);
      $tmp = array();
      $tmp2 = split(",",$vr);
      foreach($tmp2 as $val){
        $tmp[] = "\"".trim(preg_replace("/\"/","",$val))."\"";
      }
      $this->addresses = $tmp;      
    }

    /* Get reason */
    if(isset($_POST['vacation_reason_'.$this->object_id])){
      $vr = stripslashes ($_POST['vacation_reason_'.$this->object_id]);
      $this->reason = "\"".trim(preg_replace("/\"/","",$vr))."\"";
    }
  }

  function execute()
  {
    $Addresses = "";
    foreach($this->addresses as $key){
      $Addresses .= $key.", ";
    }
    $Addresses = preg_replace("/,$/","",trim($Addresses));
    $smarty = get_smarty();
    $smarty->assign("Reason",$this->reason);
    $smarty->assign("Addresses",$Addresses);
    $smarty->assign("Subject",$this->subject);
    $smarty->assign("Days",$this->days);
    $smarty->assign("ID",$this->object_id);
    $smarty->assign("Expert",$this->Expert);
    return($smarty->fetch(get_template_path("templates/element_vacation.tpl",TRUE,dirname(__FILE__))));
  }
}

class sieve_block_start 
{
  function execute()
  {
    $smarty = get_smarty();
    return($smarty->fetch(get_template_path("templates/element_block_start.tpl",TRUE,dirname(__FILE__))));
  }

  function save_object()
  {
  }

  function get_sieve_script_part()
  {
    return("{\n");
  } 
}

class sieve_block_end 
{
  function execute()
  {
    $smarty = get_smarty();
    return($smarty->fetch(get_template_path("templates/element_block_end.tpl",TRUE,dirname(__FILE__))));
  }
  function get_sieve_script_part()
  {
    return("}\n");
  } 
  function save_object()
  {
  }

}

/* This class handles the keep statement */
class sieve_keep 
{
  var $object_id = -1;

  function sieve_keep($data,$object_id)
  {
    $this->object_id = $object_id;
  }

  function save_object()
  {
  }

  function execute()
  {
    $smarty = get_smarty();
    $smarty->assign("ID", $this->object_id);
    $object_container = $smarty->fetch(get_template_path("templates/object_container.tpl",TRUE,dirname(__FILE__)));
    $object = $smarty->fetch(get_template_path("templates/element_keep.tpl",TRUE,dirname(__FILE__)));
    $str = preg_replace("/%%OBJECT_CONTENT%%/",$object,$object_container);
    return($str);
  }
  function get_sieve_script_part()
  {
    return("keep;\n");
  } 
    
}

/* This class handles the stop statement */
class sieve_stop 
{
  var $object_id = -1;

  function sieve_stop($data,$object_id)
  {
    $this->object_id = $object_id;
  }

  function save_object()
  {
  }

  function execute()
  {
    $smarty = get_smarty();
    $smarty->assign("ID", $this->object_id);
    $object_container = $smarty->fetch(get_template_path("templates/object_container.tpl",TRUE,dirname(__FILE__)));
    $object= $smarty->fetch(get_template_path("templates/element_stop.tpl",TRUE,dirname(__FILE__)));
    $str = preg_replace("/%%OBJECT_CONTENT%%/",$object,$object_container);
    return($str);
  }

  function get_sieve_script_part()
  {
    return("stop; \n");
  } 
    
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
