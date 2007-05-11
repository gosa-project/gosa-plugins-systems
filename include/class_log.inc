<?php
/*
   This code is part of GOsa (https://gosa.gonicus.de)
   Copyright (C) 2003  Cajus Pollmeier

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


/*! \brief   The logging base class
  \author  Fabian Hickert <hickert@gonicus.de>
  \version 2.6
  \date    11.04.2007

  This is the base class for the GOsa logging functionality.
  All logging should lead to this class. 
 */
class log {

  var $LOG_TO_MYSQL = FALSE;
  var $LOG_TO_SYSLOG= FALSE;
  

 /*! \brief logging constructor

    \param action         One of these values (modify|create|remove|snapshot|copy)
    \param objecttype     represents the current edited objecttype, like users/user
    \param object         represents the current edited object dn
    \param changes_array  An array containing names of all touched attributes
    \param result         A status message, containing errors or success messages 

    \sa log()
   */
  function log($action,$objecttype,$object,$changes_array = array(),$result = TRUE)
  {
    global $config;
    $ui   = get_userinfo(); 

    /* Create string out of changes */
    $changes  ="";
    foreach($changes_array as $str ){
      $changes .= $str.",";
    }
    $changes = preg_replace("/,$/","",$changes );
    
    /* Create data object */
    $entry = array(); 
    $entry['timestamp'] = time();
    $entry['user']  =$ui->dn;
    $entry['action'] = $action;
    $entry['objecttype'] = $objecttype;
    $entry['object'] = $object;
    $entry['changes'] = $changes;
    $entry['result'] = $result;
 
    /* Start logging for configured methods */
    if(isset($this->config->current['LOGGING']) && preg_match("/(^|,)syslog(,|$)/i",$this->config->current['LOGGING'])){
      @log::log_into_syslog($entry);
    }
    if(isset($this->config->current['LOGGING']) && preg_match("/(^|,)mysql(,|$)/i",$this->config->current['LOGGING'])){
      @log::log_into_db($entry);
    }
  }

   
  /* This function is used to into the systems syslog */
  function log_into_syslog($entry)
  {
    $str = $entry['user']." ".$entry['action']." ".$entry['object']." from type ".$entry['objecttype']." ".$data." : Result was ".$entry['result'];
    gosa_log($str);
  }


  /* Log into configured logging databses.*/
  function log_into_db($entry)
  {
    if(isset($this->config->data['SERVERS']['LOGGING'])){
      $servers = $this->config->data['SERVERS']['LOGGING'];
    }else{
      print_red(_("You have enabled the logging into mysql databse, but there are no logging servers available."));
      return(FALSE);
    }

    foreach($servers as $server_name => $server){
  
      $con = mysql_pconnect($server['SERVER'],$server['USER'],$server['PWD']);
      if(!$con){
        print_red(sprintf(_("Could not connect to logging server %s."),$server['SERVER']));
      }else{
        $db = mysql_select_db($server['DB'],$con);
        if(!$db){
          print_red(sprintf(_("Could not select database %s on server %s. Server ssys :%s"),$server['DB'],$server['SERVER'],mysql_error($con)));
        }else{

          /* Create mysql syntax */
          $query ="INSERT INTO gosa_log 
                    (timestamp,user,action,objecttype,object,changes,result)
                   VALUES 
                    (
                        \"".addslashes($entry['timestamp'])."\", 
                        \"".addslashes($entry['user'])."\", 
                        \"".addslashes($entry['action'])."\", 
                        \"".addslashes($entry['objecttype'])."\", 
                        \"".addslashes($entry['object'])."\", 
                        \"".addslashes($entry['changes'])."\", 
                        \"".addslashes($entry['result'])."\" 
                    );
              ";
          $res = mysql_query($query,$con);
          if(!$res){
            print_red(sprintf(_("Could not query database %s on server %s. Server ssys :%s"),$server['DB'],$server['SERVER'],mysql_error($con)));
            
          } 
        }
      }
    }
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
