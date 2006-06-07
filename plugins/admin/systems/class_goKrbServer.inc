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
    $this->DisplayName = _("Kerberos kadmin access informations");
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
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->dn);
    $ldap->cat($this->dn,array("objectClass","goKrbRealm","goKrbAdmin","goKrbPassword"));
      
    if($ldap->count()){
      $attrs =array();
      foreach(array("goKrbRealm","goKrbAdmin","goKrbPassword") as $req) {
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
      if(!in_array("goKrbServer",$attrs['objectClass'])){
        $attrs['objectClass'][] = "goKrbServer";
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
    foreach (array("goKrbAdmin", "goKrbRealm") as $attr){
        if ($this->$attr == "" || preg_match("/ /", $this->$attr)){
          $message[]= sprintf(_("The attribute '%s' is empty or contains invalid characters."), $attr);
        }
      }
      /* goKrbPassword is a must field, if goKrbServer is used as objectClass */
      if ($this->goKrbPassword == "" ){
        $message[]= sprintf(_("The specified kerberos password is empty."), $attr);
      }

    return($message);
  }
  

  function save_object()
  {
    if(isset($_POST['goKrbServerPosted'])){
      plugin::save_object();
    }
  }  
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
