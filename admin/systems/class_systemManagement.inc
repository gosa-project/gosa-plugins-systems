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
      $this->registerAction("DaemonEvent_activate","handleEvent");
    }
    $this->registerAction("saveEvent","saveEventDialog");

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

    // Handle system activation 
    if($action == "DaemonEvent_activate"){
      echo "Aktivieren!";
    }
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
      $type = $tInfo[$headpage->getType($dn)];
      return(management::editEntry($action,$target,$all,$type['tabClass'],$type['tabDesc'],$type['aclCategory']));
    }
  }


 /*! \brief  Detects actions/events send by the ui
   *           and the corresponding targets.
   */
  function detectPostActions()
  {
    $action= management::detectPostActions();
    if(isset($_POST['abort_event_dialog']))  $action['action'] = "cancel";
    if(isset($_POST['save_event_dialog']))  $action['action'] = "saveEvent";
    return($action);
  }


  /*! \brief   Overridden render method of class mangement.
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

    $tabs['FAKE_OC_NewUnknownDevice'] = &$tabs['FAKE_OC_NewDevice'];

    return($tabs);
  }
} 
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
