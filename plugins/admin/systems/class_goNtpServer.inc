<?php

require_once("class_goService.inc");

class goNtpServer extends goService{

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
    if(isset($_POST['NewNTPAdd']) && $_POST['NewNTPExport'] != "" && $this->acl_is_writeable("goTimeSource")) {
      $this->goTimeSource[$_POST['NewNTPExport']]= $_POST['NewNTPExport'];
      asort($this->goTimeSource);
    }

    /* Deleting an Entry, is a bit more complicated than adding one*/
    if((isset($_POST['DelNTPEnt'])) && (isset($_POST['goTimeSource'])) && $this->acl_is_writeable("goTimeSource")) {
      foreach ($_POST['goTimeSource'] as $entry){
        if (isset($this->goTimeSource[$entry])){
          unset($this->goTimeSource[$entry]);
        }
      }
    }

    $smarty->assign("goNtpServerACL", $this->getacl("goNtpServer")) ;
    $smarty->assign("goTimeSource" , $this->goTimeSource); 
    return($smarty->fetch(get_template_path("goNtpServer.tpl",TRUE,dirname(__FILE__))));
  }


  function getListEntry()
  {
    $fields               = goService::getListEntry();
    $fields['Message']    = _("NTP service");
    $fields['AllowEdit']  = true;
    return($fields);
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


  /* Return plugin informations for acl handling */
  function plInfo()
  {
    return (array(
          "plShortName"   => _("NTP"),
          "plDescription" => _("NTP service"),
          "plSelfModify"  => FALSE,
          "plDepends"     => array(),
          "plPriority"    => 0,
          "plSection"     => array("administration"),
          "plCategory"    => array("server"),

          "plProvidedAcls"=> array(
            "goTimeSource"        =>_("Ntp source"))
          ));
  }


  function check(){ return array();}

  function save_object(){;}
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
