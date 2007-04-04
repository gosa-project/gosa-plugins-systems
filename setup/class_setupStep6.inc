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


class setup_step_6 extends setup_step
{
  var $strict         = TRUE;
  var $governmentmode = FALSE;
  var $sambaidmapping = FALSE;
  var $minid          = 100;       
  var $account_expiration  =FALSE;

  var $generic_settings  = array( "idgen"         => "{%sn}-{%givenName[2-4]}",
                                  "idgen_active"  => FALSE,
                                  "minid"         => "100",
                                  "minid_active"  => FALSE,
                                  "wws_ou"        => "ou=winstations",
                                  "wws_ou_active" => FALSE,
                                  "snapshot_active"     => FALSE,
                                  "snapshot_base"       => "ou=snapshots,%base%",
                                  "snapshot_ldap_base"  => "%base%",
                                  "snapshot_user"       => "cn=ldapadmin,%base%",
                                  "snapshot_password"   => "",
                                  "snapshot_server"     => "%connection%");

  var $samba_settings    = array( "samba_sid"        => "0-815-4711",
                                  "samba_sid_active" => FALSE,
                                  "samba_rid"        => 1000,
                                  "samba_rid_active" => FALSE); 
  var $attributes = array("strict","governmentmode","sambaidmapping","account_expiration");

  function setup_step_6()
  {
    $this->s_title      = _("GOsa settings 2/3");
    $this->s_title_long = _("GOsa generic settings, page 2/3");
    $this->s_info       = _("This dialog allows you to setup GOsa behaviour");
  }

  
  function execute()
  {
    /* Update snapshot values, with already collected values */
    foreach($this->generic_settings as $key => $value){
      foreach(array("snapshot_ldap_base","snapshot_base","snapshot_user","snapshot_server") as $attr){
        foreach($this->parent->captured_values as $replace_name => $replace_value){
          if(is_string($replace_value))  {
            $this->generic_settings[$attr] = preg_replace("/%".$replace_name."%/",$replace_value,$this->generic_settings[$attr]);
          }
        }
      }
    }

    $smarty = get_smarty();
    $smarty->assign("generic_settings",$this->generic_settings);
    $smarty->assign("samba_settings",$this->samba_settings);
    $smarty->assign("warnings" ,$this->check());
    $smarty->assign("warnings_cnt" ,count($this->check()));
    $smarty->assign("bool",array(FALSE => _("No"), TRUE => _("Yes")));
        foreach($this->attributes as $attr){
      $smarty->assign($attr,$this->$attr);
    }
    return($smarty -> fetch (get_template_path("../setup/setup_step6.tpl")));
  }

  function save_object()
  {
    if(isset($_POST['step6_posted'])){

      /* Get attributes */
      foreach($this->attributes as $attr){
        if(isset($_POST[$attr])){
          $this->$attr = validate($_POST[$attr]);
        }
      }

      if(isset($_POST['minid_active'])){
        $this->generic_settings['minid_active'] = TRUE;
        if(isset($_POST['minid'])){
          $this->generic_settings['minid'] = $_POST['minid'];
        }
      }else{
        $this->generic_settings['minid_active'] = FALSE;
      }

      if(isset($_POST['wws_ou_active'])){
        $this->generic_settings['wws_ou_active'] = TRUE;
        if(isset($_POST['wws_ou'])){
          $this->generic_settings['wws_ou'] = $_POST['wws_ou'];
        }
      }else{
        $this->generic_settings['wws_ou_active'] = FALSE;
      }

      /* Generic settings */
      if(isset($_POST['idgen_active'])){
        $this->generic_settings['idgen_active'] = TRUE;
        if(isset($_POST['idgen'])){
          $this->generic_settings['idgen'] = $_POST['idgen'];
        }
      }else{
        $this->generic_settings['idgen_active'] = FALSE;
      }
      if(isset($_POST['snapshot_active'])){
        $this->generic_settings['snapshot_active'] = TRUE;
        if(isset($_POST['snapshot_base'])){
          $this->generic_settings['snapshot_base'] = $_POST['snapshot_base'];
        }
        if(isset($_POST['snapshot_user'])){
          $this->generic_settings['snapshot_user'] = $_POST['snapshot_user'];
        }
        if(isset($_POST['snapshot_password'])){
          $this->generic_settings['snapshot_password'] = $_POST['snapshot_password'];
        }
        if(isset($_POST['snapshot_server'])){
          $this->generic_settings['snapshot_server'] = $_POST['snapshot_server'];
        }
        if(isset($_POST['snapshot_ldap_base'])){
          $this->generic_settings['snapshot_ldap_base'] = $_POST['snapshot_ldap_base'];
        }
      }else{
        $this->generic_settings['snapshot_active'] = FALSE;
      }

      /* Samba settings */
      if(isset($_POST['samba_sid_active'])){
        $this->samba_settings['samba_sid_active'] = TRUE;
        if(isset($_POST['samba_sid'])){
          $this->samba_settings['samba_sid'] = $_POST['samba_sid'];
        }
      }else{
        $this->samba_settings['samba_sid_active'] = FALSE;
      }
      if(isset($_POST['samba_rid_active'])){
        $this->samba_settings['samba_rid_active'] = TRUE;
        if(isset($_POST['samba_rid'])){
          $this->samba_settings['samba_rid'] = $_POST['samba_rid'];
        }
      }else{
        $this->samba_settings['samba_rid_active'] = FALSE;
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
    foreach(array("samba_settings","generic_settings") as $attr){
      $tmp[$attr]= $this->$attr;
    }
    return($tmp);
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
