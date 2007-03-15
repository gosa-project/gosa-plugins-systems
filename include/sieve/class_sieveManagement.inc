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
  var $script_to_delete =-1;
  var $sieve_handle = NULL; 

  var $create_script = FALSE;

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

          $data = array();
          $data['NAME'] = $name;

          if($key == "ACTIVE" && $key === "ACTIVE"){
            $data['ACTIVE'] = TRUE;
          }else{
            $data['ACTIVE'] = FALSE;
          }
          $this->scripts[] = $data;          
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
      $this->scripts[$key]['ORIG_SCRIPT'] = $script;
      $this->scripts[$key]['MSG']   = "";
      $ret = $p->parse($script);
      if(!$ret){
        $this->scripts[$key]['STATUS']   = FALSE;
        $this->scripts[$key]['MODE']    = "Source-Only";
        $this->scripts[$key]['MSG'] = _("Parse failed")."<font color='red'>".$p->status_text."</font>";
      }else{
        $this->scripts[$key]['STATUS']   = TRUE;
        $this->scripts[$key]['MODE']    = "Structured";
        $this->scripts[$key]['MSG'] = _("Parse successful");
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
    if(isset($_POST['create_new_script'])){
      $this->create_script = TRUE;
    }

    if(isset($_POST['create_script_cancel'])){
      $this->create_script = FALSE;
    }

    if($this->create_script){
      $smarty = get_smarty();
    
      $name = "";
      if(isset($_POST['NewScriptName'])){
        $name = $_POST['NewScriptName'];
      }
  
      $err = "";
      if($name != strtolower($name)){
        $err = _("Only lower case names are allowed here.");
      }
  
      if(preg_match("/[^a-z]/i",$name)){
        $err = _("Only a-z are allowed in script names.");
      }

      if($this->create_script && isset($_POST['create_script_save']) && $err == "" ){
        $this->create_script = FALSE;

        $script = "/*New script */".
                  "stop;";
        $p = new My_Parser;

        $sc['SCRIPT'] = $script;
        $sc['ORIG_SCRIPT'] = $script;
        $sc['MSG']   = "";
  
        $ret = $p->parse($script);
        if(!$ret){
          $sc['STATUS']   = FALSE;
          $sc['MODE']    = "Source-Only";
          $sc['MSG'] = _("Parse failed")."<font color='red'>".$p->status_text."</font>";
        }else{
          $sc['STATUS']   = TRUE;
          $sc['MODE']    = "Strucktured";
          $sc['MSG'] = _("Parse successful");
        }
        $sc['PARSER'] = $p;
        $sc['EDITED'] = TRUE;
        $sc['ACTIVE'] = FALSE;
        $sc['NAME']   = $name;
        $this->scripts[$name] = $sc;
      }else{
        $smarty->assign("NewScriptName",$name);
        $smarty->assign("Error",$err);
        return($smarty->fetch(get_template_path("templates/create_script.tpl",TRUE,dirname(__FILE__))));
      }
    }

    $once = TRUE;
    foreach($_POST as $name => $value){
      if(preg_match("/^editscript_/",$name) && $once && !$this->current_handler){
        $script = preg_replace("/^editscript_/","",$name);
        $script = preg_replace("/_(x|y)/","",$script);
        $once = FALSE;

        $this->current_script = $script;
        $this->current_handler = $this->scripts[$script]['PARSER'];
      }
      if(preg_match("/^delscript_/",$name) && $once && !$this->current_handler){
        $script = preg_replace("/^delscript_/","",$name);
        $script = preg_replace("/_(x|y)/","",$script);
        $once = FALSE;
 
        $this->script_to_delete = $script;  
      }
      if(preg_match("/^active_script_/",$name) && $once && !$this->current_handler){
        $script = preg_replace("/^active_script_/","",$name);
        $script = preg_replace("/_(x|y)/","",$script);
        $once = FALSE;
 
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

        if(!$this->sieve_handle->sieve_setactivescript($this->scripts[$script]['NAME'])){
          print_red(sprintf(_("Can't activate sieve script on server. Server says '%s'."),to_string($this->sieve_handle->error_raw)));
        }else{

          foreach($this->scripts as $key => $data){

            if($key == $script){
              $this->scripts[$key]['ACTIVE'] = TRUE;
            }else{
              $this->scripts[$key]['ACTIVE'] = FALSE;
            }
          }
        }
      }
    }

    if(isset($_POST['delete_cancel'])){
      $this->script_to_delete = -1;
    }

    if(isset($_POST['delete_script_confirm'])){

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

      if(!$this->sieve_handle->sieve_deletescript($this->scripts[$this->script_to_delete]['NAME'])){
        print_red(sprintf(_("Can't remove sieve script from server. Server says '%s'."),to_string($this->sieve_handle->error_raw)));
      }else{
        unset($this->scripts[$this->script_to_delete]);
      }
      $this->script_to_delete = -1;
    }

    if($this->script_to_delete != -1){
      $smarty = get_smarty();
      $smarty->assign("Warning",
          sprintf(_("You are going to remove the sieve script '%s' from your mail server."),
            $this->scripts[$this->script_to_delete]['NAME']));

      return($smarty->fetch(get_template_path("templates/remove_script.tpl",TRUE,dirname(__FILE__))));
    }

    /* Abort saving */
    if(isset($_POST['cancel_sieve_changes'])){
      $this->current_handler = NULL;
    }

    /* Save currently edited sieve script. */
    if(isset($_POST['save_sieve_changes'])){
      $chk = $this->current_handler->check();
      if(!count($chk)){

        $sc = $this->scripts[$this->current_script]['SCRIPT'];
        $p = new My_Parser;
        if($p -> parse($sc)){

          if($this->scripts[$this->current_script]['MODE'] == "Source-Only"){
            $this->scripts[$this->current_script]['MODE'] = "Source";
          }
  
          $this->scripts[$this->current_script]['PARSER'] = $p;
          $this->scripts[$this->current_script]['EDITED'] = TRUE;
          $this->scripts[$this->current_script]['STATUS'] = TRUE;
          $this->scripts[$this->current_script]['MSG'] = _("Edited");
          $this->current_handler = NULL;
        }else{
          print_red($p->status_text);;
        }
      }else{
        print_red(_("Please fix all errors before saving."));
      }
    }


    /*************
     * Display edit dialog 
     *************/

    /* Display edit dialog, depending on Mode display different uis
     */
    if($this->current_handler){

      $current = $this->scripts[$this->current_script];

      /* Create html results */
      $smarty = get_smarty();
      $smarty->assign("Mode",$current['MODE']);
      if($current['MODE'] == "Structured"){
        $smarty->assign("Contents",$this->current_handler->tree_->execute());
      }else{
        $smarty->assign("Contents",$current['SCRIPT']);
      }
      $ret = $smarty->fetch(get_template_path("templates/edit_frame_base.tpl",TRUE,dirname(__FILE__)));
      return($ret);
    }


    /* Create list of available sieve scripts 
     */
    $List = new divSelectBox("sieveManagement");
    foreach($this->scripts as $key => $script){
  
      $edited =  $script['EDITED'];
      $active =  $script['ACTIVE'];
      
      $field1 = array("string" => "&nbsp;",
                      "attach" => "style='width:20px;'");  
      if($active){
        $field1 = array("string" => "<img src='images/true.png' alt='"._("Active")."'>",
                        "attach" => "style='width:20px;'");  
      }
      $field2 = array("string" => $script['NAME']);  
      $field3 = array("string" => $script['MSG']);
      $field4 = array("string" => _("Script length")."&nbsp;:&nbsp;".strlen($script['SCRIPT']));

      if($edited){
        $field5 = array("string" => "<img src='images/fai_new_hook.png' alt='"._("Edited")."'>",
                        "attach" => "style='width:30px;'");
      }else{
        $field5 = array("string" => "",
                        "attach" => "style='width:30px;'");
      }

      if($active){
        $field6 = array("string" => "<img src='images/empty.png' alt=' '>".
                                    "<input type='image' name='editscript_".$key."' src='images/edit.png'>".
                                    "<input type='image' name='delscript_".$key."' src='images/edittrash.png'>");
      }else{
        $field6 = array("string" => "<input type='image' name='active_script_".$key."' src='images/true.png'>".
                                    "<input type='image' name='editscript_".$key."' src='images/edit.png'>".
                                    "<input type='image' name='delscript_".$key."' src='images/edittrash.png'>");
      }
      $List ->AddEntry(array($field1,$field2,$field3,$field4,$field5,$field6)); 
    }
  
    $display ="<h2>Sieve script management</h2>";
    $display .= _("Be careful. All your changes will be saved directly to sieve, if you use the save button below.");
    $display .= "<br><input type='submit' name='create_new_script' value='"._("Create new script")."'>";
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

      $Mode = $this->scripts[$this->current_script]['MODE'];
      $skip_mode_change = false;
      if(in_array($Mode,array("Source-Only","Source"))){
        if(isset($_POST['script_contents'])){
          $sc = stripslashes($_POST['script_contents']);
          $this->scripts[$this->current_script]['SCRIPT'] = $sc;
          $p = new My_Parser;
          if($p -> parse($sc)){
            $this->current_handler = $p;
          } else {
            print_red($p->status_text);
            $skip_mode_change = TRUE;
          }
        }
      }
      if(in_array($Mode,array("Structured"))){
        $this->current_handler->save_object();
        $sc = $this->current_handler->get_sieve_script();
        $this->scripts[$this->current_script]['SCRIPT'] = $sc;
        $p = new My_Parser;
        if($p -> parse($sc)){
          $this->current_handler = $p;
        } else {
          print_red($p->status_text);
          $skip_mode_change = TRUE;
        }
      }
      if(!$skip_mode_change){
        if($this->scripts[$this->current_script]['MODE'] != "Source-Only"){
          if(isset($_POST['View_Source'])){
            $this->scripts[$this->current_script]['MODE'] = "Source";
          }
          if(isset($_POST['View_Structured'])){
            $this->scripts[$this->current_script]['MODE'] = "Structured";
          }
        }
      }
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
        $data = $this->scripts[$key]['SCRIPT'];
        if(!$this->sieve_handle->sieve_sendscript($script['NAME'], $data)){
          gosa_log("Failed to save sieve script named '".$script['NAME']."': ".to_string($this->sieve_handle->error_raw));
          $everything_went_fine = FALSE;
          print_red(to_string($this->sieve_handle->error_raw));
          $this->scripts[$key]['MSG'] = "<font color='red'>".
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
