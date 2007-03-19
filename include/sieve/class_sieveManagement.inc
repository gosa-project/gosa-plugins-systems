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
  var $Script_Error = "";
  var $Sieve_Error = "";
  var $create_script = FALSE;

  /* To add new elements we need to know 
   *  Where to the element                  -> add_new_id
   *  Whould we add above or below this id  -> add_above_below
   *  What kind of element should we add    -> add_element_type
   */
  var $add_new_element    = FALSE;
  var $add_new_id         = -1;
  var $add_above_below    = "below";
  var $add_element_type   = "sieve_comment";

  /* If this variable is TRUE, this indicates that we have the 
   *  import dialog opened. 
   */
  var $Import_Script = FALSE;

  /* Initialize the class and load all sieve scripts 
   *  try to parse them and display errors 
   */ 
  function sieveManagement($config,$dn,$parent)
  {
    $this->parent = $parent;
    plugin::plugin($config,$dn);

    /* Get sieve */
    if(!$this->sieve_handle = $this->get_sieve()){
      print_red(
        sprintf(
          _("Can't log into SIEVE server. Server says '%s'."),
          to_string($this->Sieve_Error)));
      return;
    }


    /* Get all sieve scripts names */
    if($this->sieve_handle->sieve_listscripts()){
      if (is_array($this->sieve_handle->response)){
        foreach($this->sieve_handle->response as $key => $name){

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
      $p = new My_Parser($this);
      $this->sieve_handle->sieve_getscript($script['NAME']);

      $script = "";
      foreach($this->sieve_handle->response as $line){
        $script.=$line;
      }

      $this->scripts[$key]['IS_NEW'] = FALSE;;
      $this->scripts[$key]['SCRIPT'] = $script;
      $this->scripts[$key]['ORIG_SCRIPT'] = $script;
      $this->scripts[$key]['MSG']   = "";
      $ret = $p->parse($script);
      if(!$ret){
        $this->scripts[$key]['STATUS']   = FALSE;
        $this->scripts[$key]['MODE']    = "Source";
        $this->scripts[$key]['MSG'] = _("Parse failed")."<font color='red'>".$p->status_text."</font>";
      }else{
        $this->scripts[$key]['STATUS']   = TRUE;
        $this->scripts[$key]['MODE']    = "Structured";
        $this->scripts[$key]['MSG'] = _("Parse successful");
      }
      $this->scripts[$key]['PARSER'] = $p;
      $this->scripts[$key]['EDITED'] = FALSE;
    }
    $this->sieve_handle = $this->sieve_handle;
  }


  /* Return a sieve class hanlde,
   *  false if login fails
   */
  function get_sieve()
  {
    /* Connect to sieve class and try to get all available sieve scripts */
    $cfg=  $this->config->data['SERVERS']['IMAP'][$this->parent->gosaMailServer];
    $this->Sieve_Error = "";
  
    /* Log into the mail server */
    $this->sieve_handle= new sieve(
        $cfg["sieve_server"], 
        $cfg["sieve_port"], 
        $this->parent->uid, 
        $cfg["password"], 
        $cfg["admin"]);

    /* Try to login */
    if (!$this->sieve_handle->sieve_login()){
      $this->Sieve_Error = $this->sieve_handle->error_raw;
      return(FALSE);
    }
    return($this->sieve_handle);
  }


  /* Handle sieve list 
   */
  function execute()
  {
    /***************
     * Create a new Script 
     ***************/

    /* Force opening the add script dialog */
    if(isset($_POST['create_new_script'])){
      $this->create_script = TRUE;
    }

    /* Close add script dialog, without adding a new one */
    if(isset($_POST['create_script_cancel'])){
      $this->create_script = FALSE;
    }

    /* Display create script dialog 
     *  handle posts, display warnings if specified 
     *  name is not useable. 
     * Create a new script with given name
     */
    if($this->create_script){
    
      /* Set initial name or used posted name if available */
      $name = "";
      if(isset($_POST['NewScriptName'])){
        $name = trim($_POST['NewScriptName']);
      }
 
      /* Check given name */ 
      $err = "";

      /* Is given name in lower case characters ? */
      if(isset($_POST['create_script_save'])){
        if(!strlen($name)){
          $err = _("You should specify a name for your new script.");
        }
        /* Is given name in lower case characters ? */
        if($name != strtolower($name)){
          $err = _("Only lower case names are allowed here.");
        }

        /* Only chars are allowed here */
        if(preg_match("/[^a-z]/i",$name)){
          $err = _("Only a-z are allowed in script names.");
        }

        $tmp = $this->get_used_script_names();
        if(in_array_ics($name,$tmp)){
          $err =_("The specified name is already in use.");
        }
      }

      /* Create script if everything is ok */
      if($this->create_script && isset($_POST['create_script_save']) && $err == "" ){

        /* Close dialog */
        $this->create_script = FALSE;

        /* Script contents to use */
        $script = "/*New script */".
                  "stop;";

        /* Create a new parser and initialize default values */
        $p = new My_Parser($this);
        $ret = $p->parse($script);
        $sc['SCRIPT'] = $script;
        $sc['ORIG_SCRIPT'] = $script;
        $sc['IS_NEW'] = TRUE;
        $sc['MSG']   = "";
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
      
        /* Add script */
        $this->scripts[$name] = $sc;
      }else{
      
        /* Display dialog to enter new script name */
        $smarty = get_smarty();
        $smarty->assign("NewScriptName",$name);
        $smarty->assign("Error",$err);
        return($smarty->fetch(get_template_path("templates/create_script.tpl",TRUE,dirname(__FILE__))));
      }
    }


    /*************
     * Handle several posts 
     *************/

    $once = TRUE;
    foreach($_POST as $name => $value){

      /* Edit script requested */
      if(preg_match("/^editscript_/",$name) && $once && !$this->current_handler){
        $script = preg_replace("/^editscript_/","",$name);
        $script = preg_replace("/_(x|y)/","",$script);
        $once = FALSE;

        $this->current_script = $script;
        $this->current_handler = $this->scripts[$script]['PARSER'];
      }

      /* remove script requested */
      if(preg_match("/^delscript_/",$name) && $once && !$this->current_handler){
        $script = preg_replace("/^delscript_/","",$name);
        $script = preg_replace("/_(x|y)/","",$script);
        $once = FALSE;
 
        $this->script_to_delete = $script;  
      }

      /* Activate script */
      if(preg_match("/^active_script_/",$name) && $once && !$this->current_handler){
        $script = preg_replace("/^active_script_/","",$name);
        $script = preg_replace("/_(x|y)/","",$script);
        $once = FALSE;

        /* We can only activate existing scripts */
        if(!$this->scripts[$script]['IS_NEW']){

          /* Get sieve */
          if(!$this->sieve_handle = $this->get_sieve()){
            print_red(
                sprintf(
                  _("Can't log into SIEVE server. Server says '%s'."),
                  to_string($this->Sieve_Error)));
          }

          /* Try to activate the given script and update 
           *  class script array. 
           */
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
    }

    
    /*************
     * Remove script handling 
     *************/

    /* Remove aborted */
    if(isset($_POST['delete_cancel'])){
      $this->script_to_delete = -1;
    }

    /* Remove confirmed */
    if(isset($_POST['delete_script_confirm'])){

      $script = $this->scripts[$this->script_to_delete];

      /* Just remove from array if it is a new script */
      if($script['IS_NEW']){
        unset($this->scripts[$this->script_to_delete]);
      }else{

        /* Get sieve */
        if(!$this->sieve_handle = $this->get_sieve()){
          print_red(
              sprintf(
                _("Can't log into SIEVE server. Server says '%s'."),
                to_string($this->Sieve_Error)));
        }

        if(!$this->sieve_handle->sieve_deletescript($this->scripts[$this->script_to_delete]['NAME'])){
          print_red(sprintf(_("Can't remove sieve script from server. Server says '%s'."),to_string($this->sieve_handle->error_raw)));
        }else{
          unset($this->scripts[$this->script_to_delete]);
        }
      }
      $this->script_to_delete = -1;
    }

    /* Display confirm dialog */
    if($this->script_to_delete != -1){
      $smarty = get_smarty();
      $smarty->assign("Warning",
          sprintf(_("You are going to remove the sieve script '%s' from your mail server."),
            $this->scripts[$this->script_to_delete]['NAME']));
      return($smarty->fetch(get_template_path("templates/remove_script.tpl",TRUE,dirname(__FILE__))));
    }


    /**************
     * Save script changes 
     **************/

    /* Abort saving */
    if(isset($_POST['cancel_sieve_changes'])){
      $this->current_handler = NULL;
    }

    /* Save currently edited sieve script. */
    if(isset($_POST['save_sieve_changes'])){
      $chk = $this->current_handler->check();
      if(!count($chk)){

        $sc = $this->scripts[$this->current_script]['SCRIPT'];
        $p = new My_Parser($this);
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

        if(isset($_POST['Import_Script'])){
          $this->Import_Script = TRUE;
        }

        if(isset($_POST['Import_Script_Cancel'])){
          $this->Import_Script = FALSE;
        }

        if(isset($_POST['Import_Script_Save']) && isset($_FILES['Script_To_Import'])){

          $file     = $_FILES['Script_To_Import'];

          if($file['size'] == 0){
            print_red(_("Specified file seams to empty."));
          }elseif(!file_exists($file['tmp_name'])){
            print_red(_("Upload failed, somehow nothing was uploaded or the temporary file can't be accessed."));
          }elseif(!is_readable ($file['tmp_name'])){
            print_red(sprintf(_("Can't open file '%s' to read uploaded file contents."),$file['tmp_name']));
          }else{
            
            
 
            $contents = file_get_contents($file['tmp_name']);
           
            $this->scripts[$this->current_script]['SCRIPT'] = $contents;
            if(!$this->current_handler->parse($contents)){
              $this->scripts[$this->current_script]['MODE'] = "Source";
            }else{
              $this->scripts[$this->current_script]['MODE'] = "Structured";
            }
            $this->Import_Script = FALSE;
          }
        }

        if($this->Import_Script){
          $smarty = get_smarty();
          $str = $smarty->fetch(get_template_path("templates/import_script.tpl",TRUE,dirname(__FILE__)));
          return($str);
        }
  

        /* Create dump of current sieve script */
        if(isset($_POST['Save_Copy'])){

            /* force download dialog */
            header("Content-type: application/tiff\n");
            if (preg_match('/MSIE 5.5/', $HTTP_USER_AGENT) ||
                    preg_match('/MSIE 6.0/', $HTTP_USER_AGENT)) {
                header('Content-Disposition: filename="dump.script"');
            } else {
                header('Content-Disposition: attachment; filename="dump.script"');
            }
            header("Content-transfer-encoding: binary\n");
            header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
            header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
            header("Cache-Control: no-cache");
            header("Pragma: no-cache");
            header("Cache-Control: post-check=0, pre-check=0");
            echo $this->scripts[$this->current_script]['SCRIPT'];
            exit();
        }


      /****
       * Add new element to ui
       ****/

      /* Abort add dialog */ 
      if(isset($_POST['select_new_element_type_cancel'])){
        $this->add_new_element = FALSE;
      }

      /* Add a new element */
      if($this->add_new_element){

        $element_types= array(
            "sieve_keep"      => _("Keep"),
            "sieve_comment"   => _("Comment"),
            "sieve_fileinto"  => _("File into"),
            "sieve_keep"      => _("Keep"),
            "sieve_discard"   => _("Discard"),
            "sieve_redirect"  => _("Redirect"),
            "sieve_reject"    => _("Reject"),
            "sieve_require"   => _("Require"),
            "sieve_stop"      => _("Stop"),
            "sieve_vacation"  => _("Vacation message"),
            "sieve_if"        => _("If"));


        /* Element selected */
        if(isset($_POST['element_type']) && isset($element_types[$_POST['element_type']]) 
           || isset($_POST['element_type']) &&in_array($_POST['element_type'],array("sieve_else","sieve_elsif"))){
          $this->add_element_type = $_POST['element_type'];
        }

        /* Create new element and add it to
         *  the selected position 
         */
        if(isset($_POST['select_new_element_type'])){

          $this->add_new_id;
          $data = $this->current_handler->tree_->pap;

          /* Get index of the element identified by object_id == $this->add_new_id; */
          $index = -1;
          foreach($data as $key => $obj){
            if($obj->object_id == $this->add_new_id && $index==-1){
              $index = $key;
            }
          }
 
          /* We have found the specified object_id 
           *  and want to detect the next free position 
           *  to insert the new element.
           */
          if($index != -1){
            if($this->add_above_below == "above"){
              $direction ="up";
              $next_free = $this->current_handler->tree_->_get_next_free_move_slot($index,$direction);
              $next_free ++;
            }else{
              $direction = "down";
              $next_free = $this->current_handler->tree_->_get_next_free_move_slot($index,$direction);
            }
            $this->add_new_id = $this->current_handler->tree_->pap[$next_free]->object_id;
          }

          $parent = $this->current_handler->tree_;

          /* Create elements we should add */
          if($this->add_element_type == "sieve_if"){
            $ele[] = new $this->add_element_type(NULL, preg_replace("/[^0-9]/","",microtime()),$parent);
            $ele[] = new sieve_block_start(NULL,preg_replace("/[^0-9]/","",microtime()),$parent);
            $ele[] = new sieve_block_end(NULL,preg_replace("/[^0-9]/","",microtime()),$parent);
          }elseif($this->add_element_type == "sieve_else"){
            $ele[] = new sieve_block_end(NULL,preg_replace("/[^0-9]/","",microtime()),$parent);
            $ele[] = new $this->add_element_type(NULL, preg_replace("/[^0-9]/","",microtime()),$parent);
            $ele[] = new sieve_block_start(NULL,preg_replace("/[^0-9]/","",microtime()),$parent);
          }elseif($this->add_element_type == "sieve_elsif"){
            $ele[] = new sieve_block_end(NULL,preg_replace("/[^0-9]/","",microtime()),$parent);
            $ele[] = new $this->add_element_type(NULL, preg_replace("/[^0-9]/","",microtime()),$parent);
            $ele[] = new sieve_block_start(NULL,preg_replace("/[^0-9]/","",microtime()),$parent);
          }else{
            $ele[] = new $this->add_element_type(NULL, preg_replace("/[^0-9]/","",microtime()),$parent);
          }

          $start = $end = array();
          $found = false;

          /* Add above current element*/
          if($this->add_above_below == "above"){
            foreach($data as $key => $obj){
              if($obj->object_id == $this->add_new_id){
                $found = true;
              }
              if(!$found){
                $start[] = $obj;
              }else{
                $end[] = $obj;
              }
            }
          }else{
            /* Add below current element */
            foreach($data as $key => $obj){
              if(!$found){
                $start[] = $obj;
              }else{
                $end[] = $obj;
              }
              if($obj->object_id == $this->add_new_id){
                $found = true;
              }
            }
          }


          /* Only add, if current element could be located */
          if($found){
            $new = array();
            foreach($start as $obj){
              $new[] = $obj;
            }
            foreach($ele as $el){
              $new[] = $el;
            }
            foreach($end as $obj){
              $new[] = $obj;
            }
            $data= $new;
            $this->current_handler->tree_->pap = $data;
            $this->add_new_element = FALSE;
          }else{
            print_red(_("Something went wrong while adding a new entry."));
          }
        }
      }

      /* Only display select dialog if it is necessary */
      if($this->add_new_element){
        $smarty = get_smarty();
    
        $add_else_elsif = FALSE;
        foreach($this->current_handler->tree_->pap as $obj){
          if($obj->object_id == $this->add_new_id && in_array(get_class($obj),array("sieve_if","sieve_elsif"))){
            $add_else_elsif = TRUE;
          }
        }

        if($add_else_elsif && $this->add_above_below == "below"){
          $element_types['sieve_else'] = _("Else");
          $element_types['sieve_elsif'] = _("Else if");
        }
  
        $smarty->assign("element_types",$element_types );
        $smarty->assign("element_type",$this->add_element_type);
        $str = $smarty->fetch(get_template_path("templates/add_element.tpl",TRUE,dirname(__FILE__)));
        return($str);
      }



      /****************
       * Handle test posts 
       ****************/

      /* handle some special posts from test elements 
       */
      foreach($_POST as $name => $value){
        if(preg_match("/^Add_Test_Object_/",$name)) {
          $name = preg_replace("/^Add_Test_Object_/","",$name);
          $name = preg_replace("/_(x|y)$/","",$name);

          $test_types_to_add = array(
              "address" =>_("Address"),
              "header"  =>_("Header"),
              "envelope"=>_("Envelope"),
              "size"    =>_("Size"),
              "exists"  =>_("Exists"),
              "allof"   =>_("All of"),
              "anyof"   =>_("Any of"),
              "true"    =>_("True"),
              "false"   =>_("False"));

          $smarty = get_smarty();
          $smarty->assign("ID",$name);
          $smarty->assign("test_types_to_add",$test_types_to_add);
          $ret = $smarty->fetch(get_template_path("templates/select_test_type.tpl",TRUE,dirname(__FILE__)));
          return($ret);
        }
      }

      $current = $this->scripts[$this->current_script];

      /* Create html results */
      $smarty = get_smarty();
      $smarty->assign("Mode",$current['MODE']);
      if($current['MODE'] == "Structured"){
        $smarty->assign("Contents",$this->current_handler->tree_->execute());
      }else{
        $smarty->assign("Contents",$current['SCRIPT']);
      }
      $smarty->assign("Script_Error",$this->Script_Error);
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

      if($active || $script['IS_NEW']){
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

      /* Check if there is an add object requested 
       */
      $data = $this->current_handler->tree_->pap;
      $once = TRUE;
      foreach($_POST as $name => $value){
        foreach($data as $key => $obj){
          if(isset($obj->object_id) && preg_match("/^Add_Object_Top_".$obj->object_id."_/",$name) && $once){
            $once = FALSE;
            $this->add_new_element    = TRUE;
            $this->add_new_id         = $obj->object_id;
            $this->add_above_below    = "above";
          }
          if(isset($obj->object_id) && preg_match("/^Add_Object_Bottom_".$obj->object_id."_/",$name) && $once){
            $once = FALSE;
            $this->add_new_element    = TRUE;
            $this->add_new_id         = $obj->object_id;
            $this->add_above_below    = "below";
          }
        
          if(isset($obj->object_id) && preg_match("/^Remove_Object_".$obj->object_id."_/",$name) && $once){
            $once = FALSE;
            $this->current_handler->tree_->remove_object($key);
          }
          if(isset($obj->object_id) && preg_match("/^Move_Up_Object_".$obj->object_id."_/",$name) && $once){
            $this->current_handler->tree_->move_up_down($key,"up");
            $once = FALSE;
          }
          if(isset($obj->object_id) && preg_match("/^Move_Down_Object_".$obj->object_id."_/",$name) && $once){
            $this->current_handler->tree_->move_up_down($key,"down");
            $once = FALSE;
          }
        }
      }

      /* Skip Mode changes and Parse tests 
       *  if we are currently in a subdialog 
       */

      $this->current_handler->save_object();
      $Mode = $this->scripts[$this->current_script]['MODE'];
      $skip_mode_change = false;
      if(in_array($Mode,array("Source-Only","Source"))){
        if(isset($_POST['script_contents'])){
          $sc = stripslashes($_POST['script_contents']);
          $this->scripts[$this->current_script]['SCRIPT'] = $sc;
          $p = new My_Parser($this);
          if($p -> parse($sc)){
            $this->Script_Error = "";
          } else {
            $this->Script_Error = $p->status_text;
            $skip_mode_change = TRUE;
          }
        }
      }
      if(in_array($Mode,array("Structured"))){
        $sc = $this->current_handler->get_sieve_script();
        $this->scripts[$this->current_script]['SCRIPT'] = $sc;
        $p = new My_Parser($this);
        if($p -> parse($sc)){
          $this->Script_Error = "";
        } else {
          $this->Script_Error = $p->status_text;
          $skip_mode_change = TRUE;
        }
      }
      if(!$skip_mode_change){
        if($this->scripts[$this->current_script]['MODE'] != "Source-Only"){
          $old_mode = $this->scripts[$this->current_script]['MODE'];
          if(isset($_POST['View_Source'])){
            $this->scripts[$this->current_script]['MODE'] = "Source";
          }
          if(isset($_POST['View_Structured'])){
            $this->scripts[$this->current_script]['MODE'] = "Structured";
          }
          $new_mode = $this->scripts[$this->current_script]['MODE'];

          if($old_mode != $new_mode){

            $sc = $this->scripts[$this->current_script]['SCRIPT'];
            $p = new My_Parser($this);

            if($p -> parse($sc)){
              $this->current_handler->parse($sc);
              $this->Script_Error = "";
            } else {
              $this->Script_Error = $p->status_text;
            }
          } 
        }
      }
    }
  }

  
  function get_used_script_names()
  {
    $ret = array();
    foreach($this->scripts as $script){
      $ret[] = $script['NAME'];
    }
    return($ret);
  }



  function save()
  {
    /* Get sieve */
    if(!$this->sieve_handle = $this->get_sieve()){
      print_red(
          sprintf(
            _("Can't log into SIEVE server. Server says '%s'."),
            to_string($this->Sieve_Error)));
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
