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


class setup_step_7 extends setup_step
{
  var $checks = array();
  var $schema_readable  = FALSE;
  var $attributes       = array();

  function setup_step_7()
  {
    $this->s_title      = _("Ldap schema check");
    $this->s_title_long = _("Ldap schema check");
    $this->s_info       = _("This dialog checks your ldap schema setup");
  }

  
  function execute()
  {
    $smarty = get_smarty();
    $smarty->assign("schema_readable",$this->schema_readable);
    $smarty->assign("checks",$this->checks);
    return($smarty -> fetch (get_template_path("../setup/setup_step7.tpl")));
  }

  function save_object()
  {
    if(isset($_POST['step7_posted'])){

      /* Get attributes */
      foreach($this->attributes as $attr){
        if(isset($_POST[$attr])){
          $this->$attr = validate($_POST[$attr]);
        }
      }
    }
  }

  
  function check_schema()
  {
    $messages= array();

    $checks["gosaObject"]["VERSION"]  = "2.4";
    $checks["gosaObject"]["FILE"]     = "gosa+samba3.schema/gosa.schema";
    $checks["gosaObject"]["CLASS"]    = array("gosaObject");
    $checks["gosaObject"]["STATUS"]   = FALSE;
    $checks["gosaObject"]["MUST"]     = FALSE;
    $checks["gosaObject"]["MSG"]      = ""; // The error message to display
    $checks["gosaObject"]["INFO"]     = _("GOsa object is used to store acls.");

    $checks["gosaAccount"]["VERSION"]  = "2.4";
    $checks["gosaAccount"]["FILE"]     = "gosa+samba3.schema/gosa.schema";
    $checks["gosaAccount"]["CLASS"]    = array("gosaAccount");
    $checks["gosaAccount"]["STATUS"]   = FALSE;
    $checks["gosaAccount"]["MUST"]     = TRUE;
    $checks["gosaAccount"]["MSG"]      = ""; // The error message to display
    $checks["gosaAccount"]["INFO"]     = _("Used to store account specific informations.");

    $checks["gosaLockEntry"]["VERSION"]  = "2.4";
    $checks["gosaLockEntry"]["FILE"]     = "gosa+samba3.schema/gosa.schema";
    $checks["gosaLockEntry"]["CLASS"]    = array("gosaLockEntry");
    $checks["gosaLockEntry"]["STATUS"]   = FALSE;
    $checks["gosaLockEntry"]["MUST"]     = TRUE;
    $checks["gosaLockEntry"]["MSG"]      = ""; // The error message to display
    $checks["gosaLockEntry"]["INFO"]     = _("Used to lock currently edited entries to avoid multiple changes at the same time.");

    $checks["gosaLockEntry"]["VERSION"]  = "2.4";
    $checks["gosaLockEntry"]["FILE"]     = "gosa+samba3.schema/gosa.schema";
    $checks["gosaLockEntry"]["CLASS"]    = array("gosaLockEntry");
    $checks["gosaLockEntry"]["STATUS"]   = FALSE;
    $checks["gosaLockEntry"]["MUST"]     = TRUE;
    $checks["gosaLockEntry"]["MSG"]      = ""; // The error message to display
    $checks["gosaLockEntry"]["INFO"]     = _("Used to lock currently edited entries to avoid multiple changes at the same time.");

    /* asdfasdf */


    $checks= array(
        "gosaObject"            => array("version" => "2.4"),
        "gosaAccount"           => array("version" => "2.4"),
        "gosaLockEntry"         => array("version" => "2.4"),
        "gosaCacheEntry"        => array("version" => "2.4"),
        "gosaDepartment"        => array("version" => "2.4"),

        "goFaxAccount"          => array("version" => "1.0.4", "class" => "gofaxAccount","file" => "gofax.schema"),
        "goFaxSBlock"           => array("version" => "1.0.4", "class" => "gofaxAccount","file" => "gofax.schema"),
        "goFaxRBlock"           => array("version" => "1.0.4", "class" => "gofaxAccount","file" => "gofax.schema"),

        "gosaUserTemplate"      => array("version" => "2.4", "class" => "posixAccount","file" => "nis.schema"),
        "gosaMailAccount"       => array("version" => "2.4", "class" => "mailAccount","file" => "gosa+samba3.schema"),
        "gosaProxyAccount"      => array("version" => "2.4", "class" => "proxyAccount","file" => "gosa+samba3.schema"),
        "gosaApplication"       => array("version" => "2.4", "class" => "appgroup","file" => "gosa.schema"),
        "gosaApplicationGroup"  => array("version" => "2.4", "class" => "appgroup","file" => "gosa.schema"),

        "GOhard"                => array("version" => "2.5", "class" => "terminals","file" => "goto.schema"),
        "gotoTerminal"          => array("version" => "2.5", "class" => "terminals","file" => "goto.schema"),
        "goServer"              => array("version" => "2.4","class" => "server","file" => "goserver.schema"),
        "goTerminalServer"      => array("version" => "2.4", "class" => "terminals","file" => "goto.schema"),
        "goShareServer"           => array("version" => "2.4", "class" => "terminals","file" => "goto.schema"),
        "goNtpServer"           => array("version" => "2.4", "class" => "terminals","file" => "goto.schema"),
        "goSyslogServer"        => array("version" => "2.4", "class" => "terminals","file" => "goto.schema"),
        "goLdapServer"          => array("version" => "2.4"),
        "goCupsServer"          => array("version" => "2.4", "class" => array("posixAccount", "terminals"),),
        "goImapServer"          => array("version" => "2.4", "class" => array("mailAccount", "mailgroup"),"file" => "gosa+samba3.schema"),
        "goKrbServer"           => array("version" => "2.4"),
        "goFaxServer"           => array("version" => "2.4", "class" => "gofaxAccount","file" => "gofax.schema"),
        );

  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
