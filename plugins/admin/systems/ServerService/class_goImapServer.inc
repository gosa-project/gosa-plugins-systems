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

  var $acl; 

  var $Actions              = array();
 
  function goImapServer($config,$dn)
  {
    plugin::plugin($config,$dn);
  
    $this->DisplayName = _("Cyrus service");

    $this->Actions = array( SERVICE_STOPPED=>SERVICE_STOPPED,
                            SERVICE_STARTED => SERVICE_STARTED,
                            SERVICE_RESETTED=>SERVICE_RESETTED,
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
    return($smarty->fetch(get_template_path("goImapServer.tpl",TRUE,dirname(__FILE__))));
  }

  function getListEntry()
  {
    $flag = $this->StatusFlag;
    $fields['Status']     = $this->$flag;
    $fields['Message']    = _("Cyrus service");
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
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->dn);
    $ldap->cat($this->dn,array("objectClass","goImapName","goImapConnect","goImapAdmin","goImapPassword"));

    if($ldap->count()){

      $attrs =array();
      foreach(array("goImapName","goImapConnect","goImapAdmin","goImapPassword") as $required){
        if(!isset($attrs[$required])){
          if(empty($this->$required)){
            print_red(_("Can't set new status while there are blank option within generic configuration part."));
            return;
          }else{
            $attrs[$required] = $this->$required;
          }
        }else{
          $attrs[$required] = $attrs[$required][0];
        }
      }

      $tmp = $ldap->fetch();
      for($i = 0; $i < $tmp['objectClass']['count']; $i ++){
        $attrs['objectClass'][] = $tmp['objectClass'][$i];
      }
      if(!in_array("goImapServer",$attrs['objectClass'])){
        $attrs['objectClass'][] = "goImapServer";
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
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
