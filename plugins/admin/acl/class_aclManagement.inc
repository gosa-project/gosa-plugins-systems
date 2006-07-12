<?php
/*
   This code is part of GOsa (https://gosa.gonicus.de)
   Copyright (C) 2003-2006 - Cajus Pollmeier <pollmeier@gonicus.de>

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


class aclManagement extends plugin
{
  /* Plugin definitions */
  var $plHeadline     = "ACLs";
  var $plDescription  = "This does something";

  /* Dialog attributes */
  var $acltabs              = NULL;
  var $ui                   = NULL;
  var $acl                  = "";
  var $DivListACL           = NULL;

  function aclManagement($config, $ui)
  {
    /* Save configuration for internal use */
    $this->config = $config;
    $this->ui     = $ui;

    /* Creat dialog object */
    $this->DivListACL = new divListACL($this->config,$this);
  }


  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    $smarty       = get_smarty();                 // Smarty instance
    $s_action     = "";                           // Contains the action to be taken
    $s_entry      = "";                           // The value for s_action

    /* Edit entry button pressed? */
    if( isset($_GET['act']) && $_GET['act'] == "edit_entry" ){
      $s_action= "edit";
      $s_entry= validate($_GET['id']);
    }

    $types = array("del"=>"acl_del","edit"=>"acl_edit");

    /* Test relevant POST values */  
    foreach($_POST as $key => $val){

      /* Get every possible POST combination and set s_action/s_entry accordingly */
      foreach($types as $act => $name){

        if (preg_match("/".$name.".*/", $key)){
          $s_action= $act;
          $s_entry= preg_replace("/".$name."_/i", "", $key);
        }
      }
    } 

    /* Remove coordinate prefix from POST, required by some browsers */
    $s_entry= preg_replace("/_.$/", "", $s_entry);

    /* Seperate possibly encoded tab and entry, default to tab "acl" */
    if(preg_match("/.*-.*/", $s_entry)){
      $s_tab= preg_replace("/^[^-]*-/i", "" ,$s_entry);
      $s_entry= preg_replace("/-[^-]*$/i", "", $s_entry);
    }else{
      $s_tab= "generic";
    }

    /********************
      Edit existing entry 
     ********************/

    if (($s_action=="edit") && (!isset($this->acltabs->config))){

      /* Get 'dn' from posted acl, must be unique */
      $this->dn= $this->list[trim($s_entry)]['dn'];

      /* Check locking, save current plugin in 'back_plugin', so
         the dialog knows where to return. */
      if (($acl= get_lock($this->dn)) != ""){
        $_SESSION['LOCK_VARS_TO_USE'] = array("/^acl_edit/","/^acl_del/","/^id_/");
        return(gen_locked_message ($acl, $this->dn));
      }

      /* Lock the current entry, so everyone will get the above dialog */
      add_lock ($this->dn, $this->ui->dn);

      /* Set up the ACL's for this 'dn' */
      $acl= get_permissions ($this->dn, $this->ui->subtreeACL);

      /* Register acltabs to trigger edit dialog */ 
      $this->acltabs= new acl($this->config, $this->dn);

      /* Switch tab, if it was requested by user */
      $this->acltabs->current = $s_tab;

      /* Set ACL and move DN to the headline */
      $_SESSION['objectinfo']= $this->dn;
    }


    /********************
      Edit canceled 
     ********************/

    /* Reset all relevant data, if we get a _cancel request */
    if (isset($_POST['edit_cancel']) || isset($_POST['cancel_new_acl'])){
      if (isset($this->acltabs)){
        del_lock ($this->dn);
        unset ($this->acltabs);
      }
      $this->acltabs= NULL;
      unset ($_SESSION['objectinfo']);
    }


    /********************
      Delete entry requested, display confirm dialog
     ********************/

    /* Remove acl was requested */
    if ($s_action=="del"){

      /* Get 'dn' from posted acl */
      $this->dn= $this->list[trim($s_entry)]['dn'];

      /* Load permissions for selected 'dn' and check if
         we're allowed to remove this 'dn' */
      $acl      = get_permissions ($this->dn, $this->ui->subtreeACL);
      $this->acl= get_module_permission($acl, "acl", $this->dn);
      if (chkacl($this->acl, "delete") == ""){

        /* Check locking, save current plugin in 'back_plugin', so
           the dialog knows where to return. */
        if (($acl= get_lock($this->dn)) != ""){
          $_SESSION['LOCK_VARS_TO_USE'] = array("/^acl_edit/","/^acl_del/","/^id_/");
          return(gen_locked_message ($acl, $this->dn));
        }

        /* Lock the current entry, so nobody will edit it during deletion */
        add_lock ($this->dn, $this->ui->dn);
        $smarty->assign("info", sprintf(_("You're about to delete the acl %s."), @LDAP::fix($this->dn)));
        return($smarty->fetch(get_template_path('remove.tpl', TRUE)));
      } else {

        /* Obviously the acl isn't allowed to delete. Show message and
           clean session. */
        print_red (_("You are not allowed to delete this acl!"));
      }
    }


    /********************
      Delete entry confirmed 
     ********************/

    /* Confirmation for deletion has been passed. Acl should be deleted. */
    if (isset($_POST['delete_acl_confirmed'])){

      /* Some nice guy may send this as POST, so we've to check
         for the permissions again. */
      if (chkacl($this->acl, "delete") == ""){

        /* Delete request is permitted, perform LDAP action */
        $this->acltabs= new acl($this->config, $this->dn);
        $this->acltabs->delete ();
        gosa_log ("ACL object '".$this->dn."' has been removed");
        unset ($this->acltabs);
        $this->acltabs= NULL;
      } else {

        /* Normally this shouldn't be reached, send some extra
           logs to notify the administrator */
        print_red (_("You are not allowed to delete this acl!"));

        if(isset($this->ui->uid)){
          gosa_log ("Warning: '".$this->ui->uid."' tried to trick acl deletion.");
        }
      }

      /* Remove lock file after successfull deletion */
      del_lock ($this->dn);
    }

    
    /********************
      Delete entry Canceled 
     ********************/

    /* Delete acl canceled? */
    if (isset($_POST['delete_cancel'])){
      del_lock ($this->dn);
    }


    /********************
      Edit entry finished (Save) 
     ********************/

    /* Finish acl edit is triggered by the tabulator dialog, so
       the acl wants to save edited data. Check and save at this
       point. */
    if ((isset($_POST['submit_new_acl']) || isset($_POST['edit_finish']) || isset($_POST['edit_apply'])) && (isset($this->acltabs->config))){

      /* Check tabs, will feed message array */
      $this->acltabs->last= $this->acltabs->current;
      $this->acltabs->save_object();
      $message= $this->acltabs->check();

      /* Save, or display error message? */
      if (count($message) == 0){

        /* No errors. Go ahead and prepare to ask for a password
           in case we're creating a new acl. 'dn' will be 'new'
           in this case. It is set to the correct value later. */
        if ($this->dn == "new"){
          $set_pass= 1;
        } else {
          $set_pass= 0;
        }

        /* Save acl data to ldap */
        if($this->acltabs->save() == 1){
          gosa_log ("ACL object '".$this->dn."' saving failed.");
          return;
        }
        gosa_log ("ACL object '".$this->dn."' has been saved");

        if (!isset($_POST['edit_apply'])){

          /* ACl has been saved successfully, remove lock from LDAP. */
          if ($this->dn != "new"){
            del_lock ($this->dn);
          }

          unset ($this->acltabs);
          $this->acltabs= NULL;
          unset ($_SESSION['objectinfo']);
        }
      } else {
        /* Ok. There seem to be errors regarding to the tab data,
           show message and continue as usual. */
        show_errors($message);
      }
    }


    /********************
      Display subdialog 
     ********************/

    /* Show tab dialog if object is present */
    if(isset($this->acltabs->config)){
      $display= $this->acltabs->execute();

      /* Don't show buttons if tab dialog requests this */
#      if(isset($this->acltabs->by_object)){
#        if (!$this->acltabs->dialog){
#          $display.= "<p style=\"text-align:right\">\n";
#          $display.= "<input type=submit name=\"edit_finish\" style=\"width:80px\" value=\""._("Ok")."\">\n";
#          $display.= "&nbsp;\n";
#          if ($this->dn != "new"){
#            $display.= "<input type=submit name=\"edit_apply\" value=\""._("Apply")."\">\n";
#            $display.= "&nbsp;\n";
#          }
#          $display.= "<input type=submit name=\"edit_cancel\" value=\""._("Cancel")."\">\n";
#          $display.= "</p>";
#        }
#      }
      return ($display);
    }
    
    /* Check if there is a snapshot dialog open */
    $base = $this->DivListACL->selectedBase;
    if($str = $this->showSnapshotDialog($base,$this->get_used_snapshot_bases())){
      return($str);
    }
  
    /* Return rendered main page */
        /* Display dialog with system list */
    $this->DivListACL->parent = $this;
    $this->DivListACL->execute();

    /* Add departments if subsearch is disabled */
    if(!$this->DivListACL->SubSearch){
      $this->DivListACL->AddDepartments($this->DivListACL->selectedBase,3);
    }
    $this->reload();
    $this->DivListACL->setEntries($this->list);
    return($this->DivListACL->Draw());
  }


  /* Return departments, that will be included within snapshot detection */
  function get_used_snapshot_bases()
  {
    return(array(get_people_ou().$this->DivListACL->selectedBase));
  }  


  function reload()
  {
    #FIXME  Implement regex objectClass and so on ... 
    /* Get divlist informations from filter part */
    $Regex      = $this->DivListACL -> Regex;
    $SubSearch  = $this->DivListACL -> SubSearch;
    $base       = $_SESSION['CurrentMainBase']; 
    $Attrs      = array("ou","gosaAclEntry","objectClass");

    /* Get ldap connection */
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($base);

    /* Create filter */
    $Filter = "objectClass=gosaACL";
    $Flags  = GL_SIZELIMIT;

    /* Subserach ? */
    if($SubSearch){
      $Flags |= GL_SUBSEARCH;
    }

    /* Get objects */
    $res= get_list($Filter, $this->ui->subtreeACL,$base, $Attrs, $Flags);

    $this->list = $res;
  }


  function remove_lock()
  {
    /* Remove acl lock if a DN is marked as "currently edited" */
    if (isset($this->acltabs->dn)){
      del_lock ($this->acltabs->dn);
    }
  }


  function save_object()
  {
    /* Handle divlist filter && department selection*/
    if(!is_object($this->acltabs)){
      $this->DivListACL->save_object();
    }
  }

  /* A set of disabled and therefore overloaded functions. They are
     not needed in this class. */
  function remove_from_parent() { } 
  function check() { } 
  function save() { } 
  function adapt_from_template($dn) { } 
  function password_change_needed() { } 
  function show_header($button_text, $text, $disabled= FALSE) { }

} 
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
