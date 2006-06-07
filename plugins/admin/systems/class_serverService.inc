<?php

define("START_SERVICE",1);
define("STOP_SERVICE",2);
define("RESET_SERVICE",3);

define("SERVICE_STOPPED","stopped");
define("SERVICE_STARTED","started");
define("SERVICE_RESETTED","resetted");

define("ALL_SERVICES",100);

class ServerService extends plugin
{
  /* CLI vars */
  var $cli_summary      = "Manage server services";
  var $cli_description  = "Managing services by adding,removing and configuring services. Allows to start/stop used services.";
  var $cli_parameters   = array("config"=>"Config object" , "dn"=>"Object dn");

  /* attribute list for save action */
  var $ignore_account   = TRUE;
  var $attributes       = array();
  var $objectclasses    = array();

  var $divList          = NULL;
  var $dialog           = NULL;       // Contains dialog object if a dialog is opened 

  var $plugins          = array();
  var $pluign_names     = array();

  var $current          = "";
  var $backup           = NULL;
  var $acl              ;

  function ServerService ($config, $dn)
  {
    plugin::plugin($config);
    $this->dn= $dn;
    $ui= get_userinfo();
    $this->acl= get_permissions ($ui->dn, $ui->subtreeACL);

    foreach ($config->data['TABS']['SERVERSERVICE'] as $plug){
      $name= $plug['CLASS'];
      $this->plugin_names[]= $name;
      $this->plugins[$name]= new $name($config, $dn);
      $this->plugins[$name]->acl= get_module_permission($this->acl, "$name", $ui->dn);
    }
    
    $this->divList = new divListSystemService($config,$this);
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
                           "resetServices"  => "",
                           "removeServices" => "",

                           "StartSingleService"  => "/^StartSingleService_(.*)_[xy]$/",
                           "StopSingleService"   => "/^StopSingleService_(.*)_[xy]$/",
                           "ResetSingleService"  => "/^ResetSingleService_(.*)_[xy]$/",
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
    $map =  array( "startServices"          => array("type" => START_SERVICE , "service" => ALL_SERVICES),
                      "stopServices"          => array("type" => STOP_SERVICE  , "service" => ALL_SERVICES),
                      "resetServices"         => array("type" => RESET_SERVICE , "service" => ALL_SERVICES),
                      "StartSingleService"    => array("type" => START_SERVICE , "service" => $s_entry),
                      "StopSingleService"     => array("type" => STOP_SERVICE , "service" => $s_entry),
                      "ResetSingleService"    => array("type" => RESET_SERVICE , "service" => $s_entry));
    if(isset($map[$s_action])){
      $type     = $map[$s_action]['type'];
      $service  = $map[$s_action]['service'];
      $this->ServiceStatusUpdate($type,$service);
    }


    /* Open service add dialog */
    if($s_action == "addNewService"){
      $this->dialog = new ServiceAddDialog($this->config,$this->dn,$this);
    }


    /* Remove service */
    if($s_action == "RemoveSingleService"){
      $this->plugins[$s_entry]->is_account= false;
      $this->plugins[$s_entry] = NULL;
      $this->plugins[$s_entry] = new $s_entry($this->config,$this->dn);
      $this->plugins[$s_entry]->acl = $this->acl;
      $this->plugins[$s_entry]->is_account = false;
    }


    /* Edit a service and make a backup from all attributes, 
       to be able to restore old values after aborting dialog */ 
    if($s_action == "EditSingleService"){
      $this->backup   = get_object_vars($this->plugins[$s_entry]);
      $this->dialog   = $this->plugins[$s_entry];
      $this->current  = $s_entry;
    }

   
    /* Abort service add */
    if(isset($_POST['CancelServiceAdd'])){
      $this->dialog   = NULL;
      $this->backup   = NULL;
      $this->current  = "";
    }

 
    /* Abort dialog 
       Restore vars with values before editing */
    if(isset($_POST['CancelService'])){
      if($this->backup == NULL){
        $this->plugins[$this->current] = new $this->current($this->config,$this->dn);
      }else{
        foreach($this->backup as $name => $value){
          $this->plugins[$this->current]->$name = $value;
        }
      }
      $this->dialog   = NULL;
      $this->backup   = NULL;
      $this->current  = ""; 
    }
  

    /* Abort dialog */
    if(isset($_POST['SaveService'])){

      $msgs = $this->dialog->check();
      if(count($msgs)){
        foreach($msgs as $msg){
          print_red($msg);
        }
      }else{
        $this->plugins[$this->current] = $this->dialog;
        $this->current = "";
        $this->dialog = NULL;
        $this->backup = NULL;
      }
    }


    /* Abort dialog */
    if(isset($_POST['SaveServiceAdd'])){
      $serv = $_POST['ServiceName'];    
      $this->plugins[$serv]->is_account = true;
      $this->dialog  = $this->plugins[$serv];
      $this->current = $serv;
    }


    /* There is currently a subdialog open, display this dialog */
    if($this->dialog != NULL){
      $this->dialog->save_object();
      return($this->dialog->execute());
    }


    /* Dispaly services overview */
    $this->divList->execute();
    $list = array();
    foreach($this->plugins as $name => $obj){
      if($obj->is_account){
        $list[$name] = $obj->getListEntry(); 
      }
    }
    $this->divList -> setEntries($list);
    return($this->divList->Draw());
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
        $pool_of_ocs = array_merge($pool_of_ocs,$this->plugins[$name]->conflicts);
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
      print_red(_("Can't set status while this server is not saved."));
      return;
    }

    $action = "";
    if($method == START_SERVICE){
      $action = SERVICE_STARTED;
    }elseif($method== STOP_SERVICE){
      $action = SERVICE_STOPPED;
    }elseif($method == RESET_SERVICE){
      $action = SERVICE_RESETTED;
    }else{
      print_red(sprintf(_("The specified method '%s' can't executed for services."),$action));
      return;
    }
    
    if($service == ALL_SERVICES){
      foreach($this->plugins as $name => $obj){
        if($this->plugins[$name]->is_account){
          $this->plugins[$name]->setStatus($action);
        }
      }
    }else{
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
  
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
