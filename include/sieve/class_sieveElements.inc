<?php


class sieve_elsif extends sieve_if 
{
  var $TYPE = "elsif";
}


class sieve_comment 
{
  var $data = "";

  function get_sieve_script_part()
  {
    return($this->data."\n");
  } 
    
  function sieve_comment($data)
  {
    foreach($data['ELEMENTS'] as $node){
       $this->data .= $node['text'];
    }
  }

  function execute()
  {
    $str  ="<table cellspacing=0 width='100%'><tr><td style='width:100%;background-color:#DDFFDD;'>"._("Comment");
    $str .="<input type='text' name='comment_' value='".$this->data."'>";
    $str .="</td></tr></table>";
    return($str);
  }
}


class sieve_require 

{
  var $data = array();

  function sieve_require($data)
  {
    foreach($data['ELEMENTS'] as $node ){
      if(in_array($node['class'],array("quoted-string","text"))){
        $this->data[] = preg_replace("/\"/","",$node['text']);
      }
    }
  }

  function get_sieve_script_part()
  {
    $tmp = sieve_create_strings($this->data);
    return("require ".$tmp.";\n");
  } 
    
  function execute()
  {
    $str = "<table cellspacing=0 width='100%'><tr><td style='width:100%;background-color:#DDDDFF;'>"._("Script includes");
    foreach($this->data as $req){
      $str .= "&nbsp;<i>".$req."</i>";
    }
    $str .="</td></tr></table>";
    return($str);
  }
}

class sieve_discard 
{
  var $data = array();

  function sieve_discard($data)
  {
  }

  function get_sieve_script_part()
  {
    return("discard;\n");
  } 
    
  function execute()
  {
    $str = "<table cellspacing=0 width='100%'><tr><td style='width:100%;background-color:red;'>"._("Discard message");
    $str .="</td></tr></table>";
    return($str);
  }
}



class sieve_reject 
{
  var $data = array();

  function sieve_reject($data)
  {
    $str = "";
    foreach($data['ELEMENTS'] as $node ){
      if(in_array($node['class'],array("quoted-string","text"))){
        $str .= $node['text'];
      }
    }
    $this->data = preg_replace("/\"/","",$str);
  }

  function get_sieve_script_part()
  {
    return("reject ".sieve_create_strings($this->data)."\n");
  } 
    
  function execute()
  {
    $str = "<table cellspacing=0 width='100%'><tr><td style='width:100%;background-color:gray;'>"._("Reject mail");
    $str .= "&nbsp;<textarea name='test' style='width:90%'>".$this->data."</textarea>";
    $str .="</td></tr></table>";
    return($str);
  }
}

class sieve_redirect 
{
  var $data = array();

  function sieve_redirect($data)
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
    $tmp = "[".preg_replace("/,$/","",trim($tmp))."]";
    $tmp = preg_replace ("/\"\"/","\"",$tmp);
    return("redirect ".$tmp."\n");
  } 
    
  function execute()
  {
    $str = "<table cellspacing=0 width='100%'><tr><td style='width:100%;background-color:brown;'>"._("Redirect to");
    foreach($this->data as $dest){
      $str .= "<input type='text' name='bal' value='".$dest."'><br> ";
    }
    $str .="</td></tr></table>";
    return($str);
  }
}

class sieve_fileinto 
{
  var $data = array();

  function sieve_fileinto($data)
  {
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
    $tmp = "[".preg_replace("/,$/","",trim($tmp))."]";
    $tmp = preg_replace ("/\"\"/","\"",$tmp);
    return("fileinto ".$tmp."\n");
  } 
    
  function execute()
  {
    $str = "<table cellspacing=0 width='100%'><tr><td style='width:100%;background-color:green;'>"._("File into");
    $str .= "<select name='test'>";
    foreach($this->data as $folder){
      $str .= "<option>".$folder."</option>";
    }
    $str .= "</select>";
    $str .="</td></tr></table>";

    return($str);
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
    $str .= " ".sieve_create_strings($this->reason);
    return($str."\n");
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
    return("<table cellspacing=0 width='100%'>
              <tr>
                <td style='width:20px;'>
                  <img alt='' src='images/forward.png' class='center'>
                </td>
                <td style='background-color:#BBBBBB;border: solid 2px #FFFFFF;'>");
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
    return("    </td>
              </tr>
            </table>");
  }
  function get_sieve_script_part()
  {
    return("}\n");
  } 
    
}

/* This class handles the keep statement */
class sieve_keep 
{
  function execute()
  {
    $str = "<table cellspacing=0 width='100%'>
              <tr>
                <td style='width:100%;background-color:green;'>".
                  _("Keep message");
    $str .="    </td>
              </tr>
            </table>";
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
  function execute()
  {
    $str = "<table cellspacing=0 width='100%'>
              <tr>
                <td style='width:100%;background-color:orange;'>".
                  _("Stop here");
    $str .="    </td>
              </tr>
            </table>";
    return($str);
  }

  function get_sieve_script_part()
  {
    return("stop; \n");
  } 
    
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
