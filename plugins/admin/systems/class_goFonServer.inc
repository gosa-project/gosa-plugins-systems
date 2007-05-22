<?php

require_once("class_goService.inc");

class goFonServer extends goService{
	
  var $cli_summary      = "This pluign is used within the ServerService Pluign \nand indicates that this server supports asterisk management.";
  var $cli_description  = "Some longer text\nfor help";
  var $cli_parameters   = array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* This plugin only writes its objectClass */
  var $objectclasses    = array("goFonServer");
  var $attributes       = array("goFonAdmin", "goFonPassword","goFonAreaCode", "goFonCountryCode");
  var $StatusFlag       = "goFonServerStatus";
 
  /* This class can't be assigned twice so it conflicts with itsself */
  var $conflicts        = array("goFonServer");

  var $DisplayName      = "";
  var $dn               = NULL;
  var $cn               ="";
  var $acl;

  var $goFonServerStatus  = "";
  var $goFonPassword      = "";
  var $goFonAdmin         = "";
  var $goFonAreaCode      = "";
  var $goFonCountryCode   = "";
  var $view_logged  =FALSE;
   
 
  function goFonServer($config,$dn)
  {
    goService::goService($config,$dn);
    $this->DisplayName = _("VoIP service");
  }


  function execute()
  { 
    if($this->is_account && !$this->view_logged){
      $this->view_logged = TRUE;
      @log::log("view","server/".get_class($this),$this->dn);
    }

    $smarty = get_smarty(); 
    foreach($this->attributes as $attr){
      $smarty->assign($attr,$this->$attr);
    }

    /* Assign acls */
    $tmp = $this->plInfo();
    foreach($tmp['plProvidedAcls'] as $name => $translation){
      $smarty->assign($name."ACL",$this->getacl($name));
    }

    return($smarty->fetch(get_template_path("goFonServer.tpl",TRUE,dirname(__FILE__))));
  }


  function getListEntry()
  {
    $fields = goService::getListEntry();
    $fields['Message']    = _("VoIP service - Asterisk management");
    $fields['AllowEdit']  = true;
    return($fields);
  }

  function check()
  { 
    $message = plugin::check();
    if (empty($this->goFonAdmin)){
      $message[]= _("The attribute DB user is empty or contains invalid characters.");
    }
    if (empty($this->goFonPassword)){
      $message[]= _("The attribute password is empty or contains invalid characters.");
    }
    if (empty($this->goFonAreaCode)){
      $message[]= _("The attribute local dial prefix is empty or contains invalid characters.");
    }
    if (empty($this->goFonCountryCode)){
      $message[]= _("The attribute country dial prefix is empty or contains invalid characters.");
    }
    return($message);
  }
  

  function save_object()
  {
    if(isset($_POST['goFonServerPosted'])){
      plugin::save_object();
    }
  } 


  function allow_remove()
  {
    /* Check if we are able to remove the asterisk database. If the database is still in use skip */
    if($this->is_account){
      $ldap = $this->config->get_ldap_link();
      $ldap->cd($this->config->current['BASE']);
      $ldap->search("(&(goFonHomeServer=".$this->dn.")(|(uid=*)(cn=*)))",array("dn","uid","cn","goFonHomeServer"));
      if($ldap->count()){

        /* Number of entries shown in warning */
        $i    = 3;
        $str  = "";
        while(($attrs = $ldap->fetch()) && $i >= 0){
          $i --;
          if(isset($attrs['uid'][0])){
            $str .= $attrs['uid'][0]." ";
          }else{
            $str .= $attrs['cn'][0]." ";
          }
        }

        /* Some entries found */
        if($i != 3){
          return(sprintf(_("You can't remove the asterisk database extension, it is still in use by these objects '%s'."),trim($str)));
        }
      }
    }
  }

 
  /* Return plugin informations for acl handling */
  function plInfo()
  {
    return (array(
          "plShortName"   => _("VoIP service"),
          "plDescription" => _("VoIP - asterisk management")." ("._("Services").")",
          "plSelfModify"  => FALSE,
          "plDepends"     => array(),
          "plPriority"    => 92,
          "plSection"     => array("administration"),
          "plCategory"    => array("server"),

          "plProvidedAcls"=> array(
            "start"         => _("Start"),
            "stop"          => _("Stop"),
            "restart"       => _("Restart"),

              "goFonAdmin"      => _("Admin"), 
              "goFonPassword"   => _("Password"),
              "goFonAreaCode"   => _("Area code"), 
              "goFonCountryCode"=> _("Country code"))
          ));
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
