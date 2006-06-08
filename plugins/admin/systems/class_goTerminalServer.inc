<?php

class goTerminalServer extends plugin{
	
  var $cli_summary      = "This pluign is used within the ServerService Pluign \nand indicates that this server supports asterisk management.";
  var $cli_description  = "Some longer text\nfor help";
  var $cli_parameters   = array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* This plugin only writes its objectClass */
  var $objectclasses    = array("goTerminalServer");
  var $attributes       = array("goTerminalServerStatus","goXdmcpIsEnabled", "goFontPath");
  var $StatusFlag       = "goTerminalServerStatus";
 
  /* This class can't be assigned twice so it conflicts with itsself */
  var $conflicts        = array("goTerminalServer");

  var $DisplayName      = "";
  var $dn               = NULL;
  var $acl;

  var $goTerminalServerStatus  = "";
  var $goXdmcpIsEnabled        = false;  
  var $goFontPath              = "";
 
 
  function goTerminalServer($config,$dn)
  {
    plugin::plugin($config,$dn);
    $this->DisplayName = _("Terminal service");
  }


  function execute()
  { 
    $smarty = get_smarty(); 
    foreach($this->attributes as $attr){
      $smarty->assign($attr,$this->$attr);
      $smarty->assign($attr."ACL",chkacl($this->acl,$attr));
    }
    return($smarty->fetch(get_template_path("goTerminalServer.tpl",TRUE,dirname(__FILE__))));
  }


  function getListEntry()
  {
    $flag = $this->StatusFlag;
    $fields['Status']     = $this->$flag;
    $fields['Message']    = _("Terminal service");
    $fields['AllowStart'] = true;
    $fields['AllowStop']  = true;
    $fields['AllowRestart'] = true;
    $fields['AllowRemove']= true;
    $fields['AllowEdit']  = true;
    return($fields);
  }


  function remove_from_parent()
  {
    plugin::remove_from_parent();
    /* Check if this is a new entry ... add/modify */
    $ldap = $this->config->get_ldap_link();
    $ldap->cat($this->dn,array("objectClass"));
    if($ldap->count()){
      $ldap->cd($this->dn);
      $ldap->modify($this->attrs);
    }else{
      $ldap->cd($this->dn);
      $ldap->add($this->attrs);
    }
    show_ldap_error($ldap->get_error());
  }


  function save()
  {
    plugin::save();

    if(!$this->goXdmcpIsEnabled){
      $this->attrs['goXdmcpIsEnabled'] = "0";
    }
  
    /* Check if this is a new entry ... add/modify */
    $ldap = $this->config->get_ldap_link();
    $ldap->cat($this->dn,array("objectClass"));
    if($ldap->count()){
      $ldap->cd($this->dn);
      $ldap->modify($this->attrs);
    }else{
      $ldap->cd($this->dn);
      $ldap->add($this->attrs);
    }
    show_ldap_error($ldap->get_error());
  }


  /* Directly save new status flag */
  function setStatus($value)
  {
    if($value == "none") return;
    if(!$this->initially_was_account) return;
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->dn);
    $ldap->cat($this->dn,array("objectClass"));
    if($ldap->count()){

      $tmp = $ldap->fetch();
      for($i = 0; $i < $tmp['objectClass']['count']; $i ++){
        $attrs['objectClass'][] = $tmp['objectClass'][$i];
      }
      $flag = $this->StatusFlag;
      $attrs[$flag] = $value;
      $this->$flag = $value;
      $ldap->modify($attrs);
      show_ldap_error($ldap->get_error());
    }
  }


  function check()
  { 
    $message = plugin::check();
    if(empty($this->goFontPath)){
      $message[]=_("Terminal server, must have fontpath specified.");
    }

    return($message);
  }
  

  function save_object()
  {
    if(isset($_POST['goTerminalServerPosted'])){
      plugin::save_object();
      if(isset($_POST['goXdmcpIsEnabled'])){
        $this->goXdmcpIsEnabled = true;
      }else{
        $this->goXdmcpIsEnabled = false;
      }
    }
  }  
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
