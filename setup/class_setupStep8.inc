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
  var $create_backup    = TRUE;
  var $gosa_conf_name   = "/gosa.conf";
  var $cfg_file_written = FALSE;
  var $last_backup_name = "";

  function setup_step_8()
  {
    $this->s_title      = _("Configuration file");
    $this->s_info       = _("In this step the configuration file will be created.");
    $this->s_title_long = _("Saving configuration file");
  }


  function get_conf_data()
  {
    return("currently not implemented.");
  }


  function execute()
  {

    $info= posix_getgrgid(posix_getgid());
    $webgroup = $info['name'];
      
    
    /* Check if there is currently an active gosa.conf 
     */
    $exists = file_exists(CONFIG_DIR.$this->gosa_conf_name); 

    /* Check if existing config file is writeable */
    if($exists){
      $writeable = is_writeable(CONFIG_DIR.$this->gosa_conf_name);
    }else{
      $writeable = is_writeable(CONFIG_DIR);
    }

    /* Downlaod config */
    if(isset($_POST['getconf'])){
      header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
      header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
      header("Cache-Control: no-cache");
      header("Pragma: no-cache");
      header("Cache-Control: post-check=0, pre-check=0");
      header("Content-type: text/plain");
      header('Content-Disposition: attachment; filename="gosa.conf"');
      echo $this->get_conf_data();
      exit();
    }
   
    
    /* Try to save configuration */
    $abort    = FALSE;
    $err_msg  = "";
    if(isset($_POST['saveconf'])){
      if($exists && $this->create_backup){
        if(!$this->create_backup()){
          $abort = TRUE;
          $err_msg = _("Could not create requested configuration file backup. Aborted writing config file. Please check folder permission and try again. Or use the manual method if this can not be fixed anyway.");
        }
      }
    
      if(!$abort){
        
        /* Try to create file handle */
        $fp = @fopen(CONFIG_DIR.$this->gosa_conf_name, "w");
    
        if(!$fp){
          $err_msg = sprintf(_("Can not create handle on file '%s', the configuration could not be written. Please check folder permission and try again. Or use the manual method if this can not be fixed anyway."),CONFIG_DIR.$this->gosa_conf_name);
          $abort  =TRUE;
        }else{

          $data = $this->get_conf_data();
          if(!fwrite($fp,strlen($data))){
            $err_msg = sprintf(_("Can not write file '%s'. Please check folder permission and try again. Or use the manual method if this can not be fixed anyway."),CONFIG_DIR.$this->gosa_conf_name);
            $abort  =TRUE;
          }else{
            
            @chgrp(CONFIG_DIR.$this->gosa_conf_name,$webgroup);
            @chown(CONFIG_DIR.$this->gosa_conf_name,"root");
            @chmod(CONFIG_DIR.$this->gosa_conf_name,0640);    
          }
        }
      }
    } 


    if($exists && $this->is_world_readable(CONFIG_DIR.$this->gosa_conf_name)){
      $err_msg = _("Your configuration file is currently world readable. This is a big security risk. Please updated the file permissions as shown in the manual configuration part below.");
    }


    $smarty = get_smarty();
    $smarty->assign("save_requested", isset($_POST['saveconf']));
    $smarty->assign("err_msg",$err_msg);
    $smarty->assign("webgroup", $webgroup);
    $smarty->assign("gosa_conf_name" , $this->gosa_conf_name);
    $smarty->assign("create_backup" , $this->create_backup);
    $smarty->assign("CONFIG_DIR",CONFIG_DIR);
    $smarty->assign("exists",$exists);
    $smarty->assign("last_backup_name",$this->last_backup_name);
    $smarty->assign("writeable",$writeable);
    $smarty->assign("cv",$this->parent->captured_values);
    $smarty->assign("msg_permissions",
      sprintf(_("The following file(s), folders(s) must be writeable for the web-user '%s'."),$info['name']));
    return($smarty -> fetch (get_template_path("../setup/setup_step8.tpl")));
  }


  /* check if given file is world readable */
  function is_world_readable($file)
  {
    clearstatcache();
    $p = fileperms($file);
    $w_r = (decbin($p & 4) == TRUE);
    return($w_r);
  }


  /* Create a backup of the currently existing configuration file. 
   */
  function create_backup()
  {
    $info= posix_getgrgid(posix_getgid());
    $webgroup = $info['name'];
    if(is_writeable(CONFIG_DIR) && is_writeable(CONFIG_DIR.$this->gosa_conf_name)){
      $src = CONFIG_DIR.$this->gosa_conf_name;
      $dst = CONFIG_DIR.$this->gosa_conf_name."_".date("Ymd");
      $dst_backup= $dst;
      $i = 1;
      while(file_exists($dst)){
        $dst = $dst_backup."-".$i;
        $i ++;
      }
      if(copy($src,$dst)){
        $this->last_backup_name = $dst;
        @chgrp($dst,$webgroup);
        @chown($dst,"root");
        @chmod($dst,0640);    
        return(TRUE);
      }else{
        return(FALSE);
      }
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
      if(isset($_POST['create_backup_visible'])){
        if(isset($_POST['create_backup'])){
          $this->create_backup = TRUE;
        }else{
          $this->create_backup = FALSE;
        }
      }
    }
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
