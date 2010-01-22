<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id$$
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

define("START_SERVICE",1);
define("STOP_SERVICE",2);
define("RESTART_SERVICE",3);

define("SERVICE_STOPPED",_("Stop"));
define("SERVICE_STARTED",_("Start"));
define("SERVICE_RESTARTED",_("Restart"));

define("ALL_SERVICES",100);

class ServerService extends management
{
  /* attribute list for save action */
  var $ignore_account   = TRUE;
  var $attributes       = array();
  var $objectclasses    = array();
  var $skipFooter       = TRUE;
  var $skipHeader       = TRUE;
  var $divList          = NULL;

  var $plugins          = array();
  var $pluign_names     = array();

  var $current          = "";
  var $dialog           = FALSE;
  var $backup           = NULL;
  var $acl              ;
  var $cn;
  var $parent           ;

  var $pl_notify = FALSE;

  function __construct (&$config, $dn,$parent)
  {
    $this->config = $config;
    $this->parent = $parent;
    $this->ui     = get_userinfo();
    $this->dn= $dn;
    if(isset($this->parent->attrs)){
      $this->attrs = $this->parent->attrs;
    }


    // Set storage points - We do not have any - We just create a fake list which lists all services 
    $this->storagePoints = array();

#    // Build filter
 #   if (session::global_is_set(get_class($this)."_filter")){
  #    $filter= session::global_get(get_class($this)."_filter");
   # } else {
      $filter = new filter(get_template_path("serverService-filter.xml", true));
      $filter->setObjectStorage($this->storagePoints);
    #}
    $this->setFilter($filter);

    // Build headpage
    $headpage = new listing(get_template_path("serverService-list.xml", true));
    $headpage->registerElementFilter("filterServiceStatus", "ServerService::filterServiceStatus");
    $headpage->setFilter($filter);

    
    // Initialize list of used and useable services.
    foreach ($config->data['TABS']['SERVERSERVICE'] as $plug){
      if(class_available($plug['CLASS'])){

        $name= $plug['CLASS'];
        $this->plugin_names[]= $name;
        $this->plugins[$name]= new $name($config, $dn, $this);

        /* Initialize kerberos key support */ 
        if(isset($this->plugins[$name]->krb_service_prefix) && !empty($this->plugins[$name]->krb_service_prefix)){
          $this->plugins[$name]->krb_host_key = &$this->parent->kerberos_key_service;
        }

        /* Capture all service objectClases, necessary for acl handling */ 
        if(isset($this->plugins[$name]->objectclasses)){
          foreach($this->plugins[$name]->objectclasses as $oc){
            $this->objectclasses[] = $oc;
          }
        }
      }else{
        #trigger_error("Service class missing: ".$plug['CLASS']);
      }
    }

    parent::__construct($config, $ui, "systems", $headpage);
	}


  static function getServiceList()
  { 
    return(session::get('ServerService'));
  }

  static function filterServiceStatus($a,$b,$c,$d)
  {
    $img ="";
    if(isset($d['0'])){
      switch($d[0]){
        case 'Restart' : $img = ('images/status_restarting.png'); break;
        case 'Stop' : $img = ('images/lists/off.png'); break;
        case 'Start' : $img = ('images/lists/on.png'); break;
      }
    }
    if(!empty($img)){
      return('<img src=\''.$img.'\'>');
    }
    return("");  
  }
  

