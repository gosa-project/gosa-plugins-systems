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


class setup_step_5 extends setup_step
{

  var $peopleou   = "ou=people";
  var $groupou    =  "ou=groups";
  var $peopledn   = "cn";
  var $uidbase    = 1000;
  var $encryption = "crypt";
  var $mail       = "none";
  var $theme      = "default"; 
  var $errorlvl   = TRUE;
  var $cyrusunixstyle = FALSE;

  var $pwd_rules  = array("pwminlen" => 6, 
                          "pwminlen_active" => FALSE,
                          "pwdiffer" => 5,
                          "pwdiffer_active" => FALSE,
                          "externalpwdhook" => "/usr/bin/sudo myscript",
                          "externalpwdhook_active" => FALSE);

  var $mail_settings = array("vacationdir"        => "/etc/gosa/vacation",
                             "vacationdir_active" => FALSE);
 
  var $crypt_methods  = array();
  var $mail_methods   = array();


  var $attributes = array("peopleou","groupou","peopledn","uidbase","encryption","mail","theme","errorlvl","cyrusunixstyle");

  function setup_step_5()
  {
    $this->update_strings();

    $tmp = @passwordMethod::get_available_methods_if_not_loaded();
    foreach($tmp['name'] as $name){
      $this->crypt_methods[$name] = $name;
    }
    $tmp = $this->get_available_mail_classes();
    foreach($tmp['name'] as $name){
      $this->mail_methods[$name] = $name;
    }
  }


  function update_strings()
  {
    $this->s_title      = _("GOsa settings 1/3");
    $this->s_title_long = _("GOsa generic settings, page 1/3");
    $this->s_info       = _("This dialog allows you to setup GOsa behaviour");
  }

  
  function execute()
  {
    $smarty = get_smarty();
    $smarty->assign("peopledns",array("uid","cn"));
    $smarty->assign("crypt_methods",$this->crypt_methods);
    $smarty->assign("mail_methods",$this->mail_methods);
    $smarty->assign("themes",$this->get_themes());
    $smarty->assign("pwd_rules",$this->pwd_rules);
    $smarty->assign("mail_settings",$this->mail_settings);
    $smarty->assign("bool",array(FALSE => _("No"), TRUE => _("Yes")));
    $smarty->assign("warnings" ,$this->check());
    $smarty->assign("warnings_cnt" ,count($this->check()));
    foreach($this->attributes as $attr){
      $smarty->assign($attr,$this->$attr);
    }
    return($smarty -> fetch (get_template_path("../setup/setup_step5.tpl")));
  }


  /* Returns the classnames auf the mail classes */
  function get_available_mail_classes()
  {
    $dir = opendir( "../include");
    $methods = array();
    $suffix = "class_mail-methods-";
    $lensuf = strlen($suffix);
    $prefix = ".inc";
    $lenpre = strlen($prefix);
    $i = 0;
    while (($file = readdir($dir)) !== false){

      if(stristr($file,$suffix)) {
        $lenfile = strlen($file);
        $methods['name'][$i] = substr($file,$lensuf,($lenfile-$lensuf)-$lenpre);
        $methods['file'][$i] = $file;
        $methods[$i]['file'] = $file;
        $methods[$i]['name'] = substr($file,$lensuf,($lenfile-$lensuf)-$lenpre);
        $i++;
      }
    }
    return($methods); 
  }

  function get_themes()
  {
    $dir = opendir( "../ihtml/themes/");
    $themes = array();
    while (($file = readdir($dir)) !== false){
      if(is_dir("../ihtml/themes/".$file) && !preg_match("/^\./",$file)){
        $themes[$file] = $file;
      }      
    }
    return($themes);
  }

  function check()
  {
    $message = array();

    if(preg_match("/,$/",$this->peopleou)){
      $message[] =sprintf(_("Don't add a trailing comma to '%s'."),_("People storage ou"));
    }

    if(preg_match("/,$/",$this->groupou)){
      $message[] =sprintf(_("Don't add a trailing comma to '%s'."),_("Group storage ou"));
    }

    if(!is_numeric($this->uidbase)){
      $message[] = _("Uid base must be numeric");
    }

    if(($this->pwd_rules['pwminlen_active']) && !is_numeric($this->pwd_rules['pwminlen'])){
      $message[] = _("The given password minimum length is not numeric.");
    }
    if(($this->pwd_rules['pwdiffer_active']) && !is_numeric($this->pwd_rules['pwdiffer'])){
      $message[] = _("The given password differ value is not numeric.");
    }
    return($message);

  }

  function save_object()
  {
    if(isset($_POST['step5_posted'])){

      /* Get attributes */
      foreach($this->attributes as $attr){
        if(isset($_POST[$attr])){
          $this->$attr = validate($_POST[$attr]);
        }
      }

      /* Get password settings */ 
      if(isset($_POST['pwdiffer_active'])){
        $this->pwd_rules['pwdiffer_active'] = TRUE;
        if(isset($_POST['pwdiffer'])){
          $this->pwd_rules['pwdiffer'] = $_POST['pwdiffer'];
        }
      }else{
        $this->pwd_rules['pwdiffer_active'] = FALSE;
      }

      /* Get password minimum length posts */
      if(isset($_POST['pwminlen_active'])){
        $this->pwd_rules['pwminlen_active'] = TRUE;
        if(isset($_POST['pwminlen'])){
          $this->pwd_rules['pwminlen'] = $_POST['pwminlen'];
        }
      }else{
        $this->pwd_rules['pwminlen_active'] = FALSE;
      }

      /* Mail settings */
      if(isset($_POST['vacationdir_active'])){
        $this->mail_settings['vacationdir_active'] = TRUE;
        if(isset($_POST['vacationdir'])){
          $this->mail_settings['vacationdir'] = $_POST['vacationdir'];
        }
      }else{
        $this->mail_settings['vacationdir_active'] = FALSE;
      }

      /* Mail settings */
      if(isset($_POST['externalpwdhook_active'])){
        $this->pwd_rules['externalpwdhook_active'] = TRUE;
        if(isset($_POST['externalpwdhook'])){
          $this->pwd_rules['externalpwdhook'] = $_POST['externalpwdhook'];
        }
      }else{
        $this->pwd_rules['externalpwdhook_active'] = FALSE;
      }
    }

    $tmp = $this->check(); 
    if(count($tmp) == 0){
      $this->is_completed = TRUE;
    }else{
      $this->is_completed = FALSE;
    }
  }// if tempalte posted 


  /* Attributes that are interesting for configuration generation */
  function get_attributes()
  {
    $tmp = setup_step::get_attributes();
    foreach(array("pwd_rules","mail_settings") as $attr){
      $tmp[$attr]= $this->$attr;
    }
    return($tmp);
  }
}// CLass

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
