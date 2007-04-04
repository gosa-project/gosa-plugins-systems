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


class setup_step_6a extends setup_step
{
  var $optional = array(
              "enableCopyPaste" => false,
              "forceglobals" => false,
              "forcessl" => false,
              "warnssl" => false,
              "ppd_path" => "/var/spool/ppd/",
              "ppd_path_active" => FALSE,
              "kde_applications_menu" => "",
              "kde_applications_menu_active" => FALSE,
              "compile" =>  "/var/spool/gosa",
              "debuglevel" => 0,
              "session_lifetime" => 7200,
              "max_ldap_query_time" => "5.0",
              "max_ldap_query_time_active" => FALSE,
              "smbhash" => 'SMBHASH');


  function setup_step_6a()
  {
    $this->update_strings();
  }


  function update_strings()
  {
    $this->s_title      = _("GOsa settings 3/3");
    $this->s_title_long = _("GOsa generic settings, page 3/3");
    $this->s_info       = _("This dialog allows you to setup GOsa behaviour");
  }
  

  function execute()
  {
    $smarty = get_smarty();
    $smarty->assign("bool",array(FALSE => _("No"), TRUE => _("Yes")));

    $smarty->assign("optional", $this->optional);

    foreach($this->attributes as $attr){
      $smarty->assign($attr,$this->$attr);
    }

    $smarty->assign("warnings" ,$this->check());
    $smarty->assign("warnings_cnt" ,count($this->check()));

    return($smarty -> fetch (get_template_path("../setup/setup_step6a.tpl")));
  }


  function save_object()
  {
    if(isset($_POST['step6a_posted'])){

      /* Get attributes */
      foreach($this->attributes as $attr){
        if(isset($_POST[$attr])){
          $this->$attr = validate($_POST[$attr]);
        }
      }
      
      foreach(array("enableCopyPaste","forceglobals","forcessl","warnssl","compile","debuglevel","session_lifetime","smbhash") as $name){
        if(isset($_POST[$name])){
          $this->optional[$name] = $_POST[$name];
        }
      } 

      if(isset($_POST['ppd_path_active'])){
        $this->optional['ppd_path_active'] = TRUE;
        if(isset($_POST['ppd_path'])){
          $this->optional['ppd_path'] = $_POST['ppd_path'];
        }
      }else{
        $this->optional['ppd_path_active'] = FALSE;
      }

      if(isset($_POST['kde_applications_menu_active'])){
        $this->optional['kde_applications_menu_active'] = TRUE;
        if(isset($_POST['kde_applications_menu'])){
          $this->optional['kde_applications_menu'] = $_POST['kde_applications_menu'];
        }
      }else{
        $this->optional['kde_applications_menu_active'] = FALSE;
      }

      if(isset($_POST['max_ldap_query_time_active'])){
        $this->optional['max_ldap_query_time_active'] = TRUE;
        if(isset($_POST['max_ldap_query_time'])){
          $this->optional['max_ldap_query_time'] = $_POST['max_ldap_query_time'];
        }
      }else{
        $this->optional['max_ldap_query_time_active'] = FALSE;
      }

    }

    $tmp = $this->check();
    if(count($tmp) == 0){
      $this->is_completed = TRUE;
    }else{
      $this->is_completed = FALSE;
    }
  }


  function check()
  {
    $message = array();
    return($message);
  }
  

  /* Attributes that are interesting for configuration generation */
  function get_attributes()
  {
    $tmp = setup_step::get_attributes();
    foreach(array("optional") as $attr){
      $tmp[$attr]= $this->$attr;
    }
    return($tmp);
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
