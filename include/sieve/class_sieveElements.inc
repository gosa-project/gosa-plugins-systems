<?php


class sieve_elsif extends sieve_if 
{
  var $TYPE = "elsif";
}


class sieve_comment 
{
  var $data = "";
 
    
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
  var $hanlde   = "";
  var $reason   = "";
  var $addresses= array();

  function sieve_vacation($data)
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


  function execute()
  {
    $str ="";
    $str .="<b>"._("Vacation message")."</b>";
    foreach($this->addresses as $addr){ 
      $str .="<br><input type='text' value='".$addr."' style='width:50%;'>";
    }
    $str .="<br><textarea style='width:100%;height:60px;'>".$this->reason."</textarea>";
    
    return($str);
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
}

class sieve_block_end 
{
  function execute()
  {
    return("    </td>
              </tr>
            </table>");
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
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
