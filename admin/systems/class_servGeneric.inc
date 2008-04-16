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

class servgeneric extends plugin
{
  /* Generic terminal attributes */
  var $ignore_account= TRUE;
  var $interfaces= array();

  /* Needed values and lists */
  var $base= "";
  var $cn= "";
  var $l= "";
  var $description= "";
  var $orig_dn= "";
  var $orig_cn= "";

  /* attribute list for save action */
  var $attributes= array("cn", "description","gotoMode");
  var $objectclasses= array("top", "GOhard", "goServer");

  var $gotoMode= "locked";

  var $netConfigDNS;
  var $modes = array();
  var $ui   ;
  var $validActions   = array("reboot" => "", "update" => "", "localboot" => "", "reinstall" => "", "rescan" => "",
                            "wake" => "", "memcheck" => "", "sysinfo" => "");

  var $fai_activated  =FALSE;
  var $view_logged = FALSE;

  var $currently_installing = FALSE;
  var $currently_installing_warned = FALSE;

  function servgeneric (&$config, $dn= NULL, $parent= NULL)
  {
    /* Check if FAI is activated */
    $tmp = $config->search("faiManagement", "CLASS",array('menu','tabs'));
    
    if(!empty($tmp)){
      $this->fai_activated = TRUE;
    }

    plugin::plugin ($config, $dn, $parent);

    $this->ui = get_userinfo();
    $this->modes["active"]= _("Activated");
    $this->modes["locked"]= _("Locked");

    /* Set base */
    if ($this->dn == "new"){
      $ui= get_userinfo();
      $this->base= dn2base($ui->dn);
      $this->cn= "";
    } else {
      $this->base= preg_replace ("/^[^,]+,".normalizePreg(get_ou("serverou"))."/", "", $this->dn);
    }
    $this->netConfigDNS = new termDNS($this->config,$this,$this->objectclasses);
    $this->netConfigDNS->set_acl_category("server");
    $this->netConfigDNS->set_acl_base($this->base);

    /* Check if this host is currently in installation process*/
    if(class_available("gosaSupportDaemon") && class_available("DaemonEvent")){
      $o = new gosaSupportDaemon();
      $e_types = DaemonEvent::get_event_types(USER_EVENT | SYSTEM_EVENT | HIDDEN_EVENT);
      $evts = $o->get_entries_by_mac(array($this->netConfigDNS->macAddress));
      foreach($evts as $evt){
        if(isset($e_types['QUEUED'][$evt['HEADERTAG']]) && $evt['STATUS'] == "processing" && 
            $e_types['QUEUED'][$evt['HEADERTAG']] == "DaemonEvent_reinstall"){
          $this->currently_installing =TRUE;
        }
      }
    }
       
    /* Save dn for later references */
    $this->orig_dn= $this->dn;
    $this->orig_cn= $this->cn;
  }


  function set_acl_base($base)
  {
    plugin::set_acl_base($base);
    $this->netConfigDNS->set_acl_base($base);
  }


  function set_acl_category($cat)
  {
    plugin::set_acl_category($cat);
    $this->netConfigDNS->set_acl_category($cat);
  }


  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    if($this->is_account && !$this->view_logged){
      $this->view_logged = TRUE;
      new log("view","server/".get_class($this),$this->dn);
    }

    /* Do we represent a valid server? */
    if (!$this->is_account && $this->parent === NULL){
      $display= "<img alt=\"\" src=\"images/small-error.png\" align=middle>&nbsp;<b>".
        msgPool::noValidExtension(_("server"))."</b>"; 
      return($display);
    }

    if ((isset($_POST['action'])) && ($this->acl_is_writeable("FAIstate")) && isset($this->mapActions[$_POST['saction']]) ){
      $action = $_POST['saction'];
      
      /* Check if we have an DaemonEvent for this action */
      if(class_available("DaemonEvent")){
        $events = DaemonEvent::get_event_types(SYSTEM_EVENT | HIDDEN_EVENT);
        if(isset($events['TRIGGERED']["DaemonEvent_".$action])){
          $evt = $events['TRIGGERED']["DaemonEvent_".$action];
          $tmp = new $evt['CLASS_NAME']($this->config);
          $tmp->add_targets(array($this->netConfigDNS->macAddress));
          $tmp->set_type(TRIGGERED_EVENT);
          $o_queue = new gosaSupportDaemon();
          if(!$o_queue->append($tmp)){
            msg_dialog::display(_("Service infrastructure"),msgPool::siError($o_queue->get_error()),ERROR_DIALOG);
          }
        }
      }else{
        msg_dialog::display(_("Event error"),
                    sprintf(_("Event '%s' is not available!"),$action),ERROR_DIALOG);
      }
    }

    /* Base select dialog */
    $once = true;
    foreach($_POST as $name => $value){
      if(preg_match("/^chooseBase/",$name) && $once ){
        $once = false;
        $this->dialog = new baseSelectDialog($this->config,$this,$this->get_allowed_bases());
        $this->dialog->setCurrentBase($this->base);
      }
    }

