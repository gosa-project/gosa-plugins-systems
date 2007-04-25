<?php

/*
   This code is part of GOsa (https://gosa.gonicus.de)
   Copyright (C) 2007 Fabian Hickert

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


class Step_Migrate extends setup_step
{
  var $languages      = array();
  var $attributes     = array();
  var $header_image   = "images/monitoring.png";
  var $checks         = array();

  /* Department migration attributes */
  var $dep_migration_dialog = FALSE;
  var $deps_to_migrate      = array();

  /* Department migration attributes */
  var $users_migration_dialog= FALSE;
  var $users_to_migrate      = array();

  function Step_Migrate()
  {
    $this->update_strings(); 
    $this->initialize_checks();
  }

  function update_strings()
  {
    $this->s_title      = _("LDAP inspection");
    $this->s_title_long = _("LDAP inspection");
    $this->s_info       = _("Analyze your current LDAP for GOsa compatibility");
  }

  function initialize_checks()
  {
    $this->checks = array();
    $this->checks['permissions']['TITLE']     = _("Checking permissions on ldap database");
    $this->checks['permissions']['STATUS']    = FALSE;
    $this->checks['permissions']['STATUS_MSG']= "";
    $this->checks['permissions']['ERROR_MSG'] = "";
    $this->check_ldap_permissions();

    $this->checks['deps_visible']['TITLE']     = _("Checking for invisible deparmtments");
    $this->checks['deps_visible']['STATUS']    = FALSE;
    $this->checks['deps_visible']['STATUS_MSG']= "";
    $this->checks['deps_visible']['ERROR_MSG'] = "";
    $this->check_visible_organizationalUnits();

    $this->checks['users_visible']['TITLE']     = _("Checking for invisible user");
    $this->checks['users_visible']['STATUS']    = FALSE;
    $this->checks['users_visible']['STATUS_MSG']= "";
    $this->checks['users_visible']['ERROR_MSG'] = "";
    $this->check_invisible_gosaAccounts();
  }


  /* Check ldap accessibility 
   * Create and remove a dummy object, 
   *  to ensure that we have the necessary permissions
   */
  function check_ldap_permissions()
  {
    $cv = $this->parent->captured_values;
    $ldap = new LDAP($cv['admin'],
        $cv['password'],
        $cv['connection'],
        FALSE,
        $cv['tls']);

    /* Create dummy entry 
     */
    $name     = "GOsa_setup_text_entry_".session_id().rand(0,999999);
    $dn       = "ou=".$name.",".$cv['base'];
    $testEntry= array();
    $testEntry['objectClass'][]= "top";
    $testEntry['objectClass'][]= "organizationalUnit";
    $testEntry['objectClass'][]= "gosaDepartment";
    $testEntry['description']= "Created by GOsa setup, this object can be removed.";
    $testEntry['ou']  = $name;

    /* Try to create dummy object 
     */ 
    $ldap->cd ($dn);
    $res = $ldap->add($testEntry);
    if(!$res){
      $this->checks['permissions']['STATUS']    = FALSE;
      $this->checks['permissions']['STATUS_MSG']= _("Failed");
      $this->checks['permissions']['ERROR_MSG'] = 
        sprintf(_("The specified user '%s' does not have full access to your ldap database."),$cv['admin']);
      $this->checks['permissions']['ERROR_MSG'].=
        "<input type='submit' name='retry_permissions' value='"._("Retry")."'>";
      return(false);
    }

    /* Try to remove created entry 
     */
    $res = $ldap->rmDir($dn);
    if(!$res){
      $this->checks['permissions']['STATUS']    = FALSE;
      $this->checks['permissions']['STATUS_MSG']= _("Failed");
      $this->checks['permissions']['ERROR_MSG'] = 
        sprintf(_("The specified user '%s' does not have full access to your ldap database."),$cv['admin']);
      $this->checks['permissions']['ERROR_MSG'].=
        "<input type='submit' name='retry_permissions' value='"._("Retry")."'>";
      return(false);
    }

    /* Create & remove of dummy object was successful */
    $this->checks['permissions']['STATUS']    = TRUE;
    $this->checks['permissions']['STATUS_MSG']= _("Ok");
    $this->checks['permissions']['ERROR_MSG'] = "<input type='submit' name='retry_permissions' value='"._("Retry")."'>";
    return(true);
  } 


  /* Check if there are users which will 
   *  be invisible for GOsa 
   */
  function check_invisible_gosaAccounts()
  {
    /* Remember old list of ivisible users, to be able to set 
     *  the 'html checked' status for the checkboxes again 
     */
    $cnt_ok = 0;
    $old    = $this->users_to_migrate;
    $this->users_to_migrate = array();

    /* Get collected configuration settings */
    $cv = $this->parent->captured_values;

    /* Establish ldap connection */
    $ldap = new LDAP($cv['admin'],
        $cv['password'],
        $cv['connection'],
        FALSE,
        $cv['tls']);

    /* Get all invisible users 
     */
    $ldap->cd($cv['base']); 
    $ldap->search("(&(|(objectClass=posixAccount)(objectClass=inetOrgPerson)(objectClass=organizationalPerson))(!(objectClass=gosaAccount)))",array("sn","givenName","cn","uid"));
    while($attrs = $ldap->fetch()){
      if(!preg_match("/,dc=addressbook,/",$attrs['dn'])){
        $attrs['checked'] = FALSE;
        $attrs['before']  = "";
        $attrs['after']   = "";

        /* Set objects to selected, that were selected before reload */
        if(isset($old[base64_encode($attrs['dn'])])){
          $attrs['checked'] = $old[base64_encode($attrs['dn'])]['checked'];
        }
        $this->users_to_migrate[base64_encode($attrs['dn'])] = $attrs;
      }
    }

    /* No invisible */
    if(count($this->users_to_migrate) == 0){
      $this->checks['users_visible']['STATUS']    = TRUE;
      $this->checks['users_visible']['STATUS_MSG']= _("Ok");
      $this->checks['users_visible']['ERROR_MSG'] = "";
      $this->checks['users_visible']['ERROR_MSG'] .= "<input type='submit' name='users_visible_migrate_refresh' value='"._("Retry")."'>";
    }else{
      $this->checks['users_visible']['STATUS']    = FALSE;
      $this->checks['users_visible']['STATUS_MSG']= "";
      $this->checks['users_visible']['ERROR_MSG'] = sprintf(_("Found %s users that will not be visible in GOsa."), 
          count($this->users_to_migrate));
      $this->checks['users_visible']['ERROR_MSG'] .= "<input type='submit' name='users_visible_migrate' value='"._("Migrate")."'>";
      $this->checks['users_visible']['ERROR_MSG'] .= "<input type='submit' name='users_visible_migrate_refresh' value='"._("Reload list").   "'>";
    }
  }


  /* Start user account migration 
   */  
  function migrate_gosaAccounts($only_ldif = FALSE)
  {
    /* Get collected configuration settings */
    $cv = $this->parent->captured_values;

    /* Establish ldap connection */
    $ldap = new LDAP($cv['admin'],
        $cv['password'],
        $cv['connection'],
        FALSE,
        $cv['tls']);

    /* Add gosaAccount objectClass to the selected users  
     */
    foreach($this->users_to_migrate as $key => $dep){
      if($dep['checked']){

        /* Get old objectClasses */
        $ldap->cat($dep['dn'],array("objectClass"));
        $attrs      = $ldap->fetch();

        /* Create new objectClass array */
        $new_attrs  = array();
        for($i = 0 ; $i < $attrs['objectClass']['count']; $i ++ ){
          $new_attrs['objectClass'][]   = $attrs['objectClass'][$i];
        }
        $new_attrs['objectClass'][] = "gosaAccount";

        /* Set info attributes for current object, 
         *  or write changes to the ldap database 
         */
        if($only_ldif){
          $this->users_to_migrate[$key]['before'] = $this->array_to_ldif($attrs);
          $this->users_to_migrate[$key]['after']  = $this->array_to_ldif($new_attrs);
        }else{
          $ldap->cd($attrs['dn']);
          if(!$ldap->modify($new_attrs)){
            print_red(sprintf(_("Failed to migrate the department '%s' into GOsa, error message is as follows '%s'."),$attrs['dn'],$ldap->get_error()));
            return(false);
          }
        }
      }
    }
    return(TRUE);
  }


  /* Check if there are invisible organizational Units 
   */
  function check_visible_organizationalUnits()
  {
    $cnt_ok = 0;
    $old = $this->deps_to_migrate;
    $this->deps_to_migrate = array();

    /* Get collected configuration settings */
    $cv = $this->parent->captured_values;

    /* Establish ldap connection */
    $ldap = new LDAP($cv['admin'],
        $cv['password'],
        $cv['connection'],
        FALSE,
        $cv['tls']);

    /* Skip GOsa internal departments */
    $skip_dns = array("/^ou=people,/","/^ou=groups,/","/(,|)ou=configs,/","/(,|)ou=systems,/",
        "/^ou=apps,/","/^ou=mime,/","/^ou=aclroles,/","/^ou=incoming,/",
        "/ou=snapshots,/","/(,|)dc=addressbook,/","/^(,|)ou=machineaccounts,/",
        "/(,|)ou=winstations,/");


    /* Get all invisible departments */
    $ldap->cd($cv['base']); 
    $ldap->search("(&(objectClass=organizationalUnit)(!(objectClass=gosaDepartment)))",array("ou","description","dn"));
    while($attrs = $ldap->fetch()){
      $attrs['checked'] = FALSE;
      $attrs['before']  = "";
      $attrs['after']   = "";

      /* Set objects to selected, that were selected before reload */
      if(isset($old[base64_encode($attrs['dn'])])){
        $attrs['checked'] = $old[base64_encode($attrs['dn'])]['checked'];
      }
      $this->deps_to_migrate[base64_encode($attrs['dn'])] = $attrs;
    }

    /* Filter returned list of departments and ensure that 
     *  GOsa internal departments will not be listed 
     */
    foreach($this->deps_to_migrate as $key => $attrs){
      $dn = $attrs['dn'];
      $skip = false;
      foreach($skip_dns as $skip_dn){
        if(preg_match($skip_dn,$dn)){
          $skip = true;
        }
      }
      if($skip){
        unset($this->deps_to_migrate[$key]);
      }
    }

    /* If we have no invisible departments found  
     *  tell the user that everything is ok 
     */
    if(count($this->deps_to_migrate) == 0){
      $this->checks['deps_visible']['STATUS']    = TRUE;
      $this->checks['deps_visible']['STATUS_MSG']= _("Ok");
      $this->checks['deps_visible']['ERROR_MSG'] = "";
      $this->checks['deps_visible']['ERROR_MSG'] .= "<input type='submit' name='deps_visible_migrate_refresh' value='"._("Retry")."'>";
    }else{
      $this->checks['deps_visible']['STATUS']    = FALSE;
      $this->checks['deps_visible']['STATUS_MSG']= "";//sprintf(_("%s entries found"),count($this->deps_to_migrate));
      $this->checks['deps_visible']['ERROR_MSG'] = sprintf(_("Found %s departments that will not be visible in GOsa."),count($this->deps_to_migrate));
      $this->checks['deps_visible']['ERROR_MSG'] .= "<input type='submit' name='deps_visible_migrate' value='"._("Migrate")."'>";
      $this->checks['deps_visible']['ERROR_MSG'] .= "<input type='submit' name='deps_visible_migrate_refresh' value='"._("Reload list")."'>";
    }
  }



  /* Start deparmtment migration */  
  function migrate_organizationalUnits($only_ldif = FALSE)
  {
    /* Get collected configuration settings */
    $cv = $this->parent->captured_values;

    /* Establish ldap connection */
    $ldap = new LDAP($cv['admin'],
        $cv['password'],
        $cv['connection'],
        FALSE,
        $cv['tls']);

    /* Add gosaDepartment objectClass to each selected entry 
     */
    foreach($this->deps_to_migrate as $key => $dep){
      if($dep['checked']){

        /* Get current objectClasses */
        $ldap->cat($dep['dn'],array("objectClass","description"));
        $attrs      = $ldap->fetch();

        /* Create new objectClass attribute including gosaDepartment*/
        $new_attrs  = array();
        for($i = 0 ; $i < $attrs['objectClass']['count']; $i ++ ){
          $new_attrs['objectClass'][]   = $attrs['objectClass'][$i];
        }
        $new_attrs['objectClass'][] = "gosaDepartment";

        /* Append description it is missing */
        if(!isset($attrs['description'])){
          $new_attrs['description'][] = "GOsa department";
        }

        /* Depending on the parameter >only_diff< we save the changes as ldif
         *  or we write our changes directly to the ldap database
         */
        if($only_ldif){
          $this->deps_to_migrate[$key]['before'] = $this->array_to_ldif($attrs);
          $this->deps_to_migrate[$key]['after']  = $this->array_to_ldif($new_attrs);
        }else{
          $ldap->cd($attrs['dn']);
          if(!$ldap->modify($new_attrs)){
            print_red(sprintf(_("Failed to migrate the department '%s' into GOsa, error message is as follows '%s'."),$attrs['dn'],$ldap->get_error()));
            return(false);
          }
        }
      }
    }
    return(TRUE);
  }



  function execute()
  {
    /* Permission check */
    $this->check_ldap_permissions();


    /*************
     * User Migration handling 
     *************

    /* Refresh list of deparments */
    if(isset($_POST['users_visible_migrate_refresh'])){
      $this->check_invisible_gosaAccounts();
    }

    /* Open migration dialog */
    if(isset($_POST['users_visible_migrate'])){
      $this->users_migration_dialog = TRUE;
      $this->dialog =TRUE;
    }

    /* Close migration dialog */
    if(isset($_POST['users_visible_migrate_close'])){
      $this->users_migration_dialog = FALSE;
      $this->dialog =FALSE;
    }

    /* Start migration */
    if(isset($_POST['users_visible_migrate_migrate'])){
      if($this->migrate_gosaAccounts()){
        $this->check_invisible_gosaAccounts();
      }
    }

    /* Start migration */
    if(isset($_POST['users_visible_migrate_whatsdone'])){
      $this->migrate_gosaAccounts(TRUE);
    }

    /* Display migration dialog */
    if($this->users_migration_dialog){
      $smarty = get_smarty();
      $smarty->assign("users_to_migrate",$this->users_to_migrate);
      $smarty->assign("method","migrate_users");
      return($smarty->fetch(get_template_path("setup_migrate.tpl",TRUE,dirname(__FILE__))));
    }


    /*************
     * Department Migration handling 
     *************

    /* Refresh list of deparments */
    if(isset($_POST['deps_visible_migrate_refresh'])){
      $this->check_visible_organizationalUnits();
    }

    /* Open migration dialog */
    if(isset($_POST['deps_visible_migrate'])){
      $this->dep_migration_dialog = TRUE;
      $this->dialog =TRUE;
    }

    /* Close migration dialog */
    if(isset($_POST['deps_visible_migrate_close'])){
      $this->dep_migration_dialog = FALSE;
      $this->dialog =FALSE;
    }

    /* Start migration */
    if(isset($_POST['deps_visible_migrate_migrate'])){
      if($this->migrate_organizationalUnits()){
        $this->check_visible_organizationalUnits();
      }
    }

    /* Start migration */
    if(isset($_POST['deps_visible_migrate_whatsdone'])){
      $this->migrate_organizationalUnits(TRUE);
    }

    /* Display migration dialog */
    if($this->dep_migration_dialog){
      $smarty = get_smarty();
      $smarty->assign("deps_to_migrate",$this->deps_to_migrate);
      $smarty->assign("method","migrate_deps");
      return($smarty->fetch(get_template_path("setup_migrate.tpl",TRUE,dirname(__FILE__))));
    }

    $smarty = get_smarty();
    $smarty->assign("checks",$this->checks);
    $smarty->assign("method","default");
    return($smarty->fetch(get_template_path("setup_migrate.tpl",TRUE,dirname(__FILE__))));
  }

  function save_object()
  {
    /* Get selected departments */
    if($this->dep_migration_dialog){
      foreach($this->deps_to_migrate as $id => $data){
        if(isset($_POST['migrate_'.$id])){
          $this->deps_to_migrate[$id]['checked'] = TRUE;
        }else{
          $this->deps_to_migrate[$id]['checked'] = FALSE;
        }
      }
    }

    /* Get selected users */
    if($this->users_migration_dialog){
      foreach($this->users_to_migrate as $id => $data){
        if(isset($_POST['migrate_'.$id])){
          $this->users_to_migrate[$id]['checked'] = TRUE;
        }else{
          $this->users_to_migrate[$id]['checked'] = FALSE;
        }
      }
    }

  }


  /* Return ldif information for a 
   * given attribute array 
   */
  function array_to_ldif($atts)
  {
    $ret = "";
    unset($atts['count']);
    unset($atts['dn']);
    foreach($atts as $name => $value){
      if(is_numeric($name)) {
        continue;
      }
      if(is_array($value)){
        unset($value['count']);
        foreach($value as $a_val){
          if(!preg_match('/^[a-z0-9+@#.=, \/ -]+$/i', $a_val)){
            $ret .= $name.":: ". base64_encode($a_val)."\n";
          }else{
            $ret .= $name.": ". $a_val."\n";
          }
        }
      }else{
        if(!preg_match('/^[a-z0-9+@#.=, \/ -]+$/i', $value)){
          $ret .= $name.": ". base64_encode($value)."\n";
        }else{
          $ret .= $name.": ". $value."\n";
        }
      }
    }
    return(preg_replace("/\n$/","",$ret));
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
