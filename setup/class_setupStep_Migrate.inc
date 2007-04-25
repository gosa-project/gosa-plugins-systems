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
  var $migration_dialog   = FALSE;
  var $deps_to_migrate    = array();

  

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
    $name     = "GOsa_setup_text_entry_".session_id().rand(0,999999);
    $dn       = "ou=".$name.",".$cv['base'];
    $testEntry= array();
    $testEntry['objectClass'][]= "top";
    $testEntry['objectClass'][]= "organizationalUnit";
    $testEntry['objectClass'][]= "gosaDepartment";
    $testEntry['description']= "Created by GOsa setup, this object can be removed.";
    $testEntry['ou']  = $name;
 
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

    $this->checks['permissions']['STATUS']    = TRUE;
    $this->checks['permissions']['STATUS_MSG']= _("Ok");
    $this->checks['permissions']['ERROR_MSG'] = "<input type='submit' name='retry_permissions' value='"._("Retry")."'>";
    return(true);
  } 


  function check_visible_organizationalUnits()
  {
    $this->deps_to_migrate = array();
    $cnt_ok = 0;

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
      $this->deps_to_migrate[] = $attrs;
    }
  
    /* Filter returned list of departments */
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
  
    /* No invisible */
    if(count($this->deps_to_migrate) == 0){
      $this->checks['deps_visible']['STATUS']    = TRUE;
      $this->checks['deps_visible']['STATUS_MSG']= _("Ok");
      $this->checks['deps_visible']['ERROR_MSG'] = "";
      $this->checks['deps_visible']['ERROR_MSG'] .= "<input type='submit' name='deps_visible_migrate_refresh' value='"._("Retry")."'>";
    }else{
      $this->checks['deps_visible']['STATUS']    = FALSE;
      $this->checks['deps_visible']['STATUS_MSG']= sprintf(_("%s entries found"),count($this->deps_to_migrate));
      $this->checks['deps_visible']['ERROR_MSG'] = sprintf(_("Found %s departments that will not be visible in GOsa."),count($this->deps_to_migrate));
      $this->checks['deps_visible']['ERROR_MSG'] .= "<input type='submit' name='deps_visible_migrate' value='"._("Migrate")."'>";
      $this->checks['deps_visible']['ERROR_MSG'] .= "<input type='submit' name='deps_visible_migrate_refresh' value='"._("Reload list")."'>";
    }

  }


    
  /* Start deparmtment migration */  
  function migrate_organizationalUnits()
  {
    /* Get collected configuration settings */
    $cv = $this->parent->captured_values;

    /* Establish ldap connection */
    $ldap = new LDAP($cv['admin'],
                     $cv['password'],
                     $cv['connection'],
                     FALSE,
                     $cv['tls']);

    foreach($this->deps_to_migrate as $dep){
      if($dep['checked']){

        $ldap->cat($dep['dn'],array("objectClass","description"));
        $attrs      = $ldap->fetch();
        $new_attrs  = array();

        for($i = 0 ; $i < $attrs['objectClass']['count']; $i ++ ){
          $new_attrs['objectClass'][]   = $attrs['objectClass'][$i];
        }
        $new_attrs['objectClass'][] = "gosaDepartment";
    
        if(!isset($attrs['description'])){
          $new_attrs['description'][] = "GOsa department";
        }
        
        $ldap->cd($attrs['dn']);
        if(!$ldap->modify($new_attrs)){
          print_red(sprintf(_("Failed to migrate the department '%s' into GOsa, error message is as follows '%s'."),$attrs['dn'],$ldap->get_error()));
          return(false);
        }
      }
    }
    return(TRUE);
  }



  function execute()
  {
    /* Permission check */
    $this->check_ldap_permissions();

    /* Migration options 
     */
    /* Refresh list of deparments */
    if(isset($_POST['deps_visible_migrate_refresh'])){
      $this->check_visible_organizationalUnits();
    }

    /* Open migration dialog */
    if(isset($_POST['deps_visible_migrate'])){
      $this->migration_dialog = TRUE;
      $this->dialog =TRUE;
    }

    /* Close migration dialog */
    if(isset($_POST['deps_visible_migrate_close'])){
      $this->migration_dialog = FALSE;
      $this->dialog =FALSE;
    }

    /* Start migration */
    if(isset($_POST['deps_visible_migrate_migrate'])){
      if($this->migrate_organizationalUnits()){
        $this->check_visible_organizationalUnits();
      }
    }

    /* Display migration dialog */
    if($this->migration_dialog){
      $smarty = get_smarty();
      $smarty->assign("deps_to_migrate",$this->deps_to_migrate);
      $smarty->assign("method","migrate");
      return($smarty->fetch(get_template_path("setup_migrate.tpl",TRUE,dirname(__FILE__))));
    }

    $smarty = get_smarty();
    $smarty->assign("checks",$this->checks);
    $smarty->assign("method","default");
    return($smarty->fetch(get_template_path("setup_migrate.tpl",TRUE,dirname(__FILE__))));
  }

  function save_object()
  {
    if($this->migration_dialog){
      foreach($this->deps_to_migrate as $id => $data){

        if(isset($_POST['migrate_'.$id])){
          $this->deps_to_migrate[$id]['checked'] = TRUE;
        }else{
          $this->deps_to_migrate[$id]['checked'] = FALSE;
        }
      }
    }

  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
