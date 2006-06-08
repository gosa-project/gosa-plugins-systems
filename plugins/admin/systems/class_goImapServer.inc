<?php

class goImapServer extends plugin{
	
  var $cli_summary      = "This pluign is used within the ServerService Pluign \nand indicates that this server supports mailqueue listings and so on.";
  var $cli_description  = "Some longer text\nfor help";
  var $cli_parameters   = array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* This plugin only writes its objectClass */
  var $objectclasses    = array("goImapServer");
  
  /* This class can't be assigned twice so it conflicts with itsself */
  var $conflicts        = array("goImapServer");

  var $DisplayName      = "";
  var $dn               = NULL;
  var $StatusFlag       = "goImapServerStatus";
  var $attributes       = array("goImapServerStatus",
                                "goImapName","goImapConnect","goImapAdmin","goImapPassword",
                                "goImapSieveServer","goImapSievePort",
                                "cyrusImap","cyrusImapSSL","cyrusPop3","cyrusPop3SSL");

  var $cn;

  var $goImapName           = "";
  var $goImapConnect        = "";
  var $goImapAdmin          = "";
  var $goImapPassword       = "";
  
  var $goImapSieveServer    = "";
  var $goImapSievePort      = "";
  
  var $goImapServerStatus   = "";
  
  var $cyrusImap            = false;
  var $cyrusImapSSL         = false;
  var $cyrusPop3            = false;
  var $cyrusPop3SSL         = false;
  var $is_account           = false;

  var $acl; 

  var $Actions              = array();
 
  function goImapServer($config,$dn)
  {
    plugin::plugin($config,$dn);
  
    $this->DisplayName = _("IMAP/POP3 service");

    $this->Actions = array( SERVICE_STOPPED=>SERVICE_STOPPED,
                            SERVICE_STARTED => SERVICE_STARTED,
                            SERVICE_RESTARTED=>SERVICE_RESTARTED,
                            "repair_database"=>_("Repair database"));
    
  }

  function execute()
  { 
    $smarty = get_smarty();
  
    /* set new status */
    if(isset($_POST['ExecAction'])){
      if(isset($this->Actions[$_POST['action']])){
        $this->setStatus($_POST['action']);
      }
    }

    foreach($this->attributes as $attr){
      $smarty->assign($attr,$this->$attr);  
      $smarty->assign($attr."ACL",chkacl($this->acl,$attr));  
    }
    $smarty->assign("Actions",$this->Actions);
    $smarty->assign("is_new",$this->dn);
    $smarty->assign("is_acc",$this->initially_was_account);
    return($smarty->fetch(get_template_path("goImapServer.tpl",TRUE,dirname(__FILE__))));
  }

  function getListEntry()
  {
    $flag = $this->StatusFlag;
    $fields['Status']     = $this->$flag;
    $fields['Message']    = _("Cyrus service");
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
    $this->goImapSieveServer = $this->cn;
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
    if(empty($this->goImapName)){
      $message[] =_("Please specify a server identifier.");
    }  
    if(empty($this->goImapConnect)){
      $message[] =_("Please specify a connect url.");
    }  
    if(empty($this->goImapAdmin)){
      $message[] =_("Please specify an admin user.");
    }  
    if(empty($this->goImapPassword)){
      $message[] =_("Please specify a password for the admin user.");
    } 

    /* Check connect string */
    if (!preg_match('/^\{[^:]+:[0-9]+.*\}$/', $this->goImapConnect)){
      $message[]= sprintf(_("The imap connect string needs to be in the form '%s'."),
          '{server-name:port/options}');
    }
    if (!preg_match('/^[0-9]+$/', $this->goImapSievePort)){
      $message[]= _("The sieve port needs to be numeric.");
    }
 
    return ($message);
  }


  function save_object()
  {
    if(isset($_POST['goImapServerPosted'])){
      plugin::save_object(); 

      foreach(array("cyrusImap","cyrusImapSSL","cyrusPop3","cyrusPop3SSL") as $checkbox) { 
        if(!isset($_POST[$checkbox])){
          $this->$checkbox = false;
        }else{
          $this->$checkbox = true;
        }
      }
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
