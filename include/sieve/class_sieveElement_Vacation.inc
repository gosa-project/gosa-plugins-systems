<?php

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

    /* skip if empty */
    if($data == NULL) return;

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

  function check()
  {
    return(array())  ;
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

    $object_container = $smarty->fetch(get_template_path("templates/object_container.tpl",TRUE,dirname(__FILE__)));
    $object= $smarty->fetch(get_template_path("templates/element_vacation.tpl",TRUE,dirname(__FILE__)));
    $str = preg_replace("/%%OBJECT_CONTENT%%/",$object,$object_container);
    return($str);
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
