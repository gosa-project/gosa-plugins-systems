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

class systems extends plugin
{
  /* Definitions */
  var $plHeadline     = "Systems";
  var $plDescription  = "This does something";
  var $plIcon         = "plugins/systems/images/plugin.png";
  var $departments    = array();

  /* Dialog attributes */
  var $systab   = NULL;
  var $terminals= array();
  var $ui       = NULL;
  var $DivListSystem;
  var $start_pasting_copied_objects = FALSE;
  var $CopyPasteHandler   = NULL;

  /* Arp hanlding activated */
  var $arp_handling_active = FALSE;
  var $last_action = "";

  var $dns = array();

  var $system_activation_object = ""; // The object to activate (NewDevice)
  var $fai_activated  = FALSE;
  var $si_active      = FALSE;

  var $acl_module = array("incoming","terminal","workstation","server","printer","phone","winworkstation","component");

  var $opsi = NULL;

  function systems (&$config, $ui)
  {
    /* Save configuration for internal use */
    $this->config= $config;
    $this->ui= $ui;

    /* Add FAIstate to attributes if FAI is activated */
    $tmp = $this->config->search("faiManagement", "CLASS",array('menu','tabs'));
    if(!empty($tmp)){
      $this->fai_activated = TRUE;
    }

    /* Creat dialog object */
    $this->DivListSystem = new divListSystem($this->config,$this);

    /* Copy & Paste enabled ?*/
    if ($this->config->get_cfg_value("enablecopypaste") == "true"){
      $this->CopyPasteHandler = new CopyPasteHandler($this->config);
    }

    /* Check whether the arp handling active or not */
    if($this->config->search("ArpNewDevice","CLASS",array('tabs')) != ""){
      $this->arp_handling_active = TRUE;
    }

    /* Check if we are able to communicate with the GOsa supprot daemon 
     */
    if(class_available("gosaSupportDaemon")){
      $o = new gosaSupportDaemon();
      $this->si_active = $o->connect() && class_available("DaemonEvent");
    }

    /* Check if we are able to communicate with the GOsa supprot daemon 
     */
    if(class_available("opsi")){
      $this->opsi = new opsi($this->config);
    }
  }


  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    session::set('LOCK_VARS_TO_USE',array("/^system_edit_/i","/^system_del_/","/^act/","/^id/","/^item_selected/","/^remove_multiple_systems/","/^menu_action/"));

    /********************
      Check for functional posts, edit|delete|add|... system devices 
     ********************/
    $s_action     = "";                       // Contains the action to proceed
    $s_entry      = "";                       // The value for s_action
    $base_back    = "";                       // The Link for Backbutton
    $smarty       = get_smarty();

    /* Test Posts */
    foreach($_POST as $key => $val){
      // Post for delete
      if(preg_match("/system_del.*/",$key)){
        $s_action = "del";
        $s_entry  = preg_replace("/system_del_/i","",$key);
        // Post for edit
      }elseif(preg_match("/system_edit_.*/",$key)){
        $s_action="edit";
        $s_entry  = preg_replace("/system_edit_/i","",$key);
        // Post for new
      }elseif(preg_match("/system_new.*/",$key)){
        $s_action="new";
      }elseif(preg_match("/system_tplnew.*/i",$key)){
        $s_action="new_tpl";
      }elseif(preg_match("/system_setpwd_.*/i",$key)){
        $s_action="change_pw";
        $s_entry  = preg_replace("/system_setpwd_/i","",$key);
      }elseif(preg_match("/gen_cd_.*/i",$key)){
        $s_action="gen_cd";
        $s_entry  = preg_replace("/gen_cd_/i","",$key);
      }elseif(preg_match("/^copy_.*/",$key)){
        $s_action="copy";
        $s_entry  = preg_replace("/^copy_/i","",$key);
      }elseif(preg_match("/^cut_.*/",$key)){
        $s_action="cut";
        $s_entry  = preg_replace("/^cut_/i","",$key);
      }
    }
   
    /* remove image tags from posted entry  (posts looks like this 'name_x')*/
    $s_entry  = preg_replace("/_.$/","",$s_entry);

    /* Edit was requested by pressing the name(link) of an item */
    if((isset($_GET['act']))&&($_GET['act']=="edit_entry")){
      $s_action ="edit";
      $s_entry  = $_GET['id'];
    }
    /* Create options */
    if(isset($_POST['menu_action']) && preg_match("/^newsystem_/",$_POST['menu_action'])){
      $s_action = "newsystem";
      $s_entry  = preg_replace("/^newsystem_/","",$_POST['menu_action']);
    }

    /* handle C&P from layers menu */
    if(isset($_POST['menu_action']) && preg_match("/^multiple_copy_systems/",$_POST['menu_action'])){
      $s_action = "copy_multiple";
    }
    if(isset($_POST['menu_action']) && preg_match("/^multiple_cut_systems/",$_POST['menu_action'])){
      $s_action = "cut_multiple";
    }
    if(isset($_POST['menu_action']) && preg_match("/^editPaste/",$_POST['menu_action'])){
      $s_action = "editPaste";
    }

    /* Handle daemon events */
    if(isset($_POST['menu_action']) && preg_match("/^trigger_event_/",$_POST['menu_action'])){
      $s_action = $_POST['menu_action'];
    }
    
    /* Handle daemon events */
    if(isset($_POST['menu_action']) && preg_match("/^schedule_event_/",$_POST['menu_action'])){
      $s_action = $_POST['menu_action'];
    }

    /* handle remove from layers menu */
    if(isset($_POST['menu_action']) && preg_match("/^remove_multiple/",$_POST['menu_action'])){
      $s_action = "del_multiple";
    }
    
    /* Handle instant actions from layers menu */
    foreach (array("halt", "reboot", "reinstall", "update", "wake") as $act){
	    if(isset($_POST['menu_action']) && preg_match("/^${act}_multiple/",$_POST['menu_action'])){
	      $s_action = "${act}_multiple";
	    }
    }

    /* Activate multiple machines */
    if(isset($_POST['menu_action']) && preg_match("/^activate_multiple/",$_POST['menu_action'])){
      $s_action = "activate_multiple";
    }

    /* Check for exeeded sizelimit */
    if (($message= check_sizelimit()) != ""){
      return($message);
    }

    /* Try to get informations about what kind of system to create */
    if ($s_action=="new") {
      return ($smarty->fetch(get_template_path('chooser.tpl', TRUE)));
    }

    /* Incoming handling  
     * If someone made a systemtype and ogroup selection 
     * Display the new requested entry type ... servtab  in case of server and so on.
     */
    if(isset($_POST['SystemTypeChoosen'])){
      $s_action = "SelectedSystemType";
    }

    /********************
      Copy & Paste Handling  ...
     ********************/

    /* Display the copy & paste dialog, if it is currently open */
    $ret = $this->copyPasteHandling_from_queue($s_action,$s_entry);
    if($ret){
      return($ret);
    }
  

    /********************
      Create FAI CD ...   
     ********************/
    if ($s_action=="gen_cd"){
      $this->dn= $this->terminals[$s_entry]['dn'];
      session::set('objectinfo',$this->dn);
      return ($smarty->fetch(get_template_path('gencd.tpl', TRUE)));
    }