  function execute()
  {
    /* Variable initialisation */  
    $s_action = "";
    $s_entry  = "";

    /* Walk through posts and check if there are some introductions for us */
    $PossiblePosts = array("addNewService"  => "", 
                           "startServices"  => "",
                           "stopServices"   => "",
                           "restartServices"  => "",
                           "removeServices" => "",

                           "StartSingleService"  => "/^StartSingleService_(.*)_[xy]$/",
                           "StopSingleService"   => "/^StopSingleService_(.*)_[xy]$/",
                           "RestartSingleService"  => "/^RestartSingleService_(.*)_[xy]$/",
                           "RemoveSingleService" => "/^RemoveSingleService_(.*)_[xy]$/",
                           "EditSingleService"   => "/^EditSingleService_(.*)_[xy]$/");

  
    $once = true;
    foreach($_POST as $name => $value){
      foreach($PossiblePosts as $pregCheck => $idPreg) {
        if(preg_match("/^".$pregCheck."/",$name) && $once){
          $once     = false;
          $s_action = $pregCheck;
          
          if(!empty($idPreg)){
            $s_entry = preg_replace($idPreg,"\\1",$name);
          }
        }
      }
    }


    /* Handle state changes for services */
    $map =  array(    "startServices"         => array("type" => START_SERVICE ,   "service" => ALL_SERVICES),
                      "stopServices"          => array("type" => STOP_SERVICE  ,   "service" => ALL_SERVICES),
                      "restartServices"       => array("type" => RESTART_SERVICE , "service" => ALL_SERVICES),                 
                      "StartSingleService"    => array("type" => START_SERVICE ,   "service" => $s_entry),
                      "StopSingleService"     => array("type" => STOP_SERVICE ,    "service" => $s_entry),
                      "RestartSingleService"  => array("type" => RESTART_SERVICE , "service" => $s_entry));
    if(isset($map[$s_action])){
      $type     = $map[$s_action]['type'];
      $service  = $map[$s_action]['service'];
      $this->ServiceStatusUpdate($type,$service);
    }

    /* Handle actions linked via href */
    if(isset($_GET['act']) && $_GET['act'] == "open" && isset($_GET['id'])){
      $id = $_GET['id'];
      if(isset($this->plugins[$id])){
        $s_entry = $id;
        $s_action = "EditSingleService";
      } 
    }

    /* Open service add dialog */
    if($s_action == "addNewService"){
      $this->dialog = new ServiceAddDialog($this->config,$this->dn,$this);
    }


    /* Remove service */
    if($s_action == "RemoveSingleService"){

      /* Create resetted obj */
      $new_obj = new $s_entry($this->config,$this->dn, $this);
      $new_obj -> set_acl_base($this->acl_base);
      $new_obj -> set_acl_category(preg_replace("/\/$/","",$this->acl_category));
      $tmp     = $new_obj->getListEntry();

      if($tmp['AllowRemove']){

        /* Check if we are allowed to remove this service
         */
        $str = $this->plugins[$s_entry]->allow_remove();

        if(empty($str)){
          $this->plugins[$s_entry] = $new_obj;
          $this->plugins[$s_entry]->is_account = false;
        }else{
          msg_dialog::display(_("Error"), $str, ERROR_DIALOG);
        }
      }
    }


    /* Edit a service and make a backup from all attributes, 
       to be able to restore old values after aborting dialog */ 
    if($s_action == "EditSingleService"){
      if($this->plugins[$s_entry]->acl_is_readable("")){
        $this->backup   = get_object_vars($this->plugins[$s_entry]);
        $this->dialog   = $this->plugins[$s_entry];
        $this->current  = $s_entry;
      }
    }

   
    /* Abort service add */
    if(isset($_POST['CancelServiceAdd'])){
      $this->dialog   = FALSE;
      $this->backup   = NULL;
      $this->current  = "";
    }

 
    /* Abort dialog 
       Restore vars with values before editing */
    if(isset($_POST['CancelService']) && !empty($this->current)){
      if($this->backup == NULL){
        $this->plugins[$this->current] = new $this->current($this->config,$this->dn,$this);
        $this->plugins[$this->current]-> set_acl_base($this->acl_base);
        $this->plugins[$this->current]-> set_acl_category(preg_replace("/\/$/","",$this->acl_category));

      }else{
        foreach($this->backup as $name => $value){
          $this->plugins[$this->current]->$name = $value;
        }
      }
      $this->dialog   = FALSE;
      $this->backup   = NULL;
      $this->current  = ""; 
    }
  

    /* Abort dialog */
    if(isset($_POST['SaveService']) && is_object($this->dialog)){
#      $this->dialog->save_object();
      $msgs = $this->dialog->check();
      if(count($msgs)){
        foreach($msgs as $msg){
          msg_dialog::display(_("Error"), $msg, ERROR_DIALOG);
        }
      }else{
        $this->plugins[$this->current] = $this->dialog;
        $tmp  = get_object_vars($this->dialog);
        foreach($tmp as $name => $value){
          $this->plugins[$this->current]->$name = $value;
        }
        $this->current = "";
        $this->dialog = FALSE;
        $this->backup = NULL;
      }
    }


    /* Abort dialog */
    if((isset($_POST['SaveServiceAdd'])) && (!empty($_POST['ServiceName']))){
      $serv = $_POST['ServiceName'];    
      $this->plugins[$serv]->is_account = true;
      $this->dialog  = $this->plugins[$serv];
      $this->current = $serv;
    }


    /* There is currently a subdialog open, display this dialog */
    if(is_object($this->dialog)){
      $add ="";
      if(isset($this->dialog->krb_host_key) && $this->dialog->krb_host_key instanceof krbHostKeys){
        $add = $this->dialog->krb_host_key->save_object_by_prefix($this->dialog->krb_service_prefix);
        $add = $this->dialog->krb_host_key->execute_by_prefix($this->dialog->krb_service_prefix,TRUE);
      }
      return($add.$this->dialog->execute());
    }


    /* Dispaly services overview */
    session::set('ServerService', $this->plugins);
    return(management::execute());
  }


  /* Get all used services 
      CLASSNAME => _($this->plugins[*]->DisplayName);   */
  function getAllUsedServices()
  {
    $ret = array();
    foreach($this->plugins as $name => $obj){
      if($obj->is_account){
        if(isset($obj->DisplayName)){
          $ret[$name] = $obj->DisplayName;
        }else{
          $ret[$name] = $name;
        }
      }
    }
    return($ret);
  }


