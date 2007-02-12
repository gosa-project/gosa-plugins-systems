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
                                  "snapshot_active"=> FALSE,
                                  "snapshot_base" => "ou=snapshots,dc=server,dc=de",
                                  "snapshot_user" => "cn=ldapadmin,dc=server,dc=de",
                                  "snapshot_password" => "",
                                  "snapshot_server"   => "ldap://localhost:386");

  var $samba_settings    = array( "samba_sid"        => "0-815-4711",
                                  "samba_sid_active" => FALSE,
                                  "samba_rid"        => 1000,
                                  "samba_rid_active" => FALSE); 
  var $attributes = array("strict","governmentmode","sambaidmapping","minid","account_expiration");

  function setup_step_6()
  {
    $this->s_title      = _("GOsa settings 2/2");
    $this->s_title_long = _("GOsa generic settings, page 2/2");
    $this->s_info       = _("This dialog allows you to setup GOsa behaviour");
  }

  
  function execute()
  {
    $smarty = get_smarty();
    $smarty->assign("generic_settings",$this->generic_settings);
    $smarty->assign("samba_settings",$this->samba_settings);
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
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