    /* Dialog handling */
    if(is_object($this->dialog)){
      /* Must be called before save_object */
      $this->dialog->save_object();

      if($this->dialog->isClosed()){
        $this->dialog = false;
      }elseif($this->dialog->isSelected()){
  
        /* Only accept allowed bases */
        $tmp = $this->get_allowed_bases();
        if($tmp[$this->dialog->isSelected()]){
          $this->base = $this->dialog->isSelected();
        }
        $this->dialog= false;
      }else{
        return($this->dialog->execute());
      }
    }

    /* Fill templating stuff */
    $smarty= get_smarty();

    $tmp = $this->plInfo();
    foreach($tmp['plProvidedAcls'] as $name => $translated){
      $smarty->assign($name."ACL",$this->getacl($name));
    }

    /* Assign base ACL */
    $smarty->assign("bases"   , $this->get_allowed_bases());

    /* Assign attributes */
    foreach ($this->attributes as $attr){
      $smarty->assign("$attr", $this->$attr);
    }

    $smarty->assign("staticAddress", "");
    $smarty->assign("base_select", $this->base);

    /* Assign status */
    if (gosaSupportDaemon::ping($this->netConfigDNS->macAddress)){
      $smarty->assign("actions", array("halt" => _("Switch off"), "reboot" => _("Reboot"),
                                       "update" => _("System update"),
                                       "reinstall" => _("Reinstall"),
                                       "rescan" => _("Rescan hardware"),
                                       "memcheck" => _("Memory test"),
                                       "localboot" => _("Force localboot"),
                                       "sysinfo"  => _("System analysis")));
    } else {
      $smarty->assign("actions", array("wake" => _("Wake up"),
                                       "reinstall" => _("Reinstall"),
                                       "update" => _("System update"),
                                       "memcheck" => _("Memory test"),
                                       "localboot" => _("Force localboot"),
                                       "sysinfo"  => _("System analysis")));
    }

    /* Show main page */
    $smarty->assign("fai_activated",$this->fai_activated);

    $str = $this->netConfigDNS->execute();
    if(is_object($this->netConfigDNS->dialog)){
      return($str);
    }
    $smarty->assign("netconfig", $str);
    $smarty->assign("modes", $this->modes);
    $smarty->assign("currently_installing", $this->currently_installing);

