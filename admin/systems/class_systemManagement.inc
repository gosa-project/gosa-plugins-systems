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

class systemManagement extends management
{
  var $plHeadline     = "Systems";
  var $plDescription  = "List of systems";
  var $plIcon  = "plugins/systems/images/plugin.png";

  // Tab definition 
  protected $tabClass = "";
  protected $tabType = "";
  protected $aclCategory = "";
  protected $aclPlugin   = "";
  protected $objectName   = "";

  protected $objectInfo = array();

  protected $opsi = NULL;


  protected $activationQueue  = array();
  
  function __construct($config,$ui)
  {
    $this->config = $config;
    $this->ui = $ui;

    // Set storage points 
    $tD = $this->getObjectDefinitions(); 
    $sP = array();
    foreach($tD as $entry){
      if(!empty($entry['ou']))
        $sP[] = $entry['ou'];
    }
    $this->storagePoints = array_unique($sP);

    // Build filter
#    if (session::global_is_set(get_class($this)."_filter")){
#      $filter= session::global_get(get_class($this)."_filter");
#    } else {
      $filter = new filter(get_template_path("system-filter.xml", true));
      $filter->setObjectStorage($this->storagePoints);
#    }
    $this->setFilter($filter);

    // Build headpage
    $headpage = new listing(get_template_path("system-list.xml", true));
    $headpage->setFilter($filter);
    $filter->setConverter('INCOMING', 'systemManagement::incomingFilterConverter');

    // Register Daemon Events
    if(class_available("DaemonEvent") && class_available("gosaSupportDaemon")){
      $events = DaemonEvent::get_event_types(SYSTEM_EVENT | HIDDEN_EVENT);
      foreach($events['TRIGGERED'] as $name => $data){
        $this->registerAction("T_".$name,"handleEvent");
        $this->registerAction("S_".$name,"handleEvent");
      }
      $this->registerAction("activateMultiple","activateMultiple");
    }
    $this->registerAction("saveEvent","saveEventDialog");
    $this->registerAction("createISO","createISO");
    $this->registerAction("initiateISOcreation","initiateISOcreation");
    $this->registerAction("performIsoCreation","performIsoCreation");
    $this->registerAction("systemTypeChosen","systemTypeChosen");
    $this->registerAction("handleActivationQueue","handleActivationQueue");

    // Add copy&paste and snapshot handler.
    if ($this->config->boolValueIsTrue("main", "copyPaste")){
      $this->cpHandler = new CopyPasteHandler($this->config);
    }
    if($this->config->get_cfg_value("enableSnapshots") == "true"){
      $this->snapHandler = new SnapshotHandler($this->config);
    }

    // Check if we are able to communicate with the GOsa supprot daemon
    if(class_available("gosaSupportDaemon")){
      $o = new gosaSupportDaemon();
      $this->si_active = $o->connect() && class_available("DaemonEvent");
    }

    // Check if we are able to communicate with the GOsa supprot daemon
    if(class_available("opsi")){
      $this->opsi = new opsi($this->config);
    }


    parent::__construct($config, $ui, "systems", $headpage);
  }


  function createISO($action,$target)
  {
    if(count($target) == 1){
      $smarty = get_smarty();
      $this->dn= array_pop($target);
      set_object_info($this->dn);
      return ($smarty->fetch(get_template_path('goto/gencd.tpl', TRUE)));

    }
  }
  
  function initiateISOcreation()
  {
    $smarty = get_smarty();
    $smarty->assign("src", "?plug=".$_GET['plug']."&amp;PerformIsoCreation");
    return ($smarty->fetch(get_template_path('goto/gencd_frame.tpl', TRUE)));  
  }


