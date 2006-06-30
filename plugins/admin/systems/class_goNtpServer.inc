<?php

class goNtpServer extends plugin{

  var $cli_summary      = "This pluign is used within the ServerService Pluign \nand indicates that this server supports NTP service.";
  var $cli_description  = "Some longer text\nfor help";
  var $cli_parameters   = array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* This plugin only writes its objectClass */
  var $objectclasses    = array("goNtpServer");
  var $attributes       = array("goTimeSource");
  var $StatusFlag       = "goNtpServerStatus";

  /* This class can't be assigned twice so it conflicts with itsself */
  var $conflicts        = array("goNtpServer");

  var $DisplayName      = "";
  var $dn               = NULL;
  var $goTimeSource     = array();
  var $goNtpServerStatus= "";
  var $acl;
  var $cn               = "";

  function goNtpServer($config,$dn)
  {
    plugin::plugin($config,$dn);
    $this->DisplayName = _("Time service");

    /* Load arrays */
    $tmp = array();
    if (isset($this->attrs['goTimeSource'])){
      for ($i= 0; $i<$this->attrs['goTimeSource']['count']; $i++){
        $tmp[$this->attrs['goTimeSource'][$i]]= $this->attrs['goTimeSource'][$i];
      }
    }
    $this->goTimeSource= $tmp;
  }


  function execute()
  { 
    $smarty = get_smarty(); 

    /* Here we add a new entry  */
    if(isset($_POST['NewNTPAdd']) && $_POST['NewNTPExport'] != "") {
      $this->goTimeSource[$_POST['NewNTPExport']]= $_POST['NewNTPExport'];
      asort($this->goTimeSource);
    }

    /* Deleting an Entry, is a bit more complicated than adding one*/
    if((isset($_POST['DelNTPEnt'])) && (isset($_POST['goTimeSource']))) {
      foreach ($_POST['goTimeSource'] as $entry){
        if (isset($this->goTimeSource[$entry])){
          unset($this->goTimeSource[$entry]);
        }
      }
    }

    $smarty->assign("goNtpServerACL",chkacl($this->acl,"goNtpServer")) ;
    $smarty->assign("goTimeSource" , $this->goTimeSource); 
    return($smarty->fetch(get_template_path("goNtpServer.tpl",TRUE,dirname(__FILE__))));
  }


  function getListEntry()
  {
    $this->updateStatusState();
    $flag = $this->StatusFlag;
    $fields['Status']      = $this->$flag;
    $fields['Message']    = _("NTP service");
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

    /* Remove status flag, it is not a memeber of 
        this->attributes, so ensure that it is deleted too */
    if(!empty($this->StatusFlag)){
      $this->attrs[$this->StatusFlag] = array();
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
    show_ldap_error($ldap->get_error(), sprintf(_("Removing server services/goNtpServer with dn '%s' failed."),$this->dn));
    $this->handle_post_events("remove");
  }


  function save()
  {
    plugin::save();
    $this->attrs['goTimeSource'] =array();
    foreach($this->goTimeSource as $entry){
      $this->attrs['goTimeSource'][] = $entry;
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
    show_ldap_error($ldap->get_error(), sprintf(_("Saving server services/goNtpServer with dn '%s' failed."),$this->dn));
    if($this->initially_was_account){
      $this->handle_post_events("modify");
    }else{
      $this->handle_post_events("add");
    }
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
      show_ldap_error($ldap->get_error(), sprintf(_("Set status flag for server services/goNtpServer with dn '%s' failed."),$this->dn));
      $this->action_hook();
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

      /* If there are still some %.. in our command, try to fill these with some other class vars */
      if(preg_match("/%/",$command)){
        $attrs = get_object_vars($this);
        foreach($attrs as $name => $value){
          if(!is_string($value)) continue;
          $command= preg_replace("/%$name/", $value, $command);
        }
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


  /* Get updates for status flag */
  function updateStatusState()
  {
    if(empty($this->StatusFlag)) return;

    $attrs = array();
    $flag = $this->StatusFlag;
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->cn);
    $ldap->cat($this->dn,array($flag));
    if($ldap->count()){
      $attrs = $ldap->fetch();
    }
    if(isset($attrs[$flag][0])){
      $this->$flag = $attrs[$flag][0];
    }
  }


  function check(){ return array();}

  function save_object(){;}
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