    return($smarty->fetch (get_template_path('server.tpl', TRUE)));
  }

  function remove_from_parent()
  {
    /* Cancel if there's nothing to do here */
    if ((!$this->initially_was_account) || (!$this->acl_is_removeable())){
      return;
    }

    $this->netConfigDNS->remove_from_parent();
    $ldap= $this->config->get_ldap_link();
    $ldap->rmdir($this->dn);

    update_accessTo($this->orig_cn,"");

    new log("remove","server/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());

    if (!$ldap->success()){
      msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, LDAP_MOD, get_class()));
    }

    /* Delete references to object groups */
    $ldap->cd ($this->config->current['BASE']);
    $ldap->search ("(&(objectClass=gosaGroupOfNames)(member=".LDAP::prepare4filter($this->dn)."))", array("cn"));
    while ($ldap->fetch()){
      $og= new ogroup($this->config, $ldap->getDN());
      unset($og->member[$this->dn]);
      $og->save ();
    }

    /* Clean queue form entries with this mac 
     */
    if(class_available("gosaSupportDaemon") && tests::is_mac($this->netConfigDNS->orig_macAddress)){
      $q = new gosaSupportDaemon();
      $q->clean_queue_from_mac($this->netConfigDNS->orig_macAddress);
    }
    $this->handle_post_events("remove",array("macAddress" => $this->netConfigDNS->macAddress,"ipHostNumber" => $this->netConfigDNS->ipHostNumber));
  }


  /* Save data to object */
  function save_object()
  {
    /* Save current base, to be able to revert to last base, 
        if new base is invalid or not allowed to be selected */
    $base_tmp = $this->base;
    plugin::save_object();
    $this->netConfigDNS->save_object();

    /* Get base selection */
    $tmp = $this->get_allowed_bases();
    if(isset($_POST['base'])){
      if($tmp[$_POST['base']]){
        $this->base = $_POST['base'];
      }else{
        $this->base = $base_tmp;
      }
    }
  }


  /* Check supplied data */
  function check()
  {
    /* Call common method to give check the hook */
    $message= plugin::check();
    $message= array_merge($message, $this->netConfigDNS->check());
    $this->dn= "cn=".$this->cn.",".get_ou('serverou').$this->base;

    /* must: cn */
    if ($this->cn == ""){
      msgPool::required(_("Server name"));
    }
    
    /* Check if given name is a valid host/dns name */
    if(!tests::is_dns_name($this->cn)){
      $message[]= msgPool::invalid(_("Server name"), $this->cn, "/[a-z0-9\.\-]/i");
    }

    if ($this->orig_dn != $this->dn){
      $ldap= $this->config->get_ldap_link();
      $ldap->cd ($this->base);
      $ldap->search ("(cn=".$this->cn.")", array("cn"));
      if ($ldap->count() != 0){
        while ($attrs= $ldap->fetch()){
          if ($attrs['dn'] != $this->orig_dn){
            if(!preg_match("/cn=dhcp,/",$attrs['dn']) && !preg_match("/,".get_ou('incomingou')."/",$attrs['dn']) && preg_match("/,".get_ou('serverou')."/",$attrs['dn'])){
              $message[]= msgPool::duplicated(_("Server name"));
              break;
            }
          }
        }
      }
    }

    /* Warn the user, that this host is currently installing */
    if($this->currently_installing && !$this->currently_installing_warned && !preg_match("/".normalizePreg(get_ou("incomingou"))."/",$this->orig_dn)){
      $this->currently_installing_warned = TRUE;
      $message[] = _("This host is currently installing, if you really want to save it, save again.");
    }

    return ($message);
  }


  /* Save to LDAP */
  function save()
  {
    /* Detect mode changes */
    $activate= (isset($this->saved_attributes['gotoMode']) &&
        $this->gotoMode != $this->saved_attributes['gotoMode'] &&
        $this->gotoMode == "active" &&
        tests::is_ip($this->netConfigDNS->ipHostNumber));

    plugin::save();

    /* Remove all empty values */
    if ($this->orig_dn == 'new'){
      $attrs= array();
      foreach ($this->attrs as $key => $val){
        if (is_array($val) && count($val) == 0){
          continue;
        }
        $attrs[$key]= $val;
      }
      $this->attrs= $attrs;
    }

    /* Write back to ldap */
    $ldap= $this->config->get_ldap_link();
    if ($this->orig_dn == 'new'){
      $ldap->cd($this->config->current['BASE']);
      $ldap->create_missing_trees(preg_replace('/^[^,]+,/', '', $this->dn));
      $ldap->cd($this->dn);
      $ldap->add($this->attrs);
      new log("create","server/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
      $mode= "add";
    } else {
  
      $ldap->cd($this->dn);
      $this->cleanup();
      $ldap->modify ($this->attrs); 
      new log("modify","server/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());

      /* Update all accessTo/trust dependencies */
      if($this->orig_cn != $this->cn){
        update_accessTo($this->orig_cn,$this->cn);
      }

      $mode= "modify";
    }
    if (!$ldap->success()){
      msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, LDAP_DEL, get_class()));
    }
    $this->netConfigDNS->cn = $this->cn;
    $this->netConfigDNS->save();

    /* Optionally execute a command after we're done */
    $this->handle_post_events($mode,array("macAddress" => $this->netConfigDNS->macAddress,"ipHostNumber" => $this->netConfigDNS->ipHostNumber));

    if ($activate){

      /* Send installation activation 
       */
      $events = DaemonEvent::get_event_types(SYSTEM_EVENT | HIDDEN_EVENT);
      $o_queue = new gosaSupportDaemon();
      if(isset($events['TRIGGERED']['DaemonEvent_installation_activation'])){
        $evt = $events['TRIGGERED']['DaemonEvent_installation_activation'];
        $tmp = new $evt['CLASS_NAME']($this->config);
        $tmp->set_type(TRIGGERED_EVENT);
        $tmp->add_targets(array($this->netConfigDNS->macAddress));
        if(!$o_queue->append($tmp)){
          msg_dialog::display(_("Service infrastructure"),msgPool::siError($o_queue->get_error()),ERROR_DIALOG);
        }
      }
    }

  }


  /* Display generic part for server copy & paste */
  function getCopyDialog()
  {
    $vars = array("cn");

    $smarty = get_smarty();
    $smarty->assign("cn" ,$this->cn);
    $smarty->assign("object","server");
    $str = $smarty->fetch(get_template_path("paste_generic.tpl",TRUE));
    $ret = array();
    $ret['string'] = $str;
    $ret['status'] = "";
    return($ret);
  }


  function saveCopyDialog()
  {
    if(isset($_POST['cn'])){
      $this->cn = $_POST['cn'];
    }
  }

  
  function PrepareForCopyPaste($source)  
  {
    plugin::PrepareForCopyPaste($source);
    if(isset($source['macAddress'][0])){
      $this->netConfigDNS->macAddress = $source['macAddress'][0];
    }
    if(isset($source['ipHostNumber'][0])){
      $this->netConfigDNS->ipHostNumber = $source['ipHostNumber'][0];
    }

  }
  


  /* Return plugin informations for acl handling */ 
  static function plInfo()
  {
    return (array( 
          "plShortName"   => _("Generic"),
          "plDescription" => _("Server generic"),
          "plSelfModify"  => FALSE,
          "plDepends"     => array(),
          "plPriority"    => 1,
          "plSection"     => array("administration"),           
          "plCategory"    => array("server" => array("description"  => _("Server"),
                                                     "objectClass"  => "goServer")),
          "plProvidedAcls"=> array(
            "cn"           => _("Name"),
            "description"  => _("Description"),
            "gotoMode"     => _("Goto mode"),
            "base"         => _("Base"),
            "FAIstate"     => _("Action flag"))
          ));
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>