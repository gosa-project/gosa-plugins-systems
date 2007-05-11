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


/*! \brief   The plugin base class
  \author  Cajus Pollmeier <pollmeier@gonicus.de>
  \version 2.00
  \date    24.07.2003

  This is the base class for all plugins. It can be used standalone or
  can be included by the tabs class. All management should be done
  within this class. Extend your plugins from this class.
 */
class log {

  var $LOG_TO_MYSQL = FALSE;
  var $LOG_TO_SYSLOG= FALSE;
  

 /*! \brief plugin constructor

    If 'dn' is set, the node loads the given 'dn' from LDAP

    \param dn Distinguished name to initialize plugin from
    \sa plugin()
   */
  function log($action,$objecttype,$object,$changes_array = array(),$result = TRUE)
  {
    global $config;

    $ui   = get_userinfo(); 

    $changes  ="";
    foreach($changes_array as $str ){
      $changes .= $str.",";
    }
    $changes = preg_replace("/,$/","",$str);
    
    
    $entry = array(); 
    $entry['timestamp'] = time();
    $entry['user']  =$ui->dn;
    $entry['action'] = $action;
    $entry['objecttype'] = $objecttype;
    $entry['object'] = $object;
    $entry['changes'] = $changes;
    $entry['result'] = $result;
 
  


    if(isset($this->config->current['LOGGING']) && preg_match("/syslog/i",$this->config->current['LOGGING'])){
      @log::log_into_syslog($entry);
    }
    if(isset($this->config->current['LOGGING']) && preg_match("/mysql/i",$this->config->current['LOGGING'])){
      @log::log_into_db($entry);
    }
  }

  

  function log_into_syslog($entry)
  {
    $str = $entry['user']." ".$entry['action']." ".$entry['object']." from type ".$entry['objecttype']." ".$data." : Result was ".$entry['result'];
    gosa_log($str);
  }


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
