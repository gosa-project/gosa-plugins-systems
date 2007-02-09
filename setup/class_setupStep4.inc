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


class setup_step_4 extends setup_step
{
  var $connection = "ldap://localhost:389";
  var $location   = "default";
  var $admin      = "";
  var $password   = "";
  var $base       = "";
  var $peopleou   = "ou=people,";
  var $peopledn   = "cn";
  var $groupou    = "ou=groups,";
  var $uidbase    = 1000;
  var $encryption = "crypt";
  var $mail       = "kolab";
  var $errors     = TRUE;

  var $crypt_methods  = array();
  var $mail_methods   = array();

  var $connect_id = FALSE;
  var $bind_id    = FALSE;

  var $resolve_filter = "*";
  var $resolve_user   = FALSE;

  var $attributes = array("connection","location","admin","password","base","peopleou","peopledn","groupou",
                          "uidbase","encryption","mail","errors");

  function setup_step_4()
  {
    $this->s_title      = _("Ldap settings");
    $this->s_title_long = _("Ldap connection setup");
    $this->s_info       = _("This dialog allows the basic configuration of GOsa's behaviour and properties in your main configuration.");
    $tmp = @passwordMethod::get_available_methods_if_not_loaded();
    $this->crypt_methods   = $tmp['name'];
    $tmp = $this->get_available_mail_classes();
    $this->mail_methods = $tmp['name'];
  }
  
  function execute()
  {
    $smarty = get_smarty();
    foreach($this->attributes as $attr){
      $smarty->assign($attr,$this->$attr);
    }

    $smarty->assign("peopledns",array("uid","cn"));
    $smarty->assign("crypt_methods",$this->crypt_methods);
    $smarty->assign("mail_methods",$this->mail_methods);

     
    $attr = @LDAP::get_naming_contexts($this->connection);
    unset($attr['count']);
    $smarty->assign("namingContexts",$attr);
    $smarty->assign("namingContextsCount",count($attr));

    /* Assign connection status */
    $smarty->assign("connection_status",$this->get_connection_status());

    /* Addign resolved users */
    $smarty->assign("resolve_user",$this->resolve_user);
    if($this->resolve_user){
      $tmp = $this->resolve_user();
      $smarty->assign("resolved_users",$tmp);
      $smarty->assign("resolved_users_count",count($tmp));
      $smarty->assign("resolve_filter",$this->resolve_filter);
    }
    return($smarty -> fetch (get_template_path("../setup/setup_step4.tpl")));
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

  function get_connection_status()
  {
    $this->connect_id = FALSE;
    $this->bind_id    = FALSE;

    @ldap_set_option(NULL, LDAP_OPT_DEBUG_LEVEL, 7);
    $this->connect_id = @ldap_connect($this->connection);
      
    @ldap_set_option($this->connect_id, LDAP_OPT_PROTOCOL_VERSION, 3);
    $this->bind_id = @ldap_bind($this->connect_id, $this->admin, $this->password);
    
    if(!$this->bind_id){
      $str = sprintf(_("Anonymous bind failed on server '%s'."),$this->connection); 
      if(!empty($this->admin)){
        $str = sprintf(_("Bind as user '%s' failed on server '%s'."),$this->admin,$this->connection);
      }      
      return("<font color='red'>".$str."</font>");
    }else{
      if(empty($this->admin)){
        $str = sprintf(_("Anonymous bind successful on server '%s'. Please specify user and password."),$this->connection); 
        return("<font color='blue'>".$str."</font>");
      }else{
        $str = sprintf(_("Bind as user '%s' successful on server '%s'."),$this->admin,$this->connection);
        return("<font color='green'>".$str."</font>");
      }      
    }
  }

  
  function resolve_user()
  {
    $filter  = $this->resolve_filter;
    $ldap = new LDAP("","",$this->connection);
    $ldap->cd($this->base);
    $ldap->search("(&(objectClass=person)(|(uid=".$filter.")(cn=".$filter.")))");
    $tmp = array();
    while($attrs = $ldap->fetch()){
      $tmp[$attrs['dn']]=$attrs['dn'];
      natcasesort($tmp);
    }
    return($tmp);
  }   


  function save_object()
  {
    foreach($this->attributes as $attr){
      if(isset($_POST[$attr])){
        $this->$attr = $_POST[$attr];
      }
    }

    if(isset($_POST['resolve_user'])){
      $this->resolve_user = !$this->resolve_user;
    }
    
    if(isset($_POST['resolve_filter'])){
      $this->resolve_filter = $_POST['resolve_filter'];
    }

    if(isset($_POST['use_selected_user'])){

      if(isset($_POST['admin_to_use'])){
        $this->admin = $_POST['admin_to_use'];
        $this->resolve_user = false;
      }
    }
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
