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


class setup_step_8 extends setup_step
{
  var $create_backup = TRUE;
  var $gosa_conf_name = "/gosa.conf";

  function setup_step_8()
  {
    $this->s_title      = _("Configuration file");
    $this->s_title_long = _("In this step the configuration file will be created.");
    $this->s_info       = _("Saving configuration file");
  }


  function execute()
  {

    /* Check if there is currently an active gosa.conf 
     */
    $exists = file_exists(CONFIG_DIR.$this->gosa_conf_name); 

    /* Check if existing config file is writeable */
    if($exists){
      $writeable = is_writeable(CONFIG_DIR.$this->gosa_conf_name);
    }else{
      $writeable = is_writeable(CONFIG_DIR);
    }

    $smarty = get_smarty();
  
    $info= posix_getgrgid(posix_getgid());
    $smarty->assign("webgroup", $info['name']);

    $smarty->assign("gosa_conf_name" , $this->gosa_conf_name);
    $smarty->assign("create_backup" , $this->create_backup);
    $smarty->assign("CONFIG_DIR",CONFIG_DIR);
  
    $smarty->assign("msg_permissions",sprintf(_("The following file(s), folders(s) must be writeable for the web-user '%s'."),$info['name']));
 
    $smarty->assign("exists",$exists);
    $smarty->assign("writeable",$writeable);
 
    $smarty->assign("cv",$this->parent->captured_values);
    return($smarty -> fetch (get_template_path("../setup/setup_step8.tpl")));
  }


  /* Create a backup of the currently existing configuration file. 
   */
  function create_backup()
  {
    if(is_writeable(CONFIG_DIR) && is_writeable(CONFIG_DIR.$this->gosa_conf_name)){
      $src = CONFIG_DIR.$this->gosa_conf_name;
      $dst = CONFIG_DIR.$this->gosa_conf_name."_".date("Ymd");
      $dst_backup= $dst;
      $i = 1;
      while(file_exists($dst)){
        $dst = $dst_backup."-".$i;
        $i ++;
      }
      return(rename($src,$dst));
    }else{
      return(FALSE);
    }
  }


  function save_object()
  {
    if(isset($_POST['step8_posted'])){

      /* Get attributes */
      foreach($this->attributes as $attr){
        if(isset($_POST[$attr])){
          $this->$attr = validate($_POST[$attr]);
        }
      }

      /* Backup toggle */
      if(isset($_POST['create_backup'])){
        $this->create_backup = TRUE;
      }else{
        $this->create_backup = FALSE;
      }
    }
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
