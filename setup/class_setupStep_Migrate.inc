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
    $this->checks['permissions']['CHK_FUNC']  = "check_ldap_permissions";
    $this->checks['permissions']['ERROR_MSG'] = _("The specified user '%s' does not have full access to your ldap database.");
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
      return(false);
    }

    $res = $ldap->rmDir($dn);
    if(!$res){
      $this->checks['permissions']['STATUS']    = FALSE;
      $this->checks['permissions']['STATUS_MSG']= _("Failed");
      $this->checks['permissions']['ERROR_MSG'] = 
        sprintf(_("The specified user '%s' does not have full access to your ldap database."),$cv['admin']);
      return(false);
    }

    $this->checks['permissions']['STATUS']    = TRUE;
    $this->checks['permissions']['STATUS_MSG']= _("Ok");
    $this->checks['permissions']['ERROR_MSG'] = "";
    return(true);
  } 


  function check_visible_organizationalUnits()
  {

  }


  function execute()
  {
    $smarty = get_smarty();
    $smarty->assign("checks",$this->checks);
    return($smarty->fetch(get_template_path("setup_migrate.tpl",TRUE,dirname(__FILE__))));
  }

  function save_object()
  {
    $this->is_completed = TRUE;

    $once = TRUE;
    foreach($_POST as $name => $value){
        
      if(preg_match("/^retry_/",$name) && $once){
        $once   = FALSE;
        $check  = preg_replace("/^retry_/","",$name);
        $func = $this->checks[$check]['CHK_FUNC'];
        $this->checks[$check]['STATUS']=$this->$func();
      }
    }
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
