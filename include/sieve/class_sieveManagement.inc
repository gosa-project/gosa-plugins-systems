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

  var $current_script  = -1;
  var $current_handler = NULL;

  var $sieve_handle = NULL; 


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
        $this->scripts[$key]['STATUS'] = _("Parse failed");
      }else{
        $this->scripts[$key]['STATUS'] = _("Parse successful");
      }
      $this->scripts[$key]['PARSER'] = $p;
      $this->scripts[$key]['EDITED'] = FALSE;
    }
    $this->sieve_handle = $sieve;
  }


  /* Handle sieve list 
   */
  function execute()
  {
    $once = TRUE;
    foreach($_POST as $name => $value){
      if(preg_match("/^editscript_/",$name) && $once && !$this->current_handler){
        $script = preg_replace("/^editscript_/","",$name);
        $script = preg_replace("/_(x|y)/","",$script);
        $once = FALSE;

        $this->current_script = $script;
        $this->current_handler = $this->scripts[$script]['PARSER'];
      }
    }

    /* Abort saving */
    if(isset($_POST['cancel_sieve_changes'])){
      $this->current_handler = NULL;
    }

    /* Save currently edited sieve script. */
    if(isset($_POST['save_sieve_changes'])){
      $this->scripts[$this->current_script]['PARSER'] = $this->current_handler;
      $this->scripts[$this->current_script]['EDITED'] = TRUE;
      $this->current_handler = NULL; 
    }

    /* Create output for currently opened sieve script */
    if($this->current_handler){
      $ret = $this->current_handler->execute();
      $ret .= "<div class='seperator' style='text-align:right; width:100%;'>
        <input type='submit' name='save_sieve_changes' value='"._("Save")."'>
        &nbsp;
      <input type='submit' name='cancel_sieve_changes' value='"._("Cancel")."'>
        </div>";
      return($ret);
    }

    /* Create list of available sieve scripts 
     */
    $List = new divSelectBox("sieveManagement");
    foreach($this->scripts as $key => $script){
  
      $edited =  $script['EDITED'];

      $field1 = array("string" => $script['NAME']);  
      $field2 = array("string" => $script['STATUS']);
      $field3 = array("string" => _("Script length")."&nbsp;:&nbsp;".strlen($script['SCRIPT']));

      if($edited){
        $field4 = array("string" => "<img src='images/fai_new_hook.png' alt='"._("Edited")."'>",
                        "attach" => "style='width:30px;'");
      }else{
        $field4 = array("string" => "",
                        "attach" => "style='width:30px;'");
      }

      $field5 = array("string" => "<input type='image' name='editscript_".$key."' src='images/edit.png'>");
      $List ->AddEntry(array($field1,$field2,$field3,$field4,$field5)); 
    }
  
    $display ="<h2>Sieve script management</h2>";
    $display .= _("Be careful. All your changes will be saved directly to sieve, if you use the save button below.");
    $display .=  $List->DrawList();
    
    $display .= "<p style=\"text-align:right\">\n";
    $display .= "<input type=submit name=\"sieve_finish\" style=\"width:80px\" value=\""._("Ok")."\">\n";
    $display .= "&nbsp;\n";
    $display .= "<input type=submit name=\"sieve_cancel\" value=\""._("Cancel")."\">\n";
    $display .= "</p>";
    return($display);;
  }

  function save_object()
  {
    if($this->current_handler){
      $this->current_handler->save_object();
    }
  }


  function save()
  {
    /* Connect to sieve class and try to get all available sieve scripts */
    $cfg=  $this->config->data['SERVERS']['IMAP'][$this->parent->gosaMailServer];

    $this->sieve_handle= 
        new sieve(  $cfg["sieve_server"], 
                    $cfg["sieve_port"], 
                    $this->parent->mail,
                    $cfg["password"], 
                    $cfg["admin"]);

    if (!$this->sieve_handle->sieve_login()){
      print_red(sprintf(_("Can't log into SIEVE server. Server says '%s'."),to_string($this->sieve_handle->error_raw)));
      return;
    }

    $everything_went_fine = TRUE;

    foreach($this->scripts as $key => $script){
      if($script['EDITED']){
        $data = $script['PARSER']->get_sieve_script();
        if(!$this->sieve_handle->sieve_sendscript($script['NAME']."1", $data)){
          gosa_log("Failed to save sieve script named '".$script['NAME']."': ".to_string($this->sieve_handle->error_raw));
          $everything_went_fine = FALSE;
          print_red(to_string($this->sieve_handle->error_raw));
          $this->scripts[$key]['STATUS'] = "<font color='red'>".
                                           _("Failed to save sieve script").": ".
                                           to_string($this->sieve_handle->error_raw).
                                           "</font>";
        }
      }
    }
    return($everything_went_fine);
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