  function performIsoCreation()
  {
    $return_button   = "<form method='get' action='main.php' target='_parent'>
      <input type='submit' value='"._("Back")."'>
      <input type='hidden' name='plug' value='".$_GET['plug']."'/>
      </form>";

    $dsc   = array(0 => array("pipe", "r"), 1 => array("pipe", "w"), 2 => array("pipe", "w"));

    /* Get and check command */
    $command= $this->config->search("workgeneric", "SYSTEMISOHOOK",array('tabs'));
    if (check_command($command)){
      @DEBUG (DEBUG_SHELL, __LINE__, __FUNCTION__, __FILE__, $command, "Execute");

      /* Print out html introduction */
      echo '  <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2//EN">
        <html>
        <head>
        <title></title>
        <style type="text/css">@import url("themes/default/style.css");</style>
        <script language="javascript" src="include/focus.js" type="text/javascript"></script>
        </head>
        <body style="background: none; margin:4px;" id="body" >
        <pre>';

      /* Open process handle and check if it is a valid process */
      $process= proc_open($command." '".$this->dn."'", $dsc, $pipes);
      if (is_resource($process)) {
        fclose($pipes[0]);

        /* Print out returned lines && write JS to scroll down each line */
        while (!feof($pipes[1])){
          $cur_dat = fgets($pipes[1], 1024);
          echo $cur_dat;
          echo '<script language="javascript" type="text/javascript">scrollDown2();</script>' ;
          flush();
        }
      }

      /* Get error string && close streams */
      $buffer= stream_get_contents($pipes[2]);

      fclose($pipes[1]);
      fclose($pipes[2]);
      echo "</pre>";

      /* Check return code */
      $ret= proc_close($process);
      if ($ret != 0){
        echo "<h1 style='color:red'>"._("Creating the image failed. Please see the report below.")."</h1>";
        echo "<pre style='color:red'>$buffer</pre>";
      }
      echo $return_button."<br>";
    } else {
      $tmp= "<h1 style='color:red'>".sprintf(_("Command '%s', specified for ISO creation doesn't seem to exist."), $command)."</h1>";
      echo $tmp;
      echo $return_button."<br>";
    }

    /* Scroll down completly */
    echo '<script language="javascript" type="text/javascript">scrollDown2();</script>' ;
    echo '</body></html>';
    flush();
    exit;
  }


  /*! \brief    Handle GOsa-si events
   *            All events are handled here.
   */
  function handleEvent($action="",$target=array(),$all=array())
  {
    // Detect whether this event is scheduled or triggered.
    $triggered = TRUE;
    if(preg_match("/^S_/",$action)){
      $triggered = FALSE;
    }

    // Detect triggere or scheduled actions 
    $headpage = $this->getHeadpage();
    if(preg_match("/^[TS]_/", $action)){

      // Send special reinstall action for opsi hosts
      $event = preg_replace("/^[TS]_/","",$action); 
      if($event == "DaemonEvent_reinstall" && $this->si_active && $this->opsi){
        foreach($target as $key => $dn){
          $type = $headpage->getType($dn);

          // Send Reinstall event for opsi hosts
          if($type == "FAKE_OC_OpsiHost"){
            $obj = $headpage->getEntry($dn);
            $this->opsi->job_opsi_install_client($obj['cn'][0],$obj['macAddress'][0]);
            unset($target[$key]);
          }
        }
      }
    } 

    // Now send remaining FAI/GOsa-si events here.
    if(count($target) && $this->si_active){
      $mac= array();

      // Collect target mac addresses
      $ldap = $this->config->get_ldap_link();
      $tD = $this->getObjectDefinitions();
      $events = DaemonEvent::get_event_types(SYSTEM_EVENT);
      $o_queue = new gosaSupportDaemon();
      foreach($target as $dn){
        $type = $headpage->getType($dn);
        if($tD[$type]['sendEvents']){
          $obj = $headpage->getEntry($dn);
          if(isset($obj['macAddress'][0])){
            $mac[] = $obj['macAddress'][0];
          }
        }
      }

      /* Skip installation or update trigerred events,
       *  if this entry is currently processing.
       */
      if($triggered && in_array($event,array("DaemonEvent_reinstall","DaemonEvent_update"))){
        foreach($mac as $key => $mac_address){
          foreach($o_queue->get_entries_by_mac(array($mac_address)) as $entry){
            $entry['STATUS'] = strtoupper($entry['STATUS']);
            if($entry['STATUS'] == "PROCESSING" &&
                isset($events['QUEUED'][$entry['HEADERTAG']]) &&
                in_array($events['QUEUED'][$entry['HEADERTAG']],array("DaemonEvent_reinstall","DaemonEvent_update"))){
              unset($mac[$key]);

              new log("security","systems/".get_class($this),"",array(),"Skip adding 'DaemonEvent::".$type."' for mac '".$mac_address."', there is already a job in progress.");
              break;
            }
          }
        }
      }

      // Prepare event to be added
      if(count($mac) && isset($events['BY_CLASS'][$event]) && $this->si_active){
        $event = $events['BY_CLASS'][$event];
        $this->dialogObject = new $event['CLASS_NAME']($this->config);
        $this->dialogObject->add_targets($mac);

        if($triggered){
          $this->dialogObject->set_type(TRIGGERED_EVENT);
          $o_queue->append($this->dialogObject);
          if($o_queue->is_error()){
            msg_dialog::display(_("Service infrastructure"),msgPool::siError($o_queue->get_error()),ERROR_DIALOG);
          }else{
            $this->closeDialogs();
          }
        }else{
          $this->dialogObject->set_type(SCHEDULED_EVENT);
        }
      }
    }
  }

