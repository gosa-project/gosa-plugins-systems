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


class setup_step_2 extends setup_step
{
  var $basic_checks = array();
  var $config_checks= array();

  function setup_step_2()
  {
    $this->s_title      = _("Installation check");
    $this->s_title_long = _("Basic installation checks");
    $this->s_info       = _("Some basic checks for PHP version, ldap extension...");
  }
 
  
  /* Execute and display template */ 
  function execute()
  {
    $this->run_checks();
    $smarty = get_smarty();
    $smarty->assign("basic" ,$this->basic_checks);
    $smarty->assign("config",$this->config_checks);
    return($smarty->fetch(get_template_path("setup_step2.tpl",TRUE,dirname(__FILE__))));
  }


  /* Execute all checks */ 
  function run_checks()
  {
    $i =0; 
    $this->basic_checks = array();
    $this->config_checks = array();
 
    /* PHP version check */
    $i ++;
    $this->basic_checks[$i]['NAME'] =  _("Checking PHP version");
    $this->basic_checks[$i]['DESC'] =  sprintf(_("PHP must be of version %s or %s or above for some functions."),"4.3.10","5.2.0");
    $this->basic_checks[$i]['RESULT']= $this->check_php_version();

    /* Checking imap extension */
    $i ++;
    $this->basic_checks[$i]['NAME'] =  _("Checking for ldap module");
    $this->basic_checks[$i]['DESC'] =  _("This is the main module used by GOsa and therefore really required.");
    $this->basic_checks[$i]['RESULT']= is_callable("ldap_bind");

    /* Checking for XML functions */
    $i ++;
    $this->basic_checks[$i]['NAME'] =  _("Checking for XML functions");
    $this->basic_checks[$i]['DESC'] =  _("XML functions are required to parse the configuration file.");
    $this->basic_checks[$i]['RESULT']= is_callable("xml_parser_create");

    /* Check for gettext support */
    $i ++;
    $this->basic_checks[$i]['NAME'] =  _("Checking for gettext support");
    $this->basic_checks[$i]['DESC'] =  _("Gettext support is required for internationalized GOsa.");
    $this->basic_checks[$i]['RESULT']= is_callable("bindtextdomain");

    /* iconv */
    $i ++;
    $this->basic_checks[$i]['NAME'] =  _("Checking for iconv support");
    $this->basic_checks[$i]['DESC'] =  _("This module is used by GOsa to convert samba munged dial informations and is therefore required.");
    $this->basic_checks[$i]['RESULT']= is_callable("iconv");

    /**/
    $i ++;
    $this->basic_checks[$i]['NAME'] =  _("Checking for mhash module");
    $this->basic_checks[$i]['DESC'] =  _("To use SSHA encryption, you'll need this module. If you are just using crypt or md5 encryption, ignore this message. GOsa   will run without it.");
    $this->basic_checks[$i]['RESULT']= is_callable("mhash");

    /**/
    $i ++;
    $this->basic_checks[$i]['NAME'] =  _("Checking for imap module");
    $this->basic_checks[$i]['DESC'] =  _("The IMAP module is needed to communicate with the IMAP server. It gets status informations, creates and deletes mail users. ");
    $this->basic_checks[$i]['RESULT']= is_callable("imap_open");

    /**/
    $i ++;
    $this->basic_checks[$i]['NAME'] =  _("Checking for getacl in imap");
    $this->basic_checks[$i]['DESC'] =  _("The getacl support is needed for shared folder permissions. The standard IMAP module is not capable of reading acl's. You   need a recend PHP version for this feature.");
    $this->basic_checks[$i]['RESULT']= is_callable("imap_getacl");

    /**/
    $i ++;
    $this->basic_checks[$i]['NAME'] =  _("Checking for mysql module");
    $this->basic_checks[$i]['DESC'] =  _("MySQL support is needed for reading GOfax reports from databases.");
    $this->basic_checks[$i]['RESULT']= is_callable("mysql_query");

    /**/
    $i ++;
    $this->basic_checks[$i]['NAME'] =  _("Checking for kadm5 module");
    $this->basic_checks[$i]['DESC'] =  _("Managing users in kerberos requires the kadm5 module which is downloadable via PEAR network.");
    $this->basic_checks[$i]['RESULT']= is_callable("kadm5_init_with_password");

    /**/
    $i ++;
    $this->basic_checks[$i]['NAME'] =  _("Checking for snmp Module");
    $this->basic_checks[$i]['DESC'] =  _("Simple Network Management Protocol (SNMP) is required for client monitoring.");
    $this->basic_checks[$i]['RESULT']= is_callable("snmpget");

    /* Checking for Cups module */
    $i ++;
    $this->basic_checks[$i]['NAME'] =  _("Checking for cups module");
    $this->basic_checks[$i]['DESC'] =  _("In order to read available printers from IPP protocol instead of printcap files, you've to install the CUPS module.");
    $this->basic_checks[$i]['RESULT']= is_callable("cups_get_dest_list");

    /* Checking for F ping utility */
    $i ++;
    $query= "LC_ALL=C LANG=C fping -v 2>&1";
    $output= shell_exec ($query);
    $this->basic_checks[$i]['NAME'] =  _("Checking for fping utility");
    $this->basic_checks[$i]['DESC'] =  _("The fping utility is only used if you've got a thin client based terminal environment running.");
    $this->basic_checks[$i]['RESULT']= preg_match("/^fping:/", $output);;

    /* Checking generate LM/NT password hashes */
    $i ++;
    $query= "mkntpwd 2>&1";
    $output= shell_exec ($query);
    $have_mkntpwd= preg_match("/^Usage: mkntpwd /", $output);
    if (!$have_mkntpwd){
      $query= 'LC_ALL=C LANG=C perl -MCrypt::SmbHash -e "print join(q[:], ntlmgen $ARGV[0]), $/;" &>/dev/null';
      system ($query, $ret);
      $have_mkntpwd |= ($ret == 0);
    }

    $this->basic_checks[$i]['NAME'] =  _("Password hashes");
    $this->basic_checks[$i]['DESC'] =  _("In order to use SAMBA 2/3, you've to install some additional packages to generate password hashes.");
    $this->basic_checks[$i]['RESULT']= $have_mkntpwd; 



    /* PHP Configuration checks 
     */

    /* Register_globals off */
    $i = 0;
    $this->config_checks[$i]['NAME'] =  _("register_globals"."&nbsp;"."<b>"._("Off")."</b>");
    $this->config_checks[$i]['DESC'] =  _("register_globals is a PHP mechanism to register all global varibales to be accessible from scripts without changing the scope. This may be a security risk. GOsa will run in both modes.");
    $this->config_checks[$i]['RESULT']= ini_get("register_globals") == 0;

    $i ++;
    $this->config_checks[$i]['NAME'] =  _("session.gc_maxlifetime"."&nbsp;"."<b> &gt;= 86400</b>");
    $this->config_checks[$i]['DESC'] =  _("PHP uses this value for the garbage collector to delete old sessions, setting this value to one day will prevent loosing session and cookie before they really timeout.");
    $this->config_checks[$i]['RESULT']= ini_get("session.gc_maxlifetime") >= 86400;

    $i ++;
    $session_auto_start = ini_get('session.auto_start');
    $this->config_checks[$i]['NAME'] =  _("session.auto_start"."&nbsp;"."<b>"._("Off")."</b>");
    $this->config_checks[$i]['DESC'] =  _("In Order to use GOsa without any trouble, the session.auto_register option in your php.ini must be set to 'Off'.");
    $this->config_checks[$i]['RESULT']= !$session_auto_start['local_value'];

    $i ++;
    $this->config_checks[$i]['NAME'] =  _("memory_limit")."&nbsp;"."<b> &gt;= 32</b>";
    $this->config_checks[$i]['DESC'] =  _("GOsa needs at least 32MB of memory, less will cause unpredictable errors! Increase it for larger setups.");
    $this->config_checks[$i]['RESULT']= ini_get('memory_limit') >= 32 ;


    $i ++;
    $this->config_checks[$i]['NAME'] =  _("implicit_flush")."&nbsp;"."<b>"._("Off")."</b>";
    $this->config_checks[$i]['DESC'] =  _("This option influences the Output handling. Turn this Option off, to increase performance.");
    $this->config_checks[$i]['RESULT']= !ini_get('implicit_flush');

    $i ++;
    $this->config_checks[$i]['NAME'] =  _("max_execution_time")."&nbsp;"."<b> &gt;= 30</b>";
    $this->config_checks[$i]['DESC'] =  _("The Execution time should be at least 30 seconds, because some actions may consume more time.");
    $this->config_checks[$i]['RESULT']= ini_get("max_execution_time") >= 30 ;

    $i ++;
    $this->config_checks[$i]['NAME'] =  _("expose_php")."&nbsp;<b>"._("Off")."</b>";
    $this->config_checks[$i]['DESC'] =  _("Increase the server security by setting expose_php to 'off'. PHP won't send any Information about the server you are running in this case.");
    $this->config_checks[$i]['RESULT']=  !ini_get("expose_php");


    $i ++;
    $this->config_checks[$i]['NAME'] =  _("magic_quotes_gpc")."&nbsp;<b>"._("On")."</b>";
    $this->config_checks[$i]['DESC'] =  _("Increase your server security by setting magic_quotes_gpc to 'on'. PHP will escape all quotes in strings in this case.");
    $this->config_checks[$i]['RESULT']=  ini_get('magic_quotes_gpc');
  }
  

  /* Check if current PHP version is compatible 
      with the current version of GOsa */
  function check_php_version()
  {
    if(preg_match("/^5/",phpversion())){
      return(version_compare(phpversion(),"5.2.0",">=")); 
    }else{
      return(version_compare(phpversion(),"4.3.10",">="));
    }
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
