<?php
/*
   This code is part of GOsa (https://gosa.gonicus.de)
   Copyright (C) 2003-2007 - Fabian Hickert <hickert@gonicus.de>

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


/* The sieve management class displays a list of sieve 
 *  scripts for the given mail account. 
 * The account is identified by the parents uid attribute. 
 *
 *  $config       The config object
 *  $dn           The object edited 
 *  $parent       The parent object that provides the uid attribute 
 */
class sieveManagement extends plugin
{
  var $parent = NULL;
  var $scripts= array();  

 
  /* Initialize the class and load all sieve scripts 
   *  try to parse them and display errors 
   */ 
  function sieveManagement($config,$dn,$parent)
  {
    $this->parent = $parent;
    plugin::plugin($config,$dn);


    /* Connect to sieve class and try to get all available sieve scripts */
    $cfg=  $this->config->data['SERVERS']['IMAP'][$this->parent->gosaMailServer];
    
    /* Log into the mail server */
    $sieve= new sieve($cfg["sieve_server"], $cfg["sieve_port"], $this->parent->uid,
        $cfg["password"], $cfg["admin"]);

    /* Try to login */
    if (!$sieve->sieve_login()){
      print_red(sprintf(_("Can't log into SIEVE server. Server says '%s'."),
            to_string($sieve->error_raw)));
      return;
    }

    /* Get all sieve scripts names */
    if($sieve->sieve_listscripts()){
      if (is_array($sieve->response)){
        foreach($sieve->response as $key => $name){
          $this->scripts[$key]['NAME'] = $name;
        }
      } 
    }

    /* Get script contents */
    foreach($this->scripts as $key => $script){
      $p = new My_Parser;
      $sieve->sieve_getscript($script['NAME']);

      $script = "";
      foreach($sieve->response as $line){
        $script.=$line;
      }

      $this->scripts[$key]['SCRIPT'] = $script;
      $this->scripts[$key]['MSG']   = "";
      $ret = $p->parse($script);
      if(!$ret){
        $this->scripts[$key]['MSG']   = "<font color='red'>".$p->status_text."</font>";
        $this->scripts[$key]['STATUS'] = FALSE;
      }else{
        $this->scripts[$key]['STATUS'] = TRUE;
      }
      $this->scripts[$key]['PARSER'] = $p;
    }
  }


  /* Handle sieve list 
   */
  function execute()
  {
    $once = TRUE;
    foreach($_POST as $name => $value){
      if(preg_match("/^editscript_/",$name) && $once){
        $script = preg_replace("/^editscript_/","",$name);
        $script = preg_replace("/_(x|y)/","",$script);
        $once = FALSE;

        $obj_handler = new sieveElement;
        $data = $this->scripts[$script]['PARSER'];
        $obj_handler->resolve_to_object($data);
        return($obj_handler->execute());
      }
    }


    /* Create list of available sieve scripts 
     */
    $List = new divSelectBox("sieveManagement");
    foreach($this->scripts as $key => $script){
      $field1 = array("string" => $script['NAME']);  
      if($script['STATUS']){
        $field2 = array("string" => _("Parse successful"));
      }else{
        $field2 = array("string" => _("Parse failed") .$script['MSG']);
      }
      $field3 = array("string" => _("Script length")."&nbsp;:&nbsp;".strlen($script['SCRIPT']));
      $field4 = array("string" => "<input type='image' name='editscript_".$key."' src='images/edit.png'>");
      $List ->AddEntry(array($field1,$field2,$field3,$field4)); 
    }

    return($List->DrawList());
  }

  function save_object()
  {
    
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