  function cancelEdit()
  {
    management::cancelEdit();
    $this->activationQueue = array();
  }
  
  
  function saveEventDialog()
  {
    $o_queue = new gosaSupportDaemon();
    $o_queue->append($this->dialogObject);
    if($o_queue->is_error()){
      msg_dialog::display(_("Service infrastructure"),msgPool::siError($o_queue->get_error()),ERROR_DIALOG);
    }else{
      $this->closeDialogs();
    }
 
  }
 

  /*! \brief    Update filter part for INCOMING.
   *            Allows us to search for "systemIncomingRDN".
   */
  static function incomingFilterConverter($filter)
  {
    $rdn = preg_replace("/^[^=]*=/", "", get_ou('systemIncomingRDN'));
    $rdn = preg_replace("/,.*$/","",$rdn);
    return(preg_replace("/%systemIncomingRDN/", $rdn,$filter));
  }

 
  /*! \brief    Queue selected objects to be removed. 
   *            Checks ACLs, Locks and ask for confirmation.
   */
  protected function removeEntryRequested($action="",$target=array(),$all=array())
  {
    $disallowed = array();
    $this->dns = array();

    @DEBUG (DEBUG_TRACE, __LINE__, __FUNCTION__, __FILE__,$target,"Entry removel requested!");

    // Check permissons for each target
    $tInfo = $this->getObjectDefinitions();
    $headpage = $this->getHeadpage();
    foreach($target as $dn){
      $type = $headpage->getType($dn);
      if(!isset($tInfo[$type])){
        trigger_error("Unknown object type received '".$type."' please update systemManagement::getObjectDefinitions()!");
      }else{
        $info = $tInfo[$type];
        $acl = $this->ui->get_permissions($dn, $info['aclCategory']."/".$info['aclClass']);
        if(preg_match("/d/",$acl)){
          $this->dns[] = $dn;
        }else{
          $disallowed[] = $dn;
        }
      }
    }
    if(count($disallowed)){
      msg_dialog::display(_("Permission"),msgPool::permDelete($disallowed),INFO_DIALOG);
    }

    // We've at least one entry to delete.
    if(count($this->dns)){

      // check locks
      if ($user= get_multiple_locks($this->dns)){
        return(gen_locked_message($user,$this->dns));
      }

      // Add locks
      $dns_names = array();
      foreach($this->dns as $dn){
        $dns_names[] =LDAP::fix($dn);
      }
      add_lock ($this->dns, $this->ui->dn);

      // Display confirmation dialog.
      $smarty = get_smarty();
      $smarty->assign("info", msgPool::deleteInfo($dns_names,_($this->objectName)));
      $smarty->assign("multiple", true);
      return($smarty->fetch(get_template_path('remove.tpl', TRUE)));
    }
  }