    /* Start CD-Creation */
    if ((isset($_POST["cd_create"])) && !empty($this->dn)){
      $smarty->assign("src", "?plug=".$_GET['plug']."&amp;PerformIsoCreation");
      return ($smarty->fetch(get_template_path('gencd_frame.tpl', TRUE)));
    }


    if ($this->dn != "" && isset($_GET['PerformIsoCreation'])){

      $return_button   = "<form method='get' action='main.php' target='_parent'>
        <input type='submit' value='"._("Back")."'>
        <input type='hidden' name='plug' value='".$_GET['plug']."'/>
        </form>";

      $dsc             = array(0 => array("pipe", "r"), 1 => array("pipe", "w"), 2 => array("pipe", "w"));

      /* Get and check command */
      $command= $this->config->search("workgeneric", "ISOCMD",array('tabs'));
      
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
      }

      /* Scroll down completly */
      echo '<script language="javascript" type="text/javascript">scrollDown2();</script>' ;
      echo '</body></html>';
      flush();
      exit;
    }


    /********************
      New Device hanlding  (Ogroup/System select dialog.)
     ********************/

    if($s_action == "SelectedSystemType"){

      /* Possible destination system types 
       */
      $tabs = $this->get_tab_defs();

      /* Remember dialog selection.
       */
      $selected_group = $_POST['ObjectGroup'];
      $selected_system = $_POST['SystemType'];

      $this->systab = NULL;

      /* Check if system type exists. It should! */
      if(isset($tabs[$selected_system])){

        /* Get tab informations */
        $class    = $tabs[$selected_system]["CLASS"];
        $tabname  = $tabs[$selected_system]["TABNAME"];
        $tabclass = $tabs[$selected_system]["TABCLASS"];
        $acl_cat  = $tabs[$selected_system]["ACLC"];


        if(!class_available($tabclass)){
          msg_dialog::display(_("Error"), msgPool::class_not_found($tabclass), ERROR_DIALOG);
        }else{

          /* Go through all objects that should be activated */
          foreach($this->system_activation_object as $key => $dn){

            /* Remove entry from list, to avoid page-reload problems */
            unset($this->system_activation_object[$key]);

            /* Load permissions for selected 'dn' and check if
               we're allowed to create this 'dn' */
            $this->dn = $dn;
            $ui       = get_userinfo();
            $tabacl   = $ui->get_permissions($this->dn,$acl_cat."/".$tabname);

            /* We are allowed to create the requested system type */
            if(preg_match("/c/",$tabacl)){
              $this->systab= new $tabclass($this->config, $this->config->data['TABS'][$class], $this->dn,$selected_system);
              $this->systab->set_acl_base($this->DivListSystem->selectedBase);
              $this->systab->by_object[$tabname]->base = $this->DivListSystem->selectedBase;
              $this->systab->base = $this->DivListSystem->selectedBase;


              if($selected_group != "none"){

                /*******
                 * Set gotoMode to active if we there was an ogroup selected. 
                 */
                $found = false;
                foreach(array("workgeneric"=>"active","servgeneric"=>"active","termgeneric"=>"active") as $tab => $value){
                  if(isset($this->systab->by_object[$tab]->gotoMode)) {
                    $found = true;
                    $this->systab->by_object[$tab]->gotoMode = $value;
                  }
                }
                if(!$found){
                  msg_dialog::display(_("Internal error"), _("Cannot set mode to 'active'!"), ERROR_DIALOG);
                }

                /*******
                 * Update object group membership
                 */
                $og = new ogroup($this->config,$selected_group);
                if($og){
                  $og->AddDelMembership($this->systab->dn);
                  $og->save();
                }

                /*******
                 * Set default system specific attributes 
                 */
                foreach (array("workservice", "termservice") as $cls){
                  if (isset($this->systab->by_object[$cls])){
                    $this->systab->by_object[$cls]->gotoXMouseport= "";
                    $this->systab->by_object[$cls]->gotoXMouseType= "";
                    $this->systab->by_object[$cls]->gotoXResolution= "";
                    $this->systab->by_object[$cls]->gotoXColordepth= "";
                  }
                }

                // Enable activation
                foreach (array("servgeneric", "workgeneric", "termgeneric") as $cls){
                  if (isset($this->systab->by_object[$cls])){
                    $this->systab->by_object[$cls]->auto_activate= TRUE;
                  }
                }

                // Enable sending of LDAP events
                if (isset($this->systab->by_object["workstartup"])){
                  $this->systab->by_object["workstartup"]->gotoLdap_inherit= TRUE;
                }
              }

              /* Don't save directly if there is no objectGroup selected.
                 The user will then be able to configure the missing attributes 
                 on his own.
               */
              if($selected_group != "none"){
                $this->systab->save();
                $this->systab = NULL;

                if(!isset($ldap)){
                  $ldap = $this->config->get_ldap_link();
                }
                $ldap->cd ($this->dn);
                $ldap->cat($this->dn, array('dn'));
                if(count($ldap->fetch())){
                  $ldap->cd($this->dn);
                  $ldap->rmDir($this->dn);
                }
              }
            }else{
              msg_dialog::display(_("Error"), msgPool::permCreate(), ERROR_DIALOG);
            }
          }
        }
      }
    }


    if (isset($_POST['create_system'])||$s_action=="newsystem") {
    
      $this->last_action = "";
  
      /* If the current entry is an incoming object 
       * $sw = System type as posted in new incoming handling dialog 
       */ 
      if(isset($_POST['system'])){
        $sw = $_POST['system'];
      }else{
        $sw = $s_entry;
      }
      $this->dn= "new";

      $tabs = $this->get_tab_defs();
  
      if(isset($tabs[$sw])){
        $class    = $tabs[$sw]["CLASS"];
        $tabname  = $tabs[$sw]["TABNAME"];
        $tabclass = $tabs[$sw]["TABCLASS"];
        $acl_cat  = $tabs[$sw]["ACLC"];

        /* Load permissions for selected 'dn' and check if
           we're allowed to remove this 'dn' */
        $ui       = get_userinfo();
        $tabacl   = $ui->get_permissions($this->DivListSystem->selectedBase,$acl_cat."/".$tabname);
        if(preg_match("/c/",$tabacl)){

          if(!class_available($tabclass)){
            msg_dialog::display(_("Error"), msgPool::class_not_found($tabclass), ERROR_DIALOG);
          }else{
            $this->systab= new $tabclass($this->config, $this->config->data['TABS'][$class], $this->dn,$sw);
            $this->systab->set_acl_base($this->DivListSystem->selectedBase);
            $this->systab->by_object[$tabname]->base = $this->DivListSystem->selectedBase;
            $this->systab->base = $this->DivListSystem->selectedBase;
          }
        }else{
          msg_dialog::display(_("Error"), msgPool::permCreate(), ERROR_DIALOG);
        }
      }
    }

    /********************
      System activation
     ********************/

    /* User wants to edit data? */
    if (($s_action == "activate_multiple") && (!isset($this->systab->config))){
      $this->system_activation_object = array();
      foreach($this->list_get_selected_items() as $id) {
        $obj = $this->terminals[$id];
        $type= $this->get_system_type($obj);
        if($type == "NewDevice"){
          $this->system_activation_object[] = $obj['dn'];
        }
      }
      if(count($this->system_activation_object)){
        $this->systab = new SelectDeviceType($this->config,$this->system_activation_object) ;
      }
    }


    /********************
      Edit system ...   
     ********************/

    /* User wants to edit data? */
    if (($s_action == "edit") && (!isset($this->systab->config))){
      $this->last_action = "";
      $this->dn= $this->terminals[$s_entry]['dn'];

      /* Check locking, save current plugin in 'back_plugin', so
         the dialog knows where to return. */
      if (($user= get_lock($this->dn)) != ""){
        return(gen_locked_message ($user, $this->dn));
      }

      /* Find out more about the object type */
      $attrs = $this->terminals[$s_entry];
      $type= $this->get_system_type($attrs);
     
      /* Lock the current entry, so everyone will get the
         above dialog */
      $tabs = $this->get_tab_defs();

      if($type == "ArpNewDevice"){
        if(!class_available("ArpNewDeviceTabs")){
          msg_dialog::display(_("Error"), msgPool::class_not_found("ArpNewDevice"), ERROR_DIALOG);
        }else{
          $this->systab = new ArpNewDeviceTabs($this->config,$this->config->data['TABS']['ARPNEWDEVICETABS'],$this->dn);
        }
      }elseif($type == "NewDevice"){
        if(!class_available("SelectDeviceType")){
          msg_dialog::display(_("Error"), msgPool::class_not_found("SelectDeviceType"), ERROR_DIALOG);
        }else{
          $this->system_activation_object= array($this->dn);
          $this->systab = new SelectDeviceType($this->config,$this->dn) ;
        }
      }elseif(isset($tabs[$type])){

        $class    = $tabs[$type]["CLASS"];
        $acl_cat  = $tabs[$type]["ACLC"];
        $tabclass = $tabs[$type]["TABCLASS"];

        if(!class_available($tabclass)){
          msg_dialog::display(_("Error"), msgPool::class_not_found($tabclass), ERROR_DIALOG);
        }else{
          $this->systab= new $tabclass($this->config, $this->config->data['TABS'][$class], $this->dn,$acl_cat);
          $this->systab->set_acl_base($this->dn);
          session::set('objectinfo',$this->dn);
          add_lock ($this->dn, $this->ui->dn);
        }
      }else{ 
        msg_dialog::display(_("Error"), _("Editing this type of object is not supported yet!"), ERROR_DIALOG);
        $this->remove_lock();
      }
    }


    /********************
      Change password ...   
     ********************/

    /* Set terminals root password */
    if ($s_action=="change_pw"){
      $tabs = $this->get_tab_defs();

      $dn   = $this->terminals[$s_entry]['dn'];
      $type = $this->get_system_type($this->terminals[$s_entry]);

      $class    = $tabs[$type]["CLASS"];
      $acl      = $tabs[$type]["ACL"];
      $tabclass = $tabs[$type]["TABCLASS"];
      $ui       = get_userinfo();
      $tabacl   = $ui->get_permissions($dn,$acl,"userPassword");
      if(preg_match("/w/",$tabacl)){
        $this->dn= $this->terminals[$s_entry]['dn'];
        session::set('objectinfo',$this->dn);
        return ($smarty->fetch(get_template_path('password.tpl', TRUE)));
      }else{
        msg_dialog::display(_("Permission error"), _("You have no permission to change this password!"), ERROR_DIALOG);
      }
    }


    /********************
      Password change finish, but check if entered data is ok 
     ********************/

    /* Correctly specified? */
    if (isset($_POST['password_finish'])){
      if ($_POST['new_password'] != $_POST['repeated_password']){
        msg_dialog::display(_("Error"), _("The passwords you've entered as 'New password' and 'Repeated password' do not match!"), ERROR_DIALOG);
        return($smarty->fetch(get_template_path('password.tpl', TRUE)));
      }
    }

    /********************
      Password change finish
     ********************/

    /* Change terminal password */
    if (isset($_POST['password_finish']) && 
        $_POST['new_password'] == $_POST['repeated_password']){

      /* Check if user is allowed to set password */
      $tabs = $this->get_tab_defs();

      $type = "";
      foreach($this->terminals as $terminal){
        if($terminal['dn'] == $this->dn){
          $type  = $this->get_system_type($terminal);
          break;
        } 
      }

      /* Type detected */
      $allow_for = array("terminal","workstation","server","component");
      if(!empty($type) && in_array($type,$allow_for)){

        /* Get infos */
        $plug     = $tabs[$type]["PLUG"];
        $class    = $tabs[$type]["CLASS"];
        $acl      = $tabs[$type]["ACL"];
        $tabclass = $tabs[$type]["TABCLASS"];
    
        /* Get acls */
        $ui       = get_userinfo();
        $tabacl   = $ui->get_permissions($this->dn,$acl,"userPassword");

        /* Check acls */
        if(preg_match("/w/",$tabacl)){
          $ldap = $this->config->get_ldap_link();
          $ldap->cd($this->dn);
          $ldap->cat($this->dn);
          $old_attrs = $ldap->fetch();

          $attrs= array();
          if ($_POST['new_password'] == ""){

            /* Remove password attribute 
             */
            if(in_array("simpleSecurityObject",$old_attrs['objectClass'])){
              $attrs['objectClass'] = array();
              for($i = 0 ; $i < $old_attrs['objectClass']['count'] ; $i ++){
                if(!preg_match("/simpleSecurityObject/i",$old_attrs['objectClass'][$i])){
                  $attrs['objectClass'][] = $old_attrs['objectClass'][$i];
                }
              }
            }
            $attrs['userPassword']= array();
          } else {

            /* Add/modify password attribute 
             */
            if(!in_array("simpleSecurityObject",$old_attrs['objectClass'])){
              $attrs['objectClass'] = array();
              for($i = 0 ; $i < $old_attrs['objectClass']['count'] ; $i ++){
                $attrs['objectClass'][] = $old_attrs['objectClass'][$i];
              }
              $attrs['objectClass'][] = "simpleSecurityObject";
            }

            if(class_available("passwordMethodCrypt")){
              $pwd_m = new passwordMethodCrypt($this->config);
              $pwd_m->set_hash("crypt/md5");
              $attrs['userPassword'] = $pwd_m->generate_hash($_POST['new_password']);
            }else{
              msg_dialog::display(_("Password method"),_("Password method crypt is missing. Cannot set system password."));
              $attrs = array();
            }
          }
          $ldap->modify($attrs);
          if (!$ldap->success()){
            msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, LDAP_MOD, get_class()));
          }else{
            if(class_available($plug)){
              $p = new $plug($this->config,$this->dn);
              $p->handle_post_events("modify");
            }
          }
  
          new log("security","systems/".get_class($this),$this->dn,array_keys($attrs),$ldap->get_error());
        }else{
          msg_dialog::display(_("Permission error"), _("You have no permission to change this password!"), ERROR_DIALOG);
        }
      }else{
        msg_dialog::display(_("Error"), _("Cannot determine object to change password!"), ERROR_DIALOG);
      }
      session::un_set('objectinfo');
    }


    /********************
      Delete system cancel
     ********************/

    /* Delete terminal canceled? */
    if (isset($_POST['delete_cancel']) || isset($_POST['password_cancel'])){
      $this->remove_lock();
      session::un_set('objectinfo');
    }


    /********************
      Action(s) for MULTIPLE
     ********************/

    /********************
      SCHEDULE action in GOsa Daemon
     ********************/

    if(preg_match("/^schedule_event_/",$s_action) || preg_match("/^trigger_event_/",$s_action)){
      $this->dns = array();
      $ids = $this->list_get_selected_items();

      if(count($ids) && class_available("DaemonEvent")){
        $mac= array();

        $ldap = $this->config->get_ldap_link();
        foreach($ids as $id){
          $ldap->cat ($this->terminals[$id]['dn'], array("macAddress"));
          $attrs= $ldap->fetch();
          if (isset($attrs['macAddress'][0])){
            $mac[]= $attrs['macAddress'][0];
          }
        }
        $events = DaemonEvent::get_event_types(SYSTEM_EVENT);
        $type = preg_replace("/^[a-z]*_event_/","",$s_action);
        $o_queue = new gosaSupportDaemon();

        /* Skip installation or update trigerred events, 
         *  if this entry is currently processing.
         */
        if(preg_match("/trigger_event/",$s_action) && in_array($type,array("DaemonEvent_reinstall","DaemonEvent_update"))){
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

        /* Prepare event to be added 
         */
        if(count($mac) && isset($events['BY_CLASS'][$type])){
          $event = $events['BY_CLASS'][$type];
          $this->systab = new $event['CLASS_NAME']($this->config);
          $this->systab->add_targets($mac);
          if(preg_match("/trigger_event/",$s_action)){
            $this->systab->set_type(TRIGGERED_EVENT);
          }else{
            $this->systab->set_type(SCHEDULED_EVENT);
          }
        }
      }
    }

    /* Insert scheduled events into queue */
    if(class_available("DaemonEvent") && $this->systab instanceof DaemonEvent){
      $this->systab->save_object();

      /* Save event 
       */
      if(isset($_POST['save_event_dialog']) || $this->systab->get_type() == TRIGGERED_EVENT){
        $o_queue = new gosaSupportDaemon();
        $o_queue->append($this->systab);
        if($o_queue->is_error()){
          msg_dialog::display(_("Service infrastructure"),msgPool::siError($o_queue->get_error()),ERROR_DIALOG);
        }else{
          $this->systab = FALSE;
        }
      }
      if(isset($_POST['abort_event_dialog'])){
        $this->systab = FALSE;
      }
    }


    /********************
      Delete MULTIPLE entries requested, display confirm dialog
     ********************/

    if ($s_action=="del_multiple" || $s_action == "del"){
      $this->dns = array();

      if($s_action == "del_multiple"){
        $ids = $this->list_get_selected_items();
      }else{
        $ids = array($s_entry);
      }

      $ui = get_userinfo();
      $tabs = $this->get_tab_defs();

      if(count($ids)){

        $disallowed = array();
        foreach($ids as $id){

          /* Get 'dn' from posted termlinst */
          $attrs  = $this->terminals[$id];
          $type   = $this->get_system_type($attrs);
          $dn     = $attrs['dn'];
          $acl = $this->ui->get_permissions($dn, $tabs[$type]['ACL']);
          if(preg_match("/d/",$acl)){
            $this->dns[$id] = $dn;
          }else{
            $disallowed[] = $dn;
          }
        }

        if(count($disallowed)){
          msg_dialog::display(_("Permission"),msgPool::permDelete($disallowed),INFO_DIALOG);
        }

        if(count($this->dns)){

          if ($user= get_multiple_locks($this->dns)){
            return(gen_locked_message($user,$this->dns));
          }

          $dns_names = array();
          foreach($this->dns as $dn){
            add_lock ($dn, $this->ui->dn);
            $dns_names[] = @LDAP::fix($dn);
          }

          /* Lock the current entry, so nobody will edit it during deletion */
          $smarty->assign("warning", msgPool::deleteInfo($dns_names));
          $smarty->assign("multiple", true);
          return($smarty->fetch(get_template_path('remove.tpl', TRUE)));
        }
      }
    }


    /********************
      Delete MULTIPLE entries confirmed
     ********************/

    /* Confirmation for deletion has been passed. Users should be deleted. */
    if (isset($_POST['delete_multiple_system_confirm'])){

      $ui = get_userinfo();
      $tabs = $this->get_tab_defs();

      /* Remove user by user and check acls before removeing them */
      foreach($this->dns as $key => $dn){

        /* Get 'dn' from posted termlinst */
        $attrs    = $this->terminals[$key];
        $type= $this->get_system_type($attrs);

        /* get object type */
        $tabtype  = "termtabs";
        $tabobj   = "TERMTABS";
        $tabacl   = "";
        if(isset($tabs[$type])){
          $tabtype = $tabs[$type]['TABCLASS'];
          $tabobj  = $tabs[$type]['CLASS'];
          $tabacl  = $ui->get_permissions($dn,$tabs[$type]['ACL']);

          /* Load permissions for selected 'dn' and check if
             we're allowed to remove this 'dn' */
          if(preg_match("/d/",$tabacl)){ 

            /* Delete request is permitted, perform LDAP action */
            if(in_array($type,array("ArpNewDevice","NewDevice")) && class_available("termgeneric")){
              $this->systab= new termgeneric($this->config, $dn);
              $this->systab->set_acl_base($dn);
              $this->systab->remove_from_parent();
            }elseif($tabtype=="phonetabs"){
              $this->systab= new $tabtype($this->config, $this->config->data['TABS'][$tabobj], $dn,$type);
              $this->systab->set_acl_base($dn);
              $this->systab->by_object['phoneGeneric']->remove_from_parent ();
            }else{
              $this->systab= new $tabtype($this->config,$this->config->data['TABS'][$tabobj], $dn,$type);
              $this->systab->set_acl_base($dn);
              $this->systab->delete();
            }
            unset ($this->systab);
            $this->systab= NULL;

          } else {
            /* Normally this shouldn't be reached, send some extra
               logs to notify the administrator */
            msg_dialog::display(_("Permission error"), msgPool::permDelete(), ERROR_DIALOG);
            new log("security","systems/".get_class($this),$dn,array(),"Tried to trick deletion.");
          }
        }

        /* Remove lock file after successfull deletion */
        $this->remove_lock();
        $this->dns = array();
      }
    }

    /********************
      Delete MULTIPLE entries Canceled
     ********************/

    /* Remove lock */
   if(isset($_POST['delete_multiple_system_cancel'])){

     /* Remove lock file after successfull deletion */
     $this->remove_lock();
     $this->dns = array();
   }


    /********************
      Edit system type finished, check if everything went ok
     ********************/
    /* Finish user edit is triggered by the tabulator dialog, so
       the user wants to save edited data. Check and save at this
       point. */

    /* Dirty workaround - MSG_DIALOG - OK
      If a message dialog is shown and we press 'OK'
       then try to save again.
     */
    foreach($_POST as $name => $value){
      if(preg_match("/^MSG_OK/",$name)){
        $_POST[$this->last_action] = TRUE;
      }
    }

    if ((isset($_POST['edit_finish']) || isset($_POST['edit_apply'])) && (isset($this->systab->config))){

      /* If the save routine gets interrupted by a confirm dialog, 
          store last action so we can trigger it again after 'Ok' was pressed.
         (This is the case if a system gets modified while it is installing - GOsa si)
       */
      $this->last_action = ""; 
      if(isset($_POST['edit_finish'])){
        $this->last_action = "edit_finish"; 
      }elseif(isset($_POST['edit_apply'])){
        $this->last_action = "edit_apply";
      }

      /* Check tabs, will feed message array */
      $message = $this->systab->check();

      /* Save, or display error message? */
      if (count($message) == 0){


        $this->systab->save();

        /* Terminal has been saved successfully, remove lock from LDAP. */
        if (!isset($_POST['edit_apply'])){
          if ($this->dn != "new"){
            $this->remove_lock();
          }

          unset ($this->systab);
          $this->systab= NULL;
          session::un_set('objectinfo');
        }else{
      
          /* Reinitialize tab */
          if($this->systab instanceof tabs){
            $this->systab->re_init();
          }
        }
      } else {
        /* Ok. There seem to be errors regarding to the tab data,
           show message and continue as usual. */
        msg_dialog::displayChecks($message);
      }
    }


    /********************
      Edit system was canceled 
     ********************/
    /* Cancel dialogs */
    if (isset($_POST['edit_cancel']) || isset($_POST['password_cancel']) || isset($_POST['SystemTypeAborted'])){
      if (isset($this->systab)){
        $this->remove_lock();
        unset ($this->systab);
      }
      $this->systab= NULL;
      session::un_set('objectinfo');
    }

    /********************
      Display edit dialog, or some other
     ********************/

    /* Show tab dialog if object is present */
    if (isset($this->systab->config)){
      $display= $this->systab->execute();

      /* Don't show buttons if tab dialog requests this */

      $dialog     = FALSE;
      $hide_apply = $this->dn == "new";
      $hide_apply = ($this->dn == "new") || (preg_match("/".normalizePreg(get_ou("incomingou"))."/",$this->dn));
      if(is_object($this->systab) && !isset($this->systab->by_object)){
        $dialog = TRUE;
        $hide_apply = TRUE;
      }elseif(isset($this->systab->by_object[$this->systab->current]->dialog)){
        $dia = $this->systab->by_object[$this->systab->current]->dialog;
        if($dia === TRUE || is_object($dia)){
          $dialog = TRUE;
        }  
      }
      if(isset($this->systab->current) && isset($this->systab->by_object[$this->systab->current]->netConfigDNS) && 
        $this->systab->by_object[$this->systab->current]->netConfigDNS->dialog){
        $dialog = TRUE;
      }

      if (!$dialog){
        $display.= "<p style=\"text-align:right\">\n";
        $display.= "<input type=\"submit\" name=\"edit_finish\" style=\"width:80px\" value=\"".msgPool::okButton()."\">\n";
        $display.= "&nbsp;\n";
        if (!$hide_apply){
          $display.= "<input type=submit name=\"edit_apply\" value=\"".msgPool::applyButton()."\">\n";
          $display.= "&nbsp;\n";
        }
        $display.= "<input type=\"submit\" name=\"edit_cancel\" value=\"".msgPool::cancelButton()."\">\n";
        $display.= "</p>";
      }
      return ($display);
    }

    /* Check if there is a snapshot dialog open */
    $base = $this->DivListSystem->selectedBase;
    if($str = $this->showSnapshotDialog($base,$this->get_used_snapshot_bases(),$this)){
      return($str);
    }

    /* Display dialog with system list */
    $this->DivListSystem->parent = $this;
    $this->DivListSystem->execute();

    /* Add departments if subsearch is disabled */
    if(!$this->DivListSystem->SubSearch){

      /* Add FAIstate to attributes if FAI is activated */
      if($this->fai_activated){
        $this->DivListSystem->AddDepartments($this->DivListSystem->selectedBase,4,1);
      }else{
        $this->DivListSystem->AddDepartments($this->DivListSystem->selectedBase,3,1);
      }
    }
    $this->reload();
    $this->DivListSystem->setEntries($this->terminals);
    return($this->DivListSystem->Draw());
  }


  /* Return departments, that will be included within snapshot detection */
  function get_used_snapshot_bases()
  {
    $tmp = array();

    /* Check acls, if we are not allowed to create and write each plugin tab, skip this object */

    $tabs = array(
        "terminal"        => get_ou('terminalou'),
        "workstation"     => get_ou('workstationou'),
        "incoming"        => get_ou('incomingou'),
        "server"          => get_ou('serverou'),
        "printer"         => get_ou('printerou'),
        "phone"           => get_ou('phoneou'),
        "winworkstation"  => get_winstations_ou(),
        "component"       => get_ou('componentou')
        ); 

    foreach($tabs as $acl_cat => $dn){

      $acl_all = $this->ui->has_complete_category_acls($dn.$this->DivListSystem->selectedBase,$acl_cat);
      if(preg_match("/(c.*w|w.*c)/",$acl_all)){
        $tmp[] = $dn.$this->DivListSystem->selectedBase;
      }
    }
    return($tmp); 
  }


  function remove_from_parent()
  {
    /* Optionally execute a command after we're done */
    $this->postremove();
  }


  /* Save data to object */
  function save_object()
  {
    $this->DivListSystem->save_object();
    if(is_object($this->CopyPasteHandler)){
      $this->CopyPasteHandler->save_object();
    }
  }


  /* Check values */
  function check()
  {
  }


  /* Save to LDAP */
  function save()
  {
  }

  function adapt_from_template($dn, $skip= array())
  {
  }

  function password_change_needed()
  {
  }

  function reload()
  {
    /* some var init */
    $ui = get_userinfo();
    $res              = array();
    $this->terminals  = array();
    $userregex        = "";
    $opsi_clients     = array();

    /* Set base for all searches */
    $base=  $this->DivListSystem->selectedBase;

    /* Prepare samba class name */
    $samba  ="";
    if ($this->DivListSystem->ShowWinWorkstations){
      if ($this->config->get_cfg_value("sambaversion") == "3"){
        $samba= "sambaSamAccount";
      } else {
        $samba= "sambaAccount";
      }
    }

    /* This array represents the combination between checkboxes and search filters */
    $objs = array( 
        "ShowServers"        => array("TAB" => "servtabs",      "CLASS" => "goServer"        ,"TREE" => get_ou('serverou')),
        "ShowTerminals"      => array("TAB" => "termtabs",      "CLASS" => "gotoTerminal"    ,"TREE" => get_ou('terminalou')),
        "ShowPrinters"       => array("TAB" => "printtabs",     "CLASS" => "gotoPrinter"     ,"TREE" => get_ou('printerou')),
        "ShowDevices"        => array("TAB" => "componenttabs", "CLASS" => "ieee802Device"   ,"TREE" => get_ou('componentou')),
        "ShowPhones"         => array("TAB" => "phonetabs",     "CLASS" => "goFonHardware"   ,"TREE" => get_ou('phoneou')),
        "ShowWorkstations"   => array("TAB" => "worktabs",      "CLASS" => "gotoWorkstation" ,"TREE" => get_ou('workstationou')),
        "ShowWinWorkstations"=> array("TAB" => "wintabs",       "CLASS" => $samba            ,"TREE" => get_winstations_ou() ));

    /* Include the 'Display Systems of user' attribute */ 
    if ((!empty($this->DivListSystem->UserRegex)) && ($this->DivListSystem->UserRegex!= "*")){
      $userregex = "(gotoLastUser=".$this->DivListSystem->UserRegex.")";
    }

    /* Attributes to fetch */
    $sys_attrs        = array("cn", "description", "macAddress", "objectClass", "sambaDomainName","gotoMode","FAIclass");
    $sys_categories   = array("terminal", "workstation", "server", "phone" ,"printer","incoming");

    /* Add FAIstate to attributes if FAI is activated */
    $tmp = $this->config->search("faiManagement", "CLASS",array('menu','tabs'));
    if(!empty($tmp)){
      $sys_attrs[] = "FAIstate";
    }    

    /* Walk through all possible search combinations, and search for some objects if the checkbox is enabled  */
    foreach($objs as $checkBox => $oc){

      if($this->DivListSystem->$checkBox && class_available($oc['TAB'])){
        if($this->DivListSystem->SubSearch){
          if($oc['CLASS'] != ""){
            $filter = "(&".$userregex."(objectClass=".$oc['CLASS'].")(cn=".$this->DivListSystem->Regex."))";
            $new_res = get_sub_list($filter, $sys_categories ,$oc['TREE'], $base,$sys_attrs, GL_SUBSEARCH | GL_SIZELIMIT);
            $res = array_merge($res,$new_res);
          }
        }else{
          /* User filter? */
          if($oc['CLASS'] != ""){
            $filter = "(&".$userregex."(objectClass=".$oc['CLASS'].")(cn=".$this->DivListSystem->Regex."))";
            $res = array_merge($res,get_list($filter,$sys_categories,$oc['TREE'].$base, $sys_attrs,  GL_SIZELIMIT));
          }
        }
      } 
    }

    /* Search for incoming objects */ 
    $filter = "(|(&".$userregex."(objectClass=goHard)(cn=".$this->DivListSystem->Regex.")))";
    $res = array_merge($res,get_list($filter,$sys_categories, get_ou('incomingou').$base,$sys_attrs, GL_SIZELIMIT));

    /* Append opsi systems, the opsi extension have to installed.
     */
    if($this->opsi != NULL){
      $opsi_clients = $this->opsi->get_hosts_for_system_management();
      if($this->opsi->is_error()){
        msg_dialog::display(_("Error"),msgPool::siError($this->opsi->get_error()),ERROR_DIALOG);
      }
    }
    
    /* Get all gotoTerminal's */
    $t_id = 0;
    $opsi_map  = array();
    foreach ($res as $value){

      $tmp= $value['dn'];
      $add= "";

      /* Extract base */
      foreach($objs as $obj){
        if(preg_match("/,".$obj['TREE']."/i",$value['dn'])){
          $tmp = trim( preg_replace("/^[^,]+,[^o]*".$obj['TREE']."/i","",$value['dn']));
        }
      }

      /* Create a string containing the last part of the department. */
      $dn_name = preg_replace("#^([^/]+/)*#","",convert_department_dn(@LDAP::fix($tmp)));
      if(empty($dn_name)){
        $dn_name = "/";
      }

      /* check if current object is a new one */
      if (preg_match ("/,".get_ou('incomingou')."/i", $tmp)){
        if (in_array_ics('gotoTerminal', $value['objectClass'])){
          $add= "- "._("New terminal");
        }elseif (in_array_ics('gotoWorkstation', $value['objectClass'])){
          $add= "- "._("New workstation");
        }elseif (in_array_ics('GOhard', $value['objectClass']) && !isset($value['gotoMode'])){
          $add= "- "._("Unknown device");
        }elseif (in_array_ics('GOhard', $value['objectClass'])){
          $add= "- "._("New Device");
        }
      } 

      /* Detect type of object and create an entry for $this->terminals */
      $terminal = array();

       if (0 && in_array_ics('gosa_opsi_client', $value["objectClass"])){

         /* check acl */
         $terminal             = $value;
         $terminal['type']     = "O";

       } elseif (in_array_ics('gotoTerminal', $value["objectClass"])){

        /* check acl */
        $acl = $ui->get_permissions($value['dn'],"terminal/termgeneric");
        if($add != "" || preg_match("/r/",$acl)) {
          if (isset($value["macAddress"][0]) && $value["macAddress"][0] != "-"){
            $terminal             = $value;
            $terminal['type']     = "T";
            $terminal['is_new']   = $add;
          } else {
            $terminal             = $value;
            $terminal['type']     = "D";
            $terminal['message']  = _("Terminal template for")."&nbsp;'".$dn_name."'&nbsp;";
            $terminal['location'] = array_search($tmp, $this->config->departments); 
          }
        }
      } elseif (in_array_ics('gotoWorkstation', $value["objectClass"])){

        $acl = $ui->get_permissions($value['dn'],"workstation/workgeneric");
        if($add != "" || preg_match("/r/",$acl)) {
          if (isset($value["macAddress"][0]) &&  $value["macAddress"][0] != "-"){
            $terminal             = $value;
            $terminal['type']     = "L";
            $terminal['is_new']   = $add;
          } else {
            $terminal             = $value;
            $terminal['type']     = "D";
            $terminal['location'] = array_search($tmp, $this->config->departments);
            $terminal['message']  = _("Workstation template for")."&nbsp;'".$dn_name."'&nbsp;";
          }
#          if (isset($value["FAIstate"][0])){
#            $terminal['type']= $this->getState($terminal['type'], $value["FAIstate"][0]);
#          }
        }
      } elseif (in_array_ics('gotoPrinter', $value["objectClass"])){
       
   
        $acl = $ui->get_permissions($value['dn'],"printer/printgeneric");
        if($add != "" || preg_match("/r/",$acl)) {

          $terminal             = $value;
          $terminal['type']     = "P";
        }
      } elseif (in_array_ics('goServer', $value["objectClass"])){

        $acl = $ui->get_permissions($value['dn'],"server/servgeneric");
        if($add != "" || preg_match("/r/",$acl)) {

          $terminal             = $value;
          $terminal['type']     = "S";
#          if (isset($value["FAIstate"][0])){
#            $terminal['type']= $this->getState($terminal['type'], $value["FAIstate"][0]);
#          }
        }
      } elseif (in_array_ics('goFonHardware', $value["objectClass"])){

        $acl = $ui->get_permissions($value['dn'],"phone/phoneGeneric");
        if($add != "" || preg_match("/r/",$acl)) {

          $terminal             = $value;
          $terminal['type']     = "F";
        }
      }elseif (in_array_ics("GOhard",$value['objectClass'])){

        $acl =  $ui->get_permissions($value['dn'],"server/servgeneric"). 
                $ui->get_permissions($value['dn'],"terminal/termgeneric").
                $ui->get_permissions($value['dn'],"workstation/workgeneric");
        if($add != "" || preg_match("/r/",$acl)) {

          $terminal = $value;
          $terminal['type']   = "Q";
          $terminal['is_new'] = $add;
        }
      } elseif (in_array_ics('ieee802Device', $value["objectClass"])){

        $acl = $ui->get_permissions($value['dn'],"component/componentGeneric");
        if($add != "" || preg_match("/r/",$acl)) {

          $terminal             = $value;
          $terminal['type']     = "C";
        }
      } else{

        $name= preg_replace('/\$$/', '', $value['cn'][0]);
        if (isset($value['sambaDomainName'])){
          $domain= " [".$value['sambaDomainName'][0]."]";
        } else {
          $domain= "";
        }
        $acl = $ui->get_permissions($value['dn'],"winworkstation/wingeneric");
        if($add != "" || preg_match("/r/",$acl)) {
          $terminal=$value;
          $terminal['type']     ="W";
          $terminal['domain']   = $name.$domain;
        }
      }

      /* Append collected data to the host list.
       */
      if(count($terminal)){
        $t_id ++ ;
        $this->terminals[$t_id]=$terminal;
        $opsi_map[preg_replace('/\$$/',"",$value['cn'][0])] = $t_id;
      }
    }


    /* Merge real hosts with opsi hosts.
       If there is a samba host, then merge it with the opsi host,
        to avoid duplicate entries.
     */
    foreach($opsi_clients as $entry){
      if(isset($opsi_map[$entry['cn'][0]])){
        continue;
      }
      $terminal             = $entry;
      $terminal['type']     = "O";
      $this->terminals[] = $terminal;
    }

    $tmp  =array();
    $tmp2 =array();
    foreach($this->terminals as $tkey => $val ){
      $tmp[strtolower($val['cn'][0]).$val['dn']]=$val;
      $tmp2[strtolower($val['cn'][0]).$val['dn']] = strtolower($val['cn'][0]).$val['dn'];
    }
    natcasesort($tmp2);
    $this->terminals=array();
    foreach($tmp2 as $val){
      $this->terminals[]=$tmp[$val];
    }
    reset ($this->terminals);
  }

  function remove_lock()
  {
    if (isset($this->systab->dn)){
      del_lock ($this->systab->dn);
    }
    if(isset($this->dn) && !empty($this->dn) && $this->dn != "new"){
      del_lock($this->dn);
    }
    if(isset($this->dns) && is_array($this->dns) && count($this->dns)){
      del_lock($this->dns);
    }
  }


  function copyPasteHandling_from_queue($s_action,$s_entry)
  {
    /* Check if Copy & Paste is disabled */
    if(!is_object($this->CopyPasteHandler)){
      return("");
    }

    $ui = get_userinfo();
    
    $tabs = $this->get_tab_defs();

    /* Add a single entry to queue */
    if($s_action == "cut" || $s_action == "copy"){

      /* Cleanup object queue */
      $this->CopyPasteHandler->cleanup_queue();
      $dn     = $this->terminals[$s_entry]['dn'];
      $oc     = $this->terminals[$s_entry]['objectClass'];
      $type   = $this->get_system_type($this->terminals[$s_entry]);

      $tab_o  = $tabs[$type]['CLASS'];
      $tab_c  = $tabs[$type]['TABCLASS'];
      $acl_c  = $tabs[$type]['TABNAME'];
      $acl    = $tabs[$type]['ACLC'];

      if($s_action == "copy" && $ui->is_copyable($dn,$acl,$acl_c)){
        $this->CopyPasteHandler->add_to_queue($dn,$s_action,$tab_c,$tab_o,$acl);
      }
      if($s_action == "cut" && $ui->is_cutable($dn,$acl,$acl_c)){
        $this->CopyPasteHandler->add_to_queue($dn,$s_action,$tab_c,$tab_o,$acl);
      }
    }

    /* Add entries to queue */
    if($s_action == "copy_multiple" || $s_action == "cut_multiple"){

      /* Cleanup object queue */
      $this->CopyPasteHandler->cleanup_queue();

      /* Add new entries to CP queue */
      foreach($this->list_get_selected_items() as $id){
        $dn = $this->terminals[$id]['dn'];
        $oc = $this->terminals[$id]['objectClass']; 
        $type = $this->get_system_type($this->terminals[$id]);

        if(isset($tabs[$type])){
          $tab_o  = $tabs[$type]['CLASS'];
          $tab_c  = $tabs[$type]['TABCLASS'];
          $acl_c  = $tabs[$type]['TABNAME'];
          $acl    = $tabs[$type]['ACLC'];

          if($s_action == "copy_multiple" && $ui->is_copyable($dn,$acl,$acl_c)){ 
            $this->CopyPasteHandler->add_to_queue($dn,"copy",$tab_c,$tab_o,$acl);
          }
          if($s_action == "cut_multiple" && $ui->is_cutable($dn,$acl,$acl_c)){
            $this->CopyPasteHandler->add_to_queue($dn,"cut",$tab_c,$tab_o,$acl);
          }
        }
      }
    }

    /* Start pasting entries */
    if($s_action == "editPaste"){
      $this->start_pasting_copied_objects = TRUE;
    }
  
    /* Return C&P dialog */
    if($this->start_pasting_copied_objects && $this->CopyPasteHandler->entries_queued()){

      /* Get dialog */
      $this->CopyPasteHandler->SetVar("base",$this->DivListSystem->selectedBase);
      $data = $this->CopyPasteHandler->execute();

      /* Return dialog data */
      if(!empty($data)){
        return($data);
      }
    }

    /* Automatically disable status for pasting */
    if(!$this->CopyPasteHandler->entries_queued()){
      $this->start_pasting_copied_objects = FALSE;
    }
    return("");
  }


  function get_system_type($attrs)
  {
    $classes = $attrs['objectClass'];

    $type= "";
    if (in_array_ics('gosa_opsi_client', $classes)){
      $type= "opsi_client";
    }elseif (in_array_ics('ieee802Device', $classes)){
      $type= "component";
    }elseif (in_array_ics('gotoTerminal', $classes)){
      $type= "terminal";
    }elseif (in_array_ics('gotoWorkstation', $classes)){
      $type= "workstation";
    }elseif (in_array_ics('gotoPrinter', $classes)){
      $type= "printer";
    }elseif (in_array_ics('goFonHardware', $classes)){
      $type= "phone";
    }elseif (in_array_ics('goServer', $classes)){
      $type= "server";
    }elseif (in_array_ics('GOhard', $classes) && !isset($attrs['gotoMode']) && $this->arp_handling_active){
      $type= "ArpNewDevice";
    }elseif (in_array_ics('GOhard', $classes)){
      $type= "NewDevice";
    }elseif (in_array_ics('sambaAccount', $classes) ||
        in_array_ics('sambaSamAccount', $classes)){
      $type= "winstation";
    }
    return ($type);
  }


  function convert_list($input)
  {
    $temp= "";

    $conv= array(
        "D" => array("plugins/systems/images/select_default.png",_("Template")),
        "F" => array("plugins/systems/images/select_phone.png",_("Phone")),
        "C" => array("plugins/systems/images/select_component.png",_("Network device")),
        "P" => array("plugins/systems/images/select_printer.png",_("Printer")),

        "W" => array("plugins/systems/images/select_winstation.png",_("Win workstation")),

        "L" => array("plugins/systems/images/select_workstation.png",_("Workstation")),
        "S" => array("plugins/systems/images/select_server.png",_("Server")),
        "T" => array("plugins/systems/images/select_terminal.png",_("Terminal")),

        "LX" => array("plugins/systems/images/workstation_locked.png",_("Locked workstation")),
        "SX" => array("plugins/systems/images/server_locked.png",_("Locked server")),
        "TX" => array("plugins/systems/images/terminal_locked.png",_("Locked terminal")),

        "LE" => array("plugins/systems/images/workstation_error.png",_("Workstation error")),
        "SE" => array("plugins/systems/images/server_error.png",_("Server error")),
        "TE" => array("plugins/systems/images/terminal_error.png",_("Terminal error")),

        "LB" => array("plugins/systems/images/workstation_busy.png",_("Workstation busy")),
        "SB" => array("plugins/systems/images/server_busy.png",_("Server busy")),

        "NQ" => array("plugins/systems/images/select_newsystem.png",_("New system from incoming")),
        "NT" => array("plugins/systems/images/select_new_terminal.png",_("New terminal")),
        "NL" => array("plugins/systems/images/select_new_workstation.png",_("New workstation")));

    /* Add opsi client to system types */
    if($this->opsi != NULL){
        $conv["O"] = array("plugins/systems/images/select_default.png",_("Opsi client"));
    }

    /* Use locked icons 
     */
    if( in_array($input['type'],array("S","T","L")) && 
        isset($input['gotoMode'][0]) && 
        preg_match("/locked/",$input['gotoMode'][0])){
      $input['type'].="X";
    } elseif(in_array($input['type'],array("S","L")) &&
        isset($input['FAIstate'][0])){

    /* Add FAI state icons 
     */
      $type= "";
      switch (preg_replace('/:.*$/', '', $input['FAIstate'][0])) {
        case 'error':
          $type= 'E';
          break;
        case 'installing':
        case 'install':
        case 'sysinfo':
        case 'softupdate':
        case 'scheduledupdate':
          $type= 'B';
          break;
      }

      $input['type'].= $type;
    }   

    /* Use new images if object is from incoming 
     */
    if((isset($input['is_new']))&&(!empty($input['is_new']))){
      $input['type']="N".$input['type'];
    }

    /* Check which flags this objects uses 
     */
    foreach ($conv  as $key => $value){
      $found = TRUE;
      if(strlen($key) != strlen($input['type'])) {
        $found = FALSE;
      }
      for($i = 0 ; $i < strlen($key) ; $i++){
        if(!preg_match("/".$key[$i]."/",$input['type'])){
          $found = FALSE;
        }
     }

      if($found){
        $tmp['img'] ="<img class='center' src='".$value[0]."' alt='".$key."' title='".$value['1']."'>";
        $tmp['class']=$key;
        return $tmp;
      }
    }
  }


  function list_get_selected_items()
  {
    $ids = array();
    foreach($_POST as $name => $value){
      if(preg_match("/^item_selected_[0-9]*$/",$name)){
        $id   = preg_replace("/^item_selected_/","",$name);
        $ids[$id] = $id;
      }
    }
    return($ids);
  }


  public function get_tab_defs()
  {
    $tabs = array(
        "incoming"    => array(
          "CLASS"     => "",
          "TABNAME"   => "",
          "TABCLASS"  => "",
          "ACLC"      => "incoming",
          "ACL"       => "incoming/systems"),

        "ArpNewDevice"=> array(
          "CLASS"     => "TERMTABS",
          "TABNAME"   => "termgeneric" ,
          "TABCLASS"  => "termtabs",
          "ACLC"      => "incoming",
          "ACL"       => "incoming/systems"),

        "NewDevice"   => array(
          "CLASS"     => "TERMTABS",
          "TABNAME"   => "termgeneric",
          "TABCLASS"  => "termtabs",
          "ACLC"      => "incoming",
          "ACL"       => "incoming/systems"),

        "terminal"    => array(
            "CLASS"     => "TERMTABS",
            "TABNAME"   => "termgeneric",
            "TABCLASS"  => "termtabs",
            "ACLC"      => "terminal",
            "ACL"       => "terminal/termgeneric"),

        "workstation" =>  array(
            "CLASS"   => "WORKTABS",
            "TABNAME" => "workgeneric",
            "TABCLASS"=> "worktabs",
            "ACLC"    => "workstation",
            "ACL"     => "workstation/workgeneric" ),

        "server"      => array(
            "CLASS"   => "SERVTABS",
            "TABNAME" => "servgeneric",
            "TABCLASS"=> "servtabs",
            "ACLC"    => "server",
            "ACL"     => "server/servgeneric"),

        "printer"     => array(
            "CLASS"   => "PRINTTABS",
            "TABNAME" => "printgeneric",
            "TABCLASS"=> "printtabs",
            "ACLC"    => "printer",
            "ACL"     => "printer/printgeneric"),

        "phone"       => array(
            "CLASS"   => "PHONETABS",
            "TABNAME" => "phoneGeneric",
            "TABCLASS"=> "phonetabs",
            "ACLC"    => "phone",
            "ACL"     => "phone/phoneGeneric"),

        "winstation"  => array(
            "CLASS"   => "WINTABS",
            "TABNAME" => "wingeneric",
            "TABCLASS"=> "wintabs",
            "ACLC"    => "winworkstation",
            "ACL"     => "winworkstation/wingeneric"),

        "component"   => array(
            "CLASS"   => "COMPONENTTABS",
            "TABNAME" => "componentGeneric",
            "TABCLASS"=> "componenttabs",
            "ACLC"    => "component",
            "ACL"     => "component/componentGeneric"));

    if($this->opsi != NULL){
      $tabs["opsi_client"] = array(
          "CLASS"   => "OPSITABS",
          "TABNAME" => "opsiGeneric" ,
          "TABCLASS"=> "opsi_tabs",
          "ACLC"    => "opsi" ,
          "ACL"     => "opsi/opsiGeneric");
    }

    return($tabs);
  }



  /* !! Incoming dummy acls, required to defined acls for incoming objects
   */
  static function plInfo()
  {
    return (array(
          "plShortName"   => _("Incoming objects"),
          "plDescription" => _("Incoming objects"),
          "plSelfModify"  => FALSE,
          "plDepends"     => array(),
          "plPriority"    => 99,
          "plSection"     => array("administration"),
          "plCategory"    => array("incoming"   => array( "description"  => _("Incoming"),
                                                          "objectClass"  => "")),
          "plProvidedAcls"=> array()
            
          ));
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
