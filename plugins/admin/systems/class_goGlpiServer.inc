<?php

class goGlpiServer extends plugin{
	
  var $cli_summary      = "This pluign is used within the ServerService Pluign \nand indicates that this server supports asterisk management.";
  var $cli_description  = "Some longer text\nfor help";
  var $cli_parameters   = array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* This plugin only writes its objectClass */
  var $objectclasses    = array("goGlpiServer");
  var $attributes       = array("goGlpiServerStatus","goGlpiAdmin","goGlpiDatabase","goGlpiPassword");
  var $StatusFlag       = "goGlpiServerStatus";
 
  /* This class can't be assigned twice so it conflicts with itsself */
  var $conflicts        = array("goGlpiServer");

  var $DisplayName      = "";
  var $dn               = NULL;
  var $acl;

  var $goGlpiServerStatus ="";
  var $goGlpiAdmin        ="";
  var $goGlpiDatabase     ="";
  var $goGlpiPassword     ="";
 
 
  function goGlpiServer($config,$dn)
  {
    plugin::plugin($config,$dn);
    $this->DisplayName = _("Inventory database");
  }


  function execute()
  { 
    $smarty = get_smarty(); 
    foreach($this->attributes as $attr){
      $smarty->assign($attr,$this->$attr);
      $smarty->assign($attr."ACL",chkacl($this->acl,$attr));
    }
    return($smarty->fetch(get_template_path("goGlpiServer.tpl",TRUE,dirname(__FILE__))));
  }


  function getListEntry()
  {
    $flag = $this->StatusFlag;
    $fields['Status']     = $this->$flag;
    $fields['Message']    = _("Glpi management");
    $fields['AllowStart'] = true;
    $fields['AllowStop']  = true;
    $fields['AllowReset'] = true;
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
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->dn);
    $ldap->cat($this->dn,array("objectClass","goGlpiAdmin","goGlpiDatabase"));
      
    if($ldap->count()){
      $attrs =array();
      foreach(array("goGlpiAdmin","goGlpiDatabase") as $req) {
        if(!isset($attrs[$req])){
          if(empty($this->$req)){
            print_red(sprintf(_("The required attribute '%s' is not set."),$req));
          }else{
            $attrs[$req] = $this->$req;
          }
        }else{
          $attrs[$req] = $attrs[$req][0];
        }
      }

      $tmp = $ldap->fetch();
      for($i = 0; $i < $tmp['objectClass']['count']; $i ++){
        $attrs['objectClass'][] = $tmp['objectClass'][$i];
      }    
      if(!in_array("goGlpiServer",$attrs['objectClass'])){
        $attrs['objectClass'][] = "goGlpiServer";
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
    foreach(array("goGlpiAdmin","goGlpiDatabase") as $attr){
      if(empty($this->$attr)){
        $message[]= sprintf(_("The attribute '%s' is empty or contains invalid characters."), $attr);
      }
    }
    return($message);
  }
  

  function save_object()
  {
    if(isset($_POST['goLogDBServerPosted'])){
      plugin::save_object();
    }
  }  
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