  /* Get all unused services 
      CLASSNAME => _($this->plugins[*]->DisplayName);  */
  function getAllUnusedServices()
  {
    $tmp = $this->getAllUsedServices();
    $pool_of_ocs =array();
    foreach($tmp as $name => $value){
      if(isset($this->plugins[$name]->conflicts)){
        $pool_of_ocs[]= get_class($this->plugins[$name]);
      }
    }
   
    $ret = array();
    foreach($this->plugins as $name => $obj){

      /* Skip all pluigns that will lead into conflicts */
      $skip = false;
      if(isset($obj->conflicts)){
        foreach($obj->conflicts as $oc){
          if(in_array_ics($oc,$pool_of_ocs)){
            $skip = true;
          }
        }
      }

      /* Only show createable services */
      if(!$obj->acl_is_createable()){
        $skip = true;
      }

      if(!$skip){
        if(isset($obj->DisplayName)){
          $ret[$name] = $obj->DisplayName;
        }else{
          $ret[$name] = $name;
        }
      }
    }
    return($ret);
  }
 
 
  /* This function sets the status var for each used 
     service && calls an external hook if specified in gosa.conf*/
  function ServiceStatusUpdate($method , $service)
  {
    /* Skip if this is a new server */
    if($this->dn == "new"){
      msg_dialog::display(_("Information"), _("Cannot update service status until it has been saved!"), INFO_DIALOG);
      return;
    }

    $action = "";
    if($method == START_SERVICE){
      $action = SERVICE_STARTED;
    }elseif($method== STOP_SERVICE){
      $action = SERVICE_STOPPED;
    }elseif($method == RESTART_SERVICE){
      $action = SERVICE_RESTARTED;
    }else{
      msg_dialog::display(_("Error"), _("Cannot update service status!"), ERROR_DIALOG);
      return;
    }
    
    $caseVars = array("cn","dn");
    if($service == ALL_SERVICES){
      foreach($this->plugins as $name => $obj){
        foreach($caseVars as $var){
          if(isset($this->$var)){
            $this->plugins[$name]->$var = $this->$var;  
          }
        }

        /* check if services can be restarted */
        $map =array(SERVICE_STARTED=> "AllowStart" ,
                    SERVICE_STOPPED => "AllowStop",
                    SERVICE_RESTARTED => "AllowRestart");

        /* get plugins informations, restart/start/stop actions allowed ?*/
        $tmp = $this->plugins[$name]->getListEntry();

        /* Check if given action is allowed for this service */
        if($tmp[$map[$action]] && !empty($this->plugins[$name]->StatusFlag)){
          if($this->plugins[$name]->initially_was_account && $this->plugins[$name]->is_account){
            $this->plugins[$name]->setStatus($action);
          }
        }
      }
    }else{
      foreach($caseVars as $var){
        if(isset($this->$var)){
          $this->plugins[$service]->$var = $this->$var;  
        }
      }
      if($this->plugins[$service]->is_account){
        $this->plugins[$service]->setStatus($action);
      }
    }
  }


  function check()
  {
    $message = plugin::check();
    return $message;
  }


  function save_object()
  {
    foreach($this->plugins as $name => $obj){
      if($obj->is_account){
        $this->plugins[$name]->save_object();
      }
    }
  }


  function remove_from_parent()
  {
    $caseVars = array("cn","dn");
    foreach($this->plugins as $name => $obj){
      foreach($caseVars as $var){
        if(isset($this->$var)){
          $this->plugins[$name]->$var = $this->$var;  
        }
      }
      if($this->plugins[$name]->initially_was_account){
        if(isset($this->plugins[$name]->krb_host_key) && $this->plugins[$name]->krb_host_key instanceof krbHostKeys){
          $this->plugins[$name]->krb_host_key->remove_from_parent_by_prefix($this->plugins[$name]->krb_service_prefix);
        }
        $this->plugins[$name]->remove_from_parent();
      }
    }
  }


  function save()
  {
    $caseVars = array("cn","dn");
    foreach($this->plugins as $name => $obj){

      foreach($caseVars as $var){
        if(isset($this->$var)){
          $this->plugins[$name]->$var = $this->$var;
        }
      }

      if($this->plugins[$name]->is_account){
        $this->plugins[$name]->save();
      }else{
        if($this->plugins[$name]->initially_was_account){
          $this->plugins[$name]->remove_from_parent();
        }
      }
    }
  }

  
  function PrepareForCopyPaste($source)
  {
    plugin::PrepareForCopyPaste($source);

    foreach($this->plugins as $name => $plugin){
      $this->plugins[$name]->PrepareForCopyPaste($source);
    }
  }


  /* Check if all plugins allow a remove ..  */
  function allow_remove()
  {
    foreach($this->plugins as $name => $obj){
      $str = $obj->allow_remove();
      if(!empty($str)){
        return($str);
      }
    }
  }
  

  // Forward plugin ACLs 
  function set_acl_base($base)
  {
    foreach($this->plugins as $name => $obj){
      $this->plugins[$name]->set_acl_base($base);
    }
  }

  // Forward plugin ACLs 
  function set_acl_category($category)
  {
    foreach($this->plugins as $name => $obj){
      $this->plugins[$name]->set_acl_category($category);
    }
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