  /*! \brief  Object removal was confirmed, now remove the requested entries.
   *
   *  @param  String  'action'  The name of the action which was the used as trigger.
   *  @param  Array   'target'  A list of object dns, which should be affected by this method.
   *  @param  Array   'all'     A combination of both 'action' and 'target'.
   */
  function removeEntryConfirmed($action="",$target=array(),$all=array(),
      $altTabClass="",$altTabType="",$altAclCategory="")
  {
    @DEBUG (DEBUG_TRACE, __LINE__, __FUNCTION__, __FILE__,$target,"Entry removel confirmed!");

    // Check permissons for each target
    $tInfo = $this->getObjectDefinitions();
    $headpage = $this->getHeadpage();
    $disallowed = array();
    foreach($this->dns as $key => $dn){
      $type = $headpage->getType($dn);
      if(!isset($tInfo[$type])){
        trigger_error("Unknown object type received '".$type."' please update systemManagement::getObjectDefinitions()!");
      }else{

        $info = $tInfo[$type];
        $acl = $this->ui->get_permissions($dn, $info['aclCategory']."/".$info['aclClass']);
        if(preg_match("/d/",$acl)){

          // Delete the object
          $this->dn = $dn;
          $this->tabObject= new $info['tabClass']($this->config,$this->config->data['TABS'][$info['tabDesc']], 
              $this->dn, $info['aclCategory'], true, true);
          $this->tabObject->set_acl_base($this->dn);
          $this->tabObject->parent = &$this;
          $this->tabObject->delete ();

          // Remove the lock for the current object.
          del_lock($this->dn);

        }else{
          $disallowed[] = $dn;
          new log("security","groups/".get_class($this),$dn,array(),"Tried to trick deletion.");
        }
      }
    }
    if(count($disallowed)){
      msg_dialog::display(_("Permission"),msgPool::permDelete($disallowed),INFO_DIALOG);
    }

    // Cleanup
    $this->remove_lock();
    $this->closeDialogs();
  }


  /*! \brief  Edit the selected system type.
   *
   *  @param  String  'action'  The name of the action which was the used as trigger.
   *  @param  Array   'target'  A list of object dns, which should be affected by this method.
   *  @param  Array   'all'     A combination of both 'action' and 'target'.
   */
  function editEntry($action="",$target=array(),$all=array(), $altTabClass ="", $altTabType = "", $altAclCategory="")
  {
    if(count($target) == 1){
      $tInfo = $this->getObjectDefinitions();
      $headpage = $this->getHeadpage();
      $dn = $target[0];
      $type =$headpage->getType($dn);
      $tData = $tInfo[$type];

      if($type == "FAKE_OC_ArpNewDevice"){
        if(!class_available("ArpNewDeviceTabs")){
          msg_dialog::display(_("Error"), msgPool::class_not_found("ArpNewDevice"), ERROR_DIALOG);
        }else{
          $this->system_activation_object= array($dn);
          return(management::editEntry($action,$target,$all,"ArpNewDeviceTabs","ARPNEWDEVICETABS","incoming"));
        }
      }elseif($type == "FAKE_OC_NewDevice"){
        if(!class_available("SelectDeviceType")){
          msg_dialog::display(_("Error"), msgPool::class_not_found("SelectDeviceType"), ERROR_DIALOG);
        }else{
          $this->system_activation_object= array($dn);
          $this->dialogObject = new SelectDeviceType($this->config,$dn);
          $this->skipFooter = TRUE;
          $this->displayApplyBtn = FALSE;
          // see condition  -$s_action == "::systemTypeChosen"-  for further handling
        }
      }else{
         return(management::editEntry($action,$target,$all,$tData['tabClass'],$tData['tabDesc'],$tData['aclCategory']));
      }
    }
  }


  function activateMultiple($action,$target)
  {
    $headpage = $this->getHeadpage();
    foreach($target as $dn) {
      if($headpage->getType($dn) == "FAKE_OC_NewDevice"){
        $this->activationQueue[$dn] = array();
      }
    }
    if(count($this->activationQueue)){
      $this->dialogObject = new SelectDeviceType($this->config, array_keys($this->activationQueue));
      $this->skipFooter = TRUE;
    }
  }




