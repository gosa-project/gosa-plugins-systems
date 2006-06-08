<?php

class goKrbServer extends plugin{
	
  var $cli_summary      = "This pluign is used within the ServerService Pluign \nand indicates that this server supports NTP service.";
  var $cli_description  = "Some longer text\nfor help";
  var $cli_parameters   = array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* This plugin only writes its objectClass */
  var $objectclasses    = array("goKrbServer");
  var $attributes       = array("goKrbServerStatus","goKrbRealm", "goKrbAdmin","goKrbPassword");
  var $StatusFlag       = "goKrbServerStatus";
 
  /* This class can't be assigned twice so it conflicts with itsself */
  var $conflicts        = array("goKrbServer");

  var $DisplayName      = "";
  var $dn               = NULL;
  var $goKrbServerStatus= "";
  var $acl;

  var $goKrbRealm     = "";
  var $goKrbAdmin     = "";
  var $goKrbPassword  ="";  
 
  function goKrbServer($config,$dn)
  {
    plugin::plugin($config,$dn);
    $this->DisplayName = _("Kerberos service");
  }


  function execute()
  { 
    $smarty = get_smarty(); 
    foreach($this->attributes as $attr){
      $smarty->assign($attr,$this->$attr);
      $smarty->assign($attr."ACL",chkacl($this->acl,$attr));
    }
    return($smarty->fetch(get_template_path("goKrbServer.tpl",TRUE,dirname(__FILE__))));
  }


  function getListEntry()
  {
    $flag = $this->StatusFlag;
    $fields['Status']      = $this->$flag;
    $fields['Message']    = _("Kerberos kadmin access informations");
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
      $this->action_hook();
    }
  }
 
 
  function check()
  { 
    $message = plugin::check();
    /* goKrbPassword is a must field, if goKrbServer is used as objectClass */
    if (empty($this->goKrbPassword)){
      $message[]= sprintf(_("The specified kerberos password is empty."), $attr);
    }
    if (empty($this->goKrbAdmin)){
      $message[]= sprintf(_("The specified kerberos admin is empty."), $attr);
    }
    if (empty($this->goKrbRealm)){
      $message[]= sprintf(_("The specified kerberos realm is empty."), $attr);
    }

    return($message);
  }
  

  function save_object()
  {
    if(isset($_POST['goKrbServerPosted'])){
      plugin::save_object();
    }
  } 

   function action_hook($add_attrs= array())
  {
    /* Find postcreate entries for this class */
    $command= search_config($this->config->data['MENU'], get_class($this), "ACTION_HOOK");
    if ($command == "" && isset($this->config->data['TABS'])){
      $command= search_config($this->config->data['TABS'], get_class($this), "ACTION_HOOK");
    }
    if ($command != ""){
      /* Walk through attribute list */
      foreach ($this->attributes as $attr){
        if (!is_array($this->$attr)){
          $command= preg_replace("/%$attr/", $this->$attr, $command);
        }
      }
      $command= preg_replace("/%dn/", $this->dn, $command);
      /* Additional attributes */
      foreach ($add_attrs as $name => $value){
        $command= preg_replace("/%$name/", $value, $command);
      }
      if (check_command($command)){
        @DEBUG (DEBUG_SHELL, __LINE__, __FUNCTION__, __FILE__,
            $command, "Execute");

        exec($command);
      } else {
        $message= sprintf(_("Command '%s', specified as ACTION_HOOK for plugin '%s' doesn't seem to exist."), $command, get_class($this));
        print_red ($message);
      }
    }
  }
 
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