  function systemTypeChosen()
  {
    // Detect the systems target type 
    $tInfo = $this->getObjectDefinitions();
    $selected_group = "none";
    if(isset($_POST['ObjectGroup'])){
      $selected_group = $_POST['ObjectGroup'];
    }
    $selected_system = $_POST['SystemType'];
    $tmp = array();
    foreach($this->activationQueue as $dn => $data){
      $tmp[$dn]['OG'] = $selected_group;
      $tmp[$dn]['SS'] = $selected_system;
    }
    $this->closeDialogs();
    $this->activationQueue = $tmp;
    return($this->handleActivationQueue());
  }


  function handleActivationQueue()
  {
    if(!count($this->activationQueue)) return("");

    $ldap     = $this->config->get_ldap_link();
    $pInfo    = $this->getObjectDefinitions(); 
    $ui       = get_userinfo();
    $headpage = $this->getHeadpage();
    $ldap->cd($this->config->current['BASE']);

    // Walk through systems to activate
    foreach($this->activationQueue as $dn => $data){

      if(!isset($data['SS'])) continue;

      $sysType = $data['SS'];
      $type = $pInfo[$sysType];

      // Get target type definition 
      $plugClass    = $type["plugClass"];
      $tabClass     = $type["tabClass"];
      $aclCategory  = $type["aclCategory"];
      $tabDesc      = $type["tabDesc"];

      if(!class_available($tabClass)){
        msg_dialog::display(_("Error"), msgPool::class_not_found($tabclass), ERROR_DIALOG);
      }else{

        // Load permissions for selected 'dn' and check if we're allowed to create this 'dn' 
        $this->dn = $dn;
        $acls   = $ui->get_permissions($this->dn,$aclCategory."/".$plugClass);

        // Check permissions
        if(!preg_match("/c/",$acls)){
          unset($this->activationQueue[$dn]);
          msg_dialog::display(_("Error"), msgPool::permCreate(), ERROR_DIALOG);
          continue;
        }else{

          // Open object an preset some values like the objects base 
          del_lock($dn);
          management::editEntry('editEntry',array($dn),array(),$tabClass,$tabDesc, $aclCategory);
          $this->tabObject->set_acl_base($headpage->getBase());
          if($data['OG'] != "none"){
            $this->tabObject->base = preg_replace("/^[^,]+,".preg_quote(get_ou('ogroupRDN'), '/')."/i", "", $data['OG']);
            $this->tabObject->by_object[$plugClass]->base = $this->tabObject->base;
          } else {
            $this->tabObject->by_object[$plugClass]->base = $headpage->getBase();
            $this->tabObject->base = $headpage->getBase();
          }

          // Assign some default values for opsi hosts
          if($this->tabObject instanceOf opsi_tabs){
            $ldap = $this->config->get_ldap_link();
            $ldap->cat($dn);
            $source_attrs = $ldap->fetch();
            foreach(array("macAddress" => "mac" ,"cn" => "hostId","description" => "description") as $src => $attr){
              if(isset($source_attrs[$src][0])){
                $this->tabObject->by_object['opsiGeneric']->$attr = $source_attrs[$src][0];
              }
            }
          }

          // Queue entry to be activated, when it is saved.
          if($data['OG'] != "none"){

            // Set gotoMode to active if there was an ogroup selected.
            $found = false;
            foreach(array("workgeneric"=>"active","servgeneric"=>"active","termgeneric"=>"active") as $tab => $value){
              if(isset($this->tabObject->by_object[$tab]->gotoMode)) {
                $found = true;
                $this->tabObject->by_object[$tab]->gotoMode = $value;
              }
            }
            if(!$found){
              msg_dialog::display(_("Internal error"), _("Cannot set mode to 'active'!"), ERROR_DIALOG);
            }

            // Update object group membership
            $og = new ogroup($this->config,$data['OG']);
            if($og){
              $og->AddDelMembership($this->tabObject->dn);
              $og->save();
            }

            // Set default system specific attributes
            foreach (array("workgeneric", "termgeneric") as $cls){
              if (isset($this->tabObject->by_object[$cls])){
                $this->tabObject->by_object[$cls]->set_everything_to_inherited();
              }
            }

            // Enable activation
            foreach (array("servgeneric", "workgeneric", "termgeneric") as $cls){
              if (isset($this->tabObject->by_object[$cls])){
                $this->tabObject->by_object[$cls]->auto_activate= TRUE;
              }
            }

            // Enable sending of LDAP events
            if (isset($this->tabObject->by_object["workstartup"])){
              $this->tabObject->by_object["workstartup"]->gotoLdap_inherit= TRUE;
            }
          }

          // Try to inherit everythin from the selected object group and then save
          //  the entry, normally this should work without any problems. 
          // But if there is any, then display the dialogs.
          if($data['OG'] != "none"){
            $str = $this->saveChanges();

            // There was a problem, skip activation here and allow to fix the problems..
            if(is_object($this->tabObject)){
              return;
            }
          }else{
            return;
          }
        }
      }
    }
  }


  protected function saveChanges()
  {
    $str = management::saveChanges();
    if($this->tabObject) return("");

    if(isset($this->activationQueue[$this->last_dn])){
      $this->activate_new_device($this->last_dn);
      unset($this->activationQueue[$this->last_dn]);
    }
    $this->handleActivationQueue();
  }


  /*! \brief  Sets FAIstate to "install" for "New Devices".
    This function is some kind of "Post handler" for activated systems,
    it is called directly after the object (workstabs,servtabs) gets saved.
    @param  String  $dn   The dn of the newly activated object.
    @return Boolean TRUE if activated else FALSE
   */
  function activate_new_device($dn)
  {
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    $ldap->cat($dn);
    if($ldap->count()){
      $attrs = $ldap->fetch();
      if(count(array_intersect(array('goServer','gotoTerminal'), $attrs['objectClass']))){
        $ocs = $attrs['objectClass'];
        unset($ocs['count']);
        $new_attrs = array();
        if(!in_array("FAIobject",$ocs)){
          $ocs[] = "FAIobject";
          $new_attrs['objectClass'] = $ocs;
        }
        $new_attrs['FAIstate'] = "install";
        $ldap->cd($dn);
        $ldap->modify($new_attrs);
        if (!$ldap->success()){
          msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn,
                LDAP_MOD, "activate_new_device($dn)"));
        }else{
          return(TRUE);
        }
      }
    }
    return(FALSE);
  }



 /*! \brief  Detects actions/events send by the ui
   *           and the corresponding targets.
   */
  function detectPostActions()
  {
    $action= management::detectPostActions();
    if(isset($_POST['abort_event_dialog']))  $action['action'] = "cancel";
    if(isset($_POST['save_event_dialog']))  $action['action'] = "saveEvent";
    if(isset($_POST['cd_create']))  $action['action'] = "initiateISOcreation";
    if(isset($_GET['PerformIsoCreation']))  $action['action'] = "performIsoCreation";
    if(isset($_POST['SystemTypeAborted']))  $action['action'] = "cancel";

    if(!is_object($this->tabObject) && !is_object($this->dialogObject)){
      if(count($this->activationQueue)) $action['action'] = "handleActivationQueue";
    }

    if(isset($_POST['systemTypeChosen']))  $action['action'] = "systemTypeChosen";

    return($action);
  }


  /*! \brief   Overridden render method of class management.
   *            this allows us to add a release selection box.
   */
  function renderList()
  {
    $headpage = $this->getHeadpage();
    $headpage->update();

    $tD = $this->getObjectDefinitions();
    $smarty = get_smarty();
    foreach($tD as $name => $obj){
      $smarty->assign("USE_".$name, (empty($obj['TABNAME']) || class_available($obj['TABNAME'])));
    }

    $display = $headpage->render();
    return($this->getHeader().$display);
  }


  public function getObjectDefinitions()
  {
    $tabs = array(
        "FAKE_OC_OpsiHost" => array(
          "ou"          => "",
          "plugClass"   => "opsiGeneric",
          "tabClass"    => "opsi_tabs",
          "tabDesc"     => "OPSITABS",
          "aclClass"    => "opsiGeneric",
          "sendEvents"  => TRUE,
          "aclCategory" => "opsi"),

        "goServer" => array(
          "ou"          => get_ou('serverRDN'),
          "plugClass"   => "servgeneric",
          "tabClass"    => "servtabs",
          "tabDesc"     => "SERVTABS",
          "aclClass"    => "servgeneric",
          "sendEvents"  => TRUE,
          "aclCategory" => "server"),

        "gotoWorkstation" => array(
          "ou"          => get_ou('workstationRDN'),
          "plugClass"   => "workgeneric",
          "tabClass"    => "worktabs",
          "tabDesc"     => "WORKTABS",
          "aclClass"    => "workstation",
          "sendEvents"  => TRUE,
          "aclCategory" => "workgeneric"),

        "gotoTerminal" => array(
            "ou"          => get_ou('terminalRDN'),
            "plugClass"   => "termgeneric",
            "tabClass"    => "termtabs",
            "sendEvents"  => TRUE,
            "tabDesc"     => "TERMTABS",
            "aclClass"    => "terminal",
            "aclCategory" => "termgeneric"),

        "gotoPrinter" => array(
            "ou"          => get_ou('printerRDN'),
            "plugClass"   => "printgeneric",
            "tabClass"    => "printtabs",
            "tabDesc"     => "PRINTTABS",
            "aclClass"    => "printer",
            "sendEvents"  => FALSE,
            "aclCategory" => "printgeneric"),

        "FAKE_OC_NewDevice" => array(
            "ou"          => get_ou('systemIncomingRDN'),
            "plugClass"   => "termgeneric",
            "tabClass"    => "termtabs",
            "sendEvents"  => TRUE,
            "tabDesc"     => "TERMTABS",
            "aclClass"    => "terminal",
            "aclCategory" => "termgeneric"),

        "goFonHardware" => array(
            "ou"          => get_ou('phoneRDN'),
            "plugClass"   => "phoneGeneric",
            "tabClass"    => "phonetabs",
            "tabDesc"     => "PHONETABS",
            "sendEvents"  => FALSE,
            "aclClass"    => "phone",
            "aclCategory" => "phoneGeneric"),

        "FAKE_OC_winstation" => array(
            "ou"          => get_winstations_ou(),
            "plugClass"   => "wingeneric",
            "sendEvents"  => TRUE,
            "tabClass"    => "wintabs",
            "tabDesc"     => "WINTABS",
            "aclClass"    => "wingeneric",
            "aclCategory" => "winworkstation"),

        "ieee802Device" => array(
            "ou"          => get_ou('componentRDN'),
            "plugClass"   => "componentGeneric",
            "sendEvents"  => FALSE,
            "tabClass"    => "componenttabs",
            "tabDesc"     => "COMPONENTTABS",
            "aclClass"    => "componentGeneric",
            "aclCategory" => "component"),
        );

    // Now map some special types
    $tabs['FAKE_OC_NewWorkstation'] = &$tabs['gotoWorkstation'];
    $tabs['FAKE_OC_NewTerminal'] = &$tabs['gotoTerminal'];
    $tabs['FAKE_OC_NewServer'] = &$tabs['gotoWorkstation'];
    $tabs['gotoWorkstation__IS_BUSY'] = &$tabs['gotoWorkstation'];
    $tabs['gotoWorkstation__IS_ERROR'] = &$tabs['gotoWorkstation'];
    $tabs['gotoWorkstation__IS_LOCKED'] = &$tabs['gotoWorkstation'];
    $tabs['gotoTerminal__IS_BUSY'] = &$tabs['gotoTerminal'];
    $tabs['gotoTerminal__IS_ERROR'] = &$tabs['gotoTerminal'];
    $tabs['gotoTerminal__IS_LOCKED'] = &$tabs['gotoTerminal'];
    $tabs['goServer__IS_BUSY'] = &$tabs['goServer'];
    $tabs['goServer__IS_ERROR'] = &$tabs['goServer'];
    $tabs['goServer__IS_LOCKED'] = &$tabs['goServer'];

    $tabs['FAKE_OC_ArpNewDevice'] = &$tabs['FAKE_OC_NewDevice'];

    return($tabs);
  }
} 
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
