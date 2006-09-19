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

class plugin
{
  /*!
    \brief Reference to parent object

    This variable is used when the plugin is included in tabs
    and keeps reference to the tab class. Communication to other
    tabs is possible by 'name'. So the 'fax' plugin can ask the
    'userinfo' plugin for the fax number.

    \sa tab
   */
  var $parent= NULL;

  /*!
    \brief Configuration container

    Access to global configuration
   */
  var $config= NULL;

  /*!
    \brief Mark plugin as account

    Defines whether this plugin is defined as an account or not.
    This has consequences for the plugin to be saved from tab
    mode. If it is set to 'FALSE' the tab will call the delete
    function, else the save function. Should be set to 'TRUE' if
    the construtor detects a valid LDAP object.

    \sa plugin::plugin()
   */
  var $is_account= FALSE;
  var $initially_was_account= FALSE;

  /*!
    \brief Mark plugin as template

    Defines whether we are creating a template or a normal object.
    Has conseqences on the way execute() shows the formular and how
    save() puts the data to LDAP.

    \sa plugin::save() plugin::execute()
   */
  var $is_template= FALSE;
  var $ignore_account= FALSE;
  var $is_modified= FALSE;

  /*!
    \brief Represent temporary LDAP data

    This is only used internally.
   */
  var $attrs= array();

  /* Keep set of conflicting plugins */
  var $conflicts= array();

  /* Save unit tags */
  var $gosaUnitTag= "";

  /*!
    \brief Used standard values

    dn
   */
  var $dn= "";
  var $uid= "";
  var $sn= "";
  var $givenName= "";
  var $acl= "*none*";
  var $dialog= FALSE;
  var $snapDialog = NULL;

  /* attribute list for save action */
  var $attributes= array();
  var $objectclasses= array();
  var $is_new= TRUE;
  var $saved_attributes= array();

  var $acl_base= "";
  var $acl_category= "";

  /* Plugin identifier */
  var $plHeadline= "";
  var $plDescription= "";

  /*! \brief plugin constructor

    If 'dn' is set, the node loads the given 'dn' from LDAP

    \param dn Distinguished name to initialize plugin from
    \sa plugin()
   */
  function plugin ($config, $dn= NULL, $parent= NULL)
  {
    /* Configuration is fine, allways */
    $this->config= $config;	
    $this->dn= $dn;

    /* Handle new accounts, don't read information from LDAP */
    if ($dn == "new"){
      return;
    }

    /* Save current dn as acl_base */
    $this->acl_base= $dn;

    /* Get LDAP descriptor */
    $ldap= $this->config->get_ldap_link();
    if ($dn != NULL){

      /* Load data to 'attrs' and save 'dn' */
      if ($parent != NULL){
        $this->attrs= $parent->attrs;
      } else {
        $ldap->cat ($dn);
        $this->attrs= $ldap->fetch();
      }

      /* Copy needed attributes */
      foreach ($this->attributes as $val){
        $found= array_key_ics($val, $this->attrs);
        if ($found != ""){
          $this->$val= $this->attrs["$found"][0];
        }
      }

      /* gosaUnitTag loading... */
      if (isset($this->attrs['gosaUnitTag'][0])){
        $this->gosaUnitTag= $this->attrs['gosaUnitTag'][0];
      }

      /* Set the template flag according to the existence of objectClass
         gosaUserTemplate */
      if (isset($this->attrs['objectClass'])){
        if (in_array ("gosaUserTemplate", $this->attrs['objectClass'])){
          $this->is_template= TRUE;
          @DEBUG (DEBUG_TRACE, __LINE__, __FUNCTION__, __FILE__,
              "found", "Template check");
        }
      }

      /* Is Account? */
      error_reporting(0);
      $found= TRUE;
      foreach ($this->objectclasses as $obj){
        if (preg_match('/top/i', $obj)){
          continue;
        }
        if (!isset($this->attrs['objectClass']) || !in_array_ics ($obj, $this->attrs['objectClass'])){
          $found= FALSE;
          break;
        }
      }
      error_reporting(E_ALL);
      if ($found){
        $this->is_account= TRUE;
        @DEBUG (DEBUG_TRACE, __LINE__, __FUNCTION__, __FILE__,
            "found", "Object check");
      }

      /* Prepare saved attributes */
      $this->saved_attributes= $this->attrs;
      foreach ($this->saved_attributes as $index => $value){
        if (preg_match('/^[0-9]+$/', $index)){
          unset($this->saved_attributes[$index]);
          continue;
        }
        if (!in_array($index, $this->attributes) && $index != "objectClass"){
          unset($this->saved_attributes[$index]);
          continue;
        }
        if ($this->saved_attributes[$index]["count"] == 1){
          $tmp= $this->saved_attributes[$index][0];
          unset($this->saved_attributes[$index]);
          $this->saved_attributes[$index]= $tmp;
          continue;
        }

        unset($this->saved_attributes["$index"]["count"]);
      }
    }

    /* Save initial account state */
    $this->initially_was_account= $this->is_account;
  }

  /*! \brief execute plugin

    Generates the html output for this node
   */
  function execute()
  {
    /* This one is empty currently. Fabian - please fill in the docu code */
    $_SESSION['current_class_for_help'] = get_class($this);
    /* Reset Lock message POST/GET check array, to prevent perg_match errors*/
    $_SESSION['LOCK_VARS_TO_USE'] =array();
  }

  /*! \brief execute plugin
     Removes object from parent
   */
  function remove_from_parent()
  {
    /* include global link_info */
    $ldap= $this->config->get_ldap_link();

    /* Get current objectClasses in order to add the required ones */
    $ldap->cat($this->dn);
    $tmp= $ldap->fetch ();
    if (isset($tmp['objectClass'])){
      $oc= $tmp['objectClass'];
    } else {
      $oc= array("count" => 0);
    }

    /* Remove objectClasses from entry */
    $ldap->cd($this->dn);
    $this->attrs= array();
    $this->attrs['objectClass']= array();
    for ($i= 0; $i<$oc["count"]; $i++){
      if (!in_array_ics($oc[$i], $this->objectclasses)){
        $this->attrs['objectClass'][]= $oc[$i];
      }
    }

    /* Unset attributes from entry */
    foreach ($this->attributes as $val){
      $this->attrs["$val"]= array();
    }

    /* Unset account info */
    $this->is_account= FALSE;

    /* Do not write in plugin base class, this must be done by
       children, since there are normally additional attribs,
       lists, etc. */
    /*
       $ldap->modify($this->attrs);
     */
  }


  /* Save data to object */
  function save_object()
  {
    /* Save values to object */
    foreach ($this->attributes as $val){
      if ($this->acl_is_writeable($val) && isset ($_POST["$val"])){
        /* Check for modifications */
        if (get_magic_quotes_gpc()) {
          $data= stripcslashes($_POST["$val"]);
        } else {
          $data= $this->$val = $_POST["$val"];
        }
        if ($this->$val != $data){
          $this->is_modified= TRUE;
        }
    
        /* Okay, how can I explain this fix ... 
         * In firefox, disabled option fields aren't selectable ... but in IE you can select these fileds. 
         * So IE posts these 'unselectable' option, with value = chr(194) 
         * chr(194) seems to be the &nbsp; in between the ...option>&nbsp;</option.. because there is no value=".." specified in these option fields  
         * This &nbsp; was added for W3c compliance, but now causes these ... ldap errors ... 
         * So we set these Fields to ""; a normal empty string, and we can check these values in plugin::check() again ...
         */
        if(isset($data[0]) && $data[0] == chr(194)) {
          $data = "";  
        }
        $this->$val= $data;
        //echo "<font color='blue'>".$val."</font><br>";
      }else{
        //echo "<font color='red'>".$val."</font><br>";
      }
    }
  }


  /* Save data to LDAP, depending on is_account we save or delete */
  function save()
  {
    /* include global link_info */
    $ldap= $this->config->get_ldap_link();

    /* Start with empty array */
    $this->attrs= array();

    /* Get current objectClasses in order to add the required ones */
    $ldap->cat($this->dn);
    
    $tmp= $ldap->fetch ();
    
    if (isset($tmp['objectClass'])){
      $oc= $tmp["objectClass"];
      $this->is_new= FALSE;
    } else {
      $oc= array("count" => 0);
      $this->is_new= TRUE;
    }

    /* Load (minimum) attributes, add missing ones */
    $this->attrs['objectClass']= $this->objectclasses;
    for ($i= 0; $i<$oc["count"]; $i++){
      if (!in_array_ics($oc[$i], $this->objectclasses)){
        $this->attrs['objectClass'][]= $oc[$i];
      }
    }

    /* Copy standard attributes */
    foreach ($this->attributes as $val){
      if ($this->$val != ""){
        $this->attrs["$val"]= $this->$val;
      } elseif (!$this->is_new) {
        $this->attrs["$val"]= array();
      }
    }

  }


  function cleanup()
  {
    foreach ($this->attrs as $index => $value){

      /* Convert arrays with one element to non arrays, if the saved
         attributes are no array, too */
      if (is_array($this->attrs[$index]) && 
          count ($this->attrs[$index]) == 1 &&
          isset($this->saved_attributes[$index]) &&
          !is_array($this->saved_attributes[$index])){
          
        $tmp= $this->attrs[$index][0];
        $this->attrs[$index]= $tmp;
      }

      /* Remove emtpy arrays if they do not differ */
      if (is_array($this->attrs[$index]) &&
          count($this->attrs[$index]) == 0 &&
          !isset($this->saved_attributes[$index])){
          
        unset ($this->attrs[$index]);
        continue;
      }

      /* Remove single attributes that do not differ */
      if (!is_array($this->attrs[$index]) &&
          isset($this->saved_attributes[$index]) &&
          !is_array($this->saved_attributes[$index]) &&
          $this->attrs[$index] == $this->saved_attributes[$index]){

        unset ($this->attrs[$index]);
        continue;
      }

      /* Remove arrays that do not differ */
      if (is_array($this->attrs[$index]) && 
          isset($this->saved_attributes[$index]) &&
          is_array($this->saved_attributes[$index])){
          
        if (!array_differs($this->attrs[$index],$this->saved_attributes[$index])){
          unset ($this->attrs[$index]);
          continue;
        }
      }
    }
  }

  /* Check formular input */
  function check()
  {
    $message= array();

    /* Skip if we've no config object */
    if (!isset($this->config)){
      return $message;
    }

    /* Find hooks entries for this class */
    $command= search_config($this->config->data['MENU'], get_class($this), "CHECK");
    if ($command == "" && isset($this->config->data['TABS'])){
      $command= search_config($this->config->data['TABS'], get_class($this), "CHECK");
    }

    if ($command != ""){

      if (!check_command($command)){
        $message[]= sprintf(_("Command '%s', specified as CHECK hook for plugin '%s' doesn't seem to exist."), $command,
                            get_class($this));
      } else {

        /* Generate "ldif" for check hook */
        $ldif= "dn: $this->dn\n";
        
        /* ... objectClasses */
        foreach ($this->objectclasses as $oc){
          $ldif.= "objectClass: $oc\n";
        }
        
        /* ... attributes */
        foreach ($this->attributes as $attr){
          if ($this->$attr == ""){
            continue;
          }
          if (is_array($this->$attr)){
            foreach ($this->$attr as $val){
              $ldif.= "$attr: $val\n";
            }
          } else {
              $ldif.= "$attr: ".$this->$attr."\n";
          }
        }

        /* Append empty line */
        $ldif.= "\n";

        /* Feed "ldif" into hook and retrieve result*/
        $descriptorspec = array( 0 => array("pipe", "r"), 1 => array("pipe", "w"), 2 => array("pipe", "w"));
        $fh= proc_open($command, $descriptorspec, $pipes);
        if (is_resource($fh)) {
          fwrite ($pipes[0], $ldif);
          fclose($pipes[0]);
          
          $result= stream_get_contents($pipes[1]);
          if ($result != ""){
            $message[]= $result;
          }
          
          fclose($pipes[1]);
          fclose($pipes[2]);
          proc_close($fh);
        }
      }

    }

    return ($message);
  }

  /* Adapt from template, using 'dn' */
  function adapt_from_template($dn)
  {
    /* Include global link_info */
    $ldap= $this->config->get_ldap_link();

    /* Load requested 'dn' to 'attrs' */
    $ldap->cat ($dn);
    $this->attrs= $ldap->fetch();

    /* Walk through attributes */
    foreach ($this->attributes as $val){

      if (isset($this->attrs["$val"][0])){

        /* If attribute is set, replace dynamic parts: 
           %sn, %givenName and %uid. Fill these in our local variables. */
        $value= $this->attrs["$val"][0];

        foreach (array("sn", "givenName", "uid") as $repl){
          if (preg_match("/%$repl/i", $value)){
            $value= preg_replace ("/%$repl/i", $this->parent->$repl, $value);
          }
        }
        $this->$val= $value;
      }
    }

    /* Is Account? */
    $found= TRUE;
    foreach ($this->objectclasses as $obj){
      if (preg_match('/top/i', $obj)){
        continue;
      }
      if (!in_array_ics ($obj, $this->attrs['objectClass'])){
        $found= FALSE;
        break;
      }
    }
    if ($found){
      $this->is_account= TRUE;
    }
  }

  /* Indicate whether a password change is needed or not */
  function password_change_needed()
  {
    return FALSE;
  }


  /* Show header message for tab dialogs */
  function show_enable_header($button_text, $text, $disabled= FALSE)
  {
    if (($disabled == TRUE) || (!$this->acl_is_createable())){
      $state= "disabled";
    } else {
      $state= "";
    }
    $display= "<table summary=\"\" width=\"100%\"><tr>\n<td colspan=2><p><b>$text</b></p>\n";
    $display.= "<input type=submit value=\"$button_text\" name=\"modify_state\" ".$state.
      "><p class=\"seperator\">&nbsp;</p></td></tr></table>";

    return($display);
  }


  /* Show header message for tab dialogs */
  function show_disable_header($button_text, $text, $disabled= FALSE)
  {
    if (($disabled == TRUE) || !$this->acl_is_removeable()){
      $state= "disabled";
    } else {
      $state= "";
    }
    $display= "<table summary=\"\" width=\"100%\"><tr>\n<td colspan=2><p><b>$text</b></p>\n";
    $display.= "<input type=submit value=\"$button_text\" name=\"modify_state\" ".$state.
      "><p class=\"seperator\">&nbsp;</p></td></tr></table>";

    return($display);
  }


  /* Show header message for tab dialogs */
  function show_header($button_text, $text, $disabled= FALSE)
  {
    echo "FIXME: show_header should be replaced by show_disable_header and show_enable_header<br>";
    if ($disabled == TRUE){
      $state= "disabled";
    } else {
      $state= "";
    }
    $display= "<table summary=\"\" width=\"100%\"><tr>\n<td colspan=2><p><b>$text</b></p>\n";
    $display.= "<input type=submit value=\"$button_text\" name=\"modify_state\" ".
      ($this->acl_is_createable()?'':'disabled')." ".$state.
      "><p class=\"seperator\">&nbsp;</p></td></tr></table>";

    return($display);
  }


  function postcreate($add_attrs= array())
  {
    /* Find postcreate entries for this class */
    $command= search_config($this->config->data['MENU'], get_class($this), "POSTCREATE");
    if ($command == "" && isset($this->config->data['TABS'])){
      $command= search_config($this->config->data['TABS'], get_class($this), "POSTCREATE");
    }

    if ($command != ""){
      /* Walk through attribute list */
      foreach ($this->attributes as $attr){
        if (!is_array($this->$attr)){
          $command= preg_replace("/%$attr/", $this->$attr, $command);
        }
      }
      $command= preg_replace("/%dn/", $this->dn, $command);

      /* Additional attributes */
      foreach ($add_attrs as $name => $value){
        $command= preg_replace("/%$name/", $value, $command);
      }

      if (check_command($command)){
        @DEBUG (DEBUG_SHELL, __LINE__, __FUNCTION__, __FILE__,
            $command, "Execute");

        exec($command);
      } else {
        $message= sprintf(_("Command '%s', specified as POSTCREATE for plugin '%s' doesn't seem to exist."), $command, get_class($this));
        print_red ($message);
      }
    }
  }

  function postmodify($add_attrs= array())
  {
    /* Find postcreate entries for this class */
    $command= search_config($this->config->data['MENU'], get_class($this), "POSTMODIFY");
    if ($command == "" && isset($this->config->data['TABS'])){
      $command= search_config($this->config->data['TABS'], get_class($this), "POSTMODIFY");
    }

    if ($command != ""){
      /* Walk through attribute list */
      foreach ($this->attributes as $attr){
        if (!is_array($this->$attr)){
          $command= preg_replace("/%$attr/", $this->$attr, $command);
        }
      }
      $command= preg_replace("/%dn/", $this->dn, $command);

      /* Additional attributes */
      foreach ($add_attrs as $name => $value){
        $command= preg_replace("/%$name/", $value, $command);
      }

      if (check_command($command)){
        @DEBUG (DEBUG_SHELL, __LINE__, __FUNCTION__, __FILE__,
            $command, "Execute");

        exec($command);
      } else {
        $message= sprintf(_("Command '%s', specified as POSTMODIFY for plugin '%s' doesn't seem to exist."), $command, get_class($this));
        print_red ($message);
      }
    }
  }

  function postremove($add_attrs= array())
  {
    /* Find postremove entries for this class */
    $command= search_config($this->config->data['MENU'], get_class($this), "POSTREMOVE");
    if ($command == "" && isset($this->config->data['TABS'])){
      $command= search_config($this->config->data['TABS'], get_class($this), "POSTREMOVE");
    }

    if ($command != ""){
      /* Walk through attribute list */
      foreach ($this->attributes as $attr){
        if (!is_array($this->$attr)){
          $command= preg_replace("/%$attr/", $this->$attr, $command);
        }
      }
      $command= preg_replace("/%dn/", $this->dn, $command);

      /* Additional attributes */
      foreach ($add_attrs as $name => $value){
        $command= preg_replace("/%$name/", $value, $command);
      }

      if (check_command($command)){
        @DEBUG (DEBUG_SHELL, __LINE__, __FUNCTION__, __FILE__,
            $command, "Execute");

        exec($command);
      } else {
        $message= sprintf(_("Command '%s', specified as POSTREMOVE for plugin '%s' doesn't seem to exist."), $command, get_class($this));
        print_red ($message);
      }
    }
  }

  /* Create unique DN */
  function create_unique_dn($attribute, $base)
  {
    $ldap= $this->config->get_ldap_link();
    $base= preg_replace("/^,*/", "", $base);

    /* Try to use plain entry first */
    $dn= "$attribute=".$this->$attribute.",$base";
    $ldap->cat ($dn, array('dn'));
    if (!$ldap->fetch()){
      return ($dn);
    }

    /* Look for additional attributes */
    foreach ($this->attributes as $attr){
      if ($attr == $attribute || $this->$attr == ""){
        continue;
      }

      $dn= "$attribute=".$this->$attribute."+$attr=".$this->$attr.",$base";
      $ldap->cat ($dn, array('dn'));
      if (!$ldap->fetch()){
        return ($dn);
      }
    }

    /* None found */
    return ("none");
  }

  function rebind($ldap, $referral)
  {
    $credentials= LDAP::get_credentials($referral, $this->config->current['REFERRAL']);
    if (ldap_bind($ldap, $credentials['ADMIN'], $credentials['PASSWORD'])) {
      $this->error = "Success";
      $this->hascon=true;
      $this->reconnect= true;
      return (0);
    } else {
      $this->error = "Could not bind to " . $credentials['ADMIN'];
      return NULL;
    }
  }

  /* This is a workaround function. */
  function copy($src_dn, $dst_dn)
  {
    /* Rename dn in possible object groups */
    $ldap= $this->config->get_ldap_link();
    $ldap->search('(&(objectClass=gosaGroupOfNames)(member='.@LDAP::fix($src_dn).'))',
        array('cn'));
    while ($attrs= $ldap->fetch()){
      $og= new ogroup($this->config, $ldap->getDN());
      unset($og->member[$src_dn]);
      $og->member[$dst_dn]= $dst_dn;
      $og->save ();
    }

    $ldap->cat($dst_dn);
    $attrs= $ldap->fetch();
    if (count($attrs)){
      trigger_error("Trying to overwrite ".@LDAP::fix($dst_dn).", which already exists.",
          E_USER_WARNING);
      return (FALSE);
    }

    $ldap->cat($src_dn);
    $attrs= $ldap->fetch();
    if (!count($attrs)){
      trigger_error("Trying to move ".@LDAP::fix($src_dn).", which does not seem to exist.",
          E_USER_WARNING);
      return (FALSE);
    }

    /* Grummble. This really sucks. PHP ldap doesn't support rdn stuff. */
    $ds= ldap_connect($this->config->current['SERVER']);
    ldap_set_option($ds, LDAP_OPT_PROTOCOL_VERSION, 3);
    if (function_exists("ldap_set_rebind_proc") && isset($this->config->current['REFERRAL'])) {
      ldap_set_rebind_proc($ds, array(&$this, "rebind"));
    }

    $r=ldap_bind($ds,$this->config->current['ADMIN'], $this->config->current['PASSWORD']);
    error_reporting (0);
    $sr=ldap_read($ds, @LDAP::fix($src_dn), "objectClass=*");

    /* Fill data from LDAP */
    $new= array();
    if ($sr) {
      $ei=ldap_first_entry($ds, $sr);
      if ($ei) {
        foreach($attrs as $attr => $val){
          if ($info = ldap_get_values_len($ds, $ei, $attr)){
            for ($i= 0; $i<$info['count']; $i++){
              if ($info['count'] == 1){
                $new[$attr]= $info[$i];
              } else {
                $new[$attr][]= $info[$i];
              }
            }
          }
        }
      }
    }

    /* close conncetion */
    error_reporting (E_ALL);
    ldap_unbind($ds);

    /* Adapt naming attribute */
    $dst_name= preg_replace("/^([^=]+)=.*$/", "\\1", $dst_dn);
    $dst_val = preg_replace("/^[^=]+=([^,+]+).*,.*$/", "\\1", $dst_dn);
    $new[$dst_name]= @LDAP::fix($dst_val);

    /* Check if this is a department.
     * If it is a dep. && there is a , override in his ou 
     *  change \2C to , again, else this entry can't be saved ...
     */
    if((isset($new['ou'])) &&( preg_match("/\\,/",$new['ou']))){
      $new['ou'] = preg_replace("/\\\\,/",",",$new['ou']);
    }

    /* Save copy */
    $ldap->connect();
    $ldap->cd($this->config->current['BASE']);
    
    $ldap->create_missing_trees(preg_replace('/^[^,]+,/', '', $dst_dn));

    /* FAIvariable=.../..., cn=.. 
        could not be saved, because the attribute FAIvariable was different to 
        the dn FAIvariable=..., cn=... */
    if(in_array_ics("FAIdebconfInfo",$new['objectClass'])){
      $new['FAIvariable'] = $ldap->fix($new['FAIvariable']);
    }
    $ldap->cd($dst_dn);
    $ldap->add($new);

    if ($ldap->error != "Success"){
      trigger_error("Trying to save $dst_dn failed.",
          E_USER_WARNING);
      return(FALSE);
    }

    return (TRUE);
  }


  function move($src_dn, $dst_dn)
  {
    /* Copy source to destination */
    if (!$this->copy($src_dn, $dst_dn)){
      return (FALSE);
    }

    /* Delete source */
    $ldap= $this->config->get_ldap_link();
    $ldap->rmdir($src_dn);
    if ($ldap->error != "Success"){
      trigger_error("Trying to delete $src_dn failed.",
          E_USER_WARNING);
      return (FALSE);
    }

    return (TRUE);
  }


  /* Move/Rename complete trees */
  function recursive_move($src_dn, $dst_dn)
  {
    /* Check if the destination entry exists */
    $ldap= $this->config->get_ldap_link();

    /* Check if destination exists - abort */
    $ldap->cat($dst_dn, array('dn'));
    if ($ldap->fetch()){
      trigger_error("recursive_move $dst_dn already exists.",
          E_USER_WARNING);
      return (FALSE);
    }

    /* Perform a search for all objects to be moved */
    $objects= array();
    $ldap->cd($src_dn);
    $ldap->search("(objectClass=*)", array("dn"));
    while($attrs= $ldap->fetch()){
      $dn= $attrs['dn'];
      $objects[$dn]= strlen($dn);
    }

    /* Sort objects by indent level */
    asort($objects);
    reset($objects);

    /* Copy objects from small to big indent levels by replacing src_dn by dst_dn */
    foreach ($objects as $object => $len){
      $src= $object;
      $dst= preg_replace("/$src_dn$/", "$dst_dn", $object);
      if (!$this->copy($src, $dst)){
        return (FALSE);
      }
    }

    /* Remove src_dn */
    $ldap->cd($src_dn);
    $ldap->recursive_remove();
    return (TRUE);
  }


  function handle_post_events($mode, $add_attrs= array())
  {
    switch ($mode){
      case "add":
        $this->postcreate($add_attrs);
      break;

      case "modify":
        $this->postmodify($add_attrs);
      break;

      case "remove":
        $this->postremove($add_attrs);
      break;
    }
  }


  function saveCopyDialog(){
  }


  function getCopyDialog(){
    return(array("string"=>"","status"=>""));
  }


  function PrepareForCopyPaste($source){
    $todo = $this->attributes;
    if(isset($this->CopyPasteVars)){
      $todo = array_merge($todo,$this->CopyPasteVars);
    }
    $todo[] = "is_account";
    foreach($todo as $var){
      $this->$var = $source->$var;
    }
  }


  function handle_object_tagging($dn= "", $tag= "", $show= false)
  {
    //FIXME: How to optimize this? We have at least two
    //       LDAP accesses per object. It would be a good
    //       idea to have it integrated.

    /* No dn? Self-operation... */
    if ($dn == ""){
      $dn= $this->dn;

      /* No tag? Find it yourself... */
      if ($tag == ""){
        $len= strlen($dn);

        @DEBUG (DEBUG_TRACE, __LINE__, __FUNCTION__, __FILE__, "No tag for $dn - looking for one...", "Tagging");
        $relevant= array();
        foreach ($this->config->adepartments as $key => $ntag){

          /* This one is bigger than our dn, its not relevant... */
          if ($len <= strlen($key)){
            continue;
          }

          /* This one matches with the latter part. Break and don't fix this entry */
          if (preg_match('/(^|,)'.normalizePreg($key).'$/', $dn)){
            @DEBUG (DEBUG_TRACE, __LINE__, __FUNCTION__, __FILE__, "DEBUG: Possibly relevant: $key", "Tagging");
            $relevant[strlen($key)]= $ntag;
            continue;
          }

        }

        /* If we've some relevant tags to set, just get the longest one */
        if (count($relevant)){
          ksort($relevant);
          $tmp= array_keys($relevant);
          $idx= end($tmp);
          $tag= $relevant[$idx];
          $this->gosaUnitTag= $tag;
        }
      }
    }


    /* Set tag? */
    if ($tag != ""){
      /* Set objectclass and attribute */
      $ldap= $this->config->get_ldap_link();
      $ldap->cat($dn, array('gosaUnitTag', 'objectClass'));
      $attrs= $ldap->fetch();
      if(isset($attrs['gosaUnitTag'][0]) && $attrs['gosaUnitTag'][0] == $tag){
        if ($show) {
          echo sprintf(_("Object '%s' is already tagged"), @LDAP::fix($dn))."<br>";
          flush();
        }
        return;
      }
      if (count($attrs)){
        if ($show){
          echo sprintf(_("Adding tag (%s) to object '%s'"), $tag, @LDAP::fix($dn))."<br>";
          flush();
        }
        $nattrs= array("gosaUnitTag" => $tag);
        $nattrs['objectClass']= array();
        for ($i= 0; $i<$attrs['objectClass']['count']; $i++){
          $oc= $attrs['objectClass'][$i];
          if ($oc != "gosaAdministrativeUnitTag"){
            $nattrs['objectClass'][]= $oc;
          }
        }
        $nattrs['objectClass'][]= "gosaAdministrativeUnitTag";
        $ldap->cd($dn);
        $ldap->modify($nattrs);
        show_ldap_error($ldap->get_error(), sprintf(_("Handle object tagging with dn '%s' failed."),$dn));
      } else {
        @DEBUG (DEBUG_TRACE, __LINE__, __FUNCTION__, __FILE__, "Not tagging ($tag) $dn - seems to have moved away", "Tagging");
      }

    } else {
      /* Remove objectclass and attribute */
      $ldap= $this->config->get_ldap_link();
      $ldap->cat($dn, array('gosaUnitTag', 'objectClass'));
      $attrs= $ldap->fetch();
      if (isset($attrs['objectClass']) && !in_array_ics("gosaAdministrativeUnitTag", $attrs['objectClass'])){
        @DEBUG (DEBUG_TRACE, __LINE__, __FUNCTION__, __FILE__, "$dn is not tagged", "Tagging");
        return;
      }
      if (count($attrs)){
        if ($show){
          echo sprintf(_("Removing tag from object '%s'"), @LDAP::fix($dn))."<br>";
          flush();
        }
        $nattrs= array("gosaUnitTag" => array());
        $nattrs['objectClass']= array();
        for ($i= 0; $i<$attrs['objectClass']['count']; $i++){
          $oc= $attrs['objectClass'][$i];
          if ($oc != "gosaAdministrativeUnitTag"){
            $nattrs['objectClass'][]= $oc;
          }
        }
        $ldap->cd($dn);
        $ldap->modify($nattrs);
        show_ldap_error($ldap->get_error(), sprintf(_("Handle object tagging with dn '%s' failed."),$dn));
      } else {
        @DEBUG (DEBUG_TRACE, __LINE__, __FUNCTION__, __FILE__, "Not removing tag ($tag) $dn - seems to have moved away", "Tagging");
      }
    }

  }


  /* Add possibility to stop remove process */
  function allow_remove()
  {
    $reason= "";
    return $reason;
  }


  /* Create a snapshot of the current object */
  function create_snapshot($type= "snapshot", $description= array())
  {

    /* Check if snapshot functionality is enabled */
    if(!$this->snapshotEnabled()){
      return;
    }

    /* Get configuration from gosa.conf */
    $tmp = $this->config->current;

    /* Create lokal ldap connection */
    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);

    /* check if there are special server configurations for snapshots */
    if(!isset($tmp['SNAPSHOT_SERVER'])){

      /* Source and destination server are both the same, just copy source to dest obj */
      $ldap_to      = $ldap;
      $snapldapbase = $this->config->current['BASE'];

    }else{
      $server         = $tmp['SNAPSHOT_SERVER'];
      $user           = $tmp['SNAPSHOT_USER'];
      $password       = $tmp['SNAPSHOT_PASSWORD'];
      $snapldapbase   = $tmp['SNAPSHOT_LDAP_BASE'];

      $ldap_to        = new LDAP($user,$password, $server);
      $ldap_to -> cd($snapldapbase);
      show_ldap_error($ldap->get_error(), sprintf(_("Saving object snapshot with dn '%s' failed."),$snapldapbase));
    }

    /* check if the dn exists */ 
    if ($ldap->dn_exists($this->dn)){

      /* Extract seconds & mysecs, they are used as entry index */
      list($usec, $sec)= explode(" ", microtime());

      /* Collect some infos */
      $base           = $this->config->current['BASE'];
      $snap_base      = $tmp['SNAPSHOT_BASE'];
      $base_of_object = preg_replace ('/^[^,]+,/i', '', $this->dn);
      $new_base       = preg_replace("/".normalizePreg($base)."$/","",$base_of_object).$snap_base;

      /* Create object */
#$data             = preg_replace('/^dn:.*\n/', '', $ldap->gen_ldif($this->dn,"(!(objectClass=gosaDepartment))"));
      $data             = $ldap->gen_ldif($this->dn,"(&(!(objectClass=gosaDepartment))(!(objectClass=FAIclass)))");
      $newName          = preg_replace("/\./", "", $sec."-".$usec);
      $target= array();
      $target['objectClass']            = array("top", "gosaSnapshotObject");
      $target['gosaSnapshotData']       = gzcompress($data, 6);
      $target['gosaSnapshotType']       = $type;
      $target['gosaSnapshotDN']         = $this->dn;
      $target['description']            = $description;
      $target['gosaSnapshotTimestamp']  = $newName;

      /* Insert the new snapshot 
         But we have to check first, if the given gosaSnapshotTimestamp
         is already used, in this case we should increment this value till there is 
         an unused value. */ 
      $new_dn                           = "gosaSnapshotTimestamp=".$newName.",".$new_base;
      $ldap_to->cat($new_dn);
      while($ldap_to->count()){
        $ldap_to->cat($new_dn);
        $newName = preg_replace("/\./", "", $sec."-".($usec++));
        $new_dn                           = "gosaSnapshotTimestamp=".$newName.",".$new_base;
        $target['gosaSnapshotTimestamp']  = $newName;
      } 

      /* Inset this new snapshot */
      $ldap_to->cd($snapldapbase);
      $ldap_to->create_missing_trees($new_base);
      $ldap_to->cd($new_dn);
      $ldap_to->add($target);

      show_ldap_error($ldap->get_error(), sprintf(_("Saving object snapshot with dn '%s' failed."),$new_base));
      show_ldap_error($ldap_to->get_error(), sprintf(_("Saving object snapshot with dn '%s' failed."),$new_base));
    }
  }

  function remove_snapshot($dn)
  {
echo "FIXME: remove_snapshot uses old acl's<br>";
    $ui   = get_userinfo();
    $acl  = get_permissions ($dn, $ui->subtreeACL);
    $acl  = get_module_permission($acl, "snapshot", $dn);

    if (chkacl($this->acl, "delete") == ""){
      $ldap = $this->config->get_ldap_link();
      $ldap->cd($this->config->current['BASE']);
      $ldap->rmdir_recursive($dn);
    }else{
      print_red (_("You are not allowed to delete this snapshot!"));
    }
  }


  /* returns true if snapshots are enabled, and false if it is disalbed
     There will also be some errors psoted, if the configuration failed */
  function snapshotEnabled()
  {
    $tmp = $this->config->current;
    if(isset($tmp['ENABLE_SNAPSHOT'])){
      if (preg_match("/^true$/i", $tmp['ENABLE_SNAPSHOT']) || preg_match("/yes/i", $tmp['ENABLE_SNAPSHOT'])){

        /* Check if the snapshot_base is defined */
        if(!isset($tmp['SNAPSHOT_BASE'])){
          print_red(sprintf(_("The snapshot functionality is enabled, but the required variable '%s' is not configured in your gosa.conf."),$missing));
          return(FALSE);
        }

        /* check if there are special server configurations for snapshots */
        if(isset($tmp['SNAPSHOT_SERVER'])){

          /* check if all required vars are available to create a new ldap connection */
          $missing = "";
          foreach(array("SNAPSHOT_SERVER","SNAPSHOT_USER","SNAPSHOT_PASSWORD","SNAPSHOT_LDAP_BASE") as $var){
            if(!isset($tmp[$var])){
              $missing .= $var." ";
              print_red(sprintf(_("The snapshot functionality is enabled, but the required variable(s) '%s' is not configured in your gosa.conf."),$missing));
              return(FALSE);
            }
          }
        }
        return(TRUE);
      }
    }
    return(FALSE);
  }


  /* Return available snapshots for the given base 
   */
  function Available_SnapsShots($dn,$raw = false)
  {
    if(!$this->snapshotEnabled()) return(array());

    /* Create an additional ldap object which
       points to our ldap snapshot server */
    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    $tmp = $this->config->current;

    /* check if there are special server configurations for snapshots */
    if(isset($tmp['SNAPSHOT_SERVER'])){
      $server       = $tmp['SNAPSHOT_SERVER'];
      $user         = $tmp['SNAPSHOT_USER'];
      $password     = $tmp['SNAPSHOT_PASSWORD'];
      $snapldapbase = $tmp['SNAPSHOT_LDAP_BASE'];
      $ldap_to      = new LDAP($user,$password, $server);
      $ldap_to -> cd ($snapldapbase);
      show_ldap_error($ldap->get_error(), sprintf(_("Method get available snapshots with dn '%s' failed."),$this->dn));
    }else{
      $ldap_to    = $ldap;
    }

    /* Prepare bases and some other infos */
    $base           = $this->config->current['BASE'];
    $snap_base      = $tmp['SNAPSHOT_BASE'];
    $base_of_object = preg_replace ('/^[^,]+,/i', '', $dn);
    $new_base       = preg_replace("/".normalizePreg($base)."$/","",$base_of_object).$snap_base;
    $tmp            = array(); 

    /* Fetch all objects with  gosaSnapshotDN=$dn */
    $ldap_to->cd($new_base);
    $ldap_to->ls("(&(objectClass=gosaSnapshotObject)(gosaSnapshotDN=".$dn."))",$new_base,
        array("gosaSnapshotType","gosaSnapshotTimestamp","gosaSnapshotDN","description")); 

    /* Put results into a list and add description if missing */
    while($entry = $ldap_to->fetch()){ 
      if(!isset($entry['description'][0])){
        $entry['description'][0]  = "";
      }
      $tmp[] = $entry; 
    }

    /* Return the raw array, or format the result */
    if($raw){
      return($tmp);
    }else{  
      $tmp2 = array();
      foreach($tmp as $entry){
        $tmp2[base64_encode($entry['dn'])] = $entry['description'][0]; 
      }
    }
    return($tmp2);
  }


  function getAllDeletedSnapshots($base_of_object,$raw = false)
  {
    if(!$this->snapshotEnabled()) return(array());

    /* Create an additional ldap object which
       points to our ldap snapshot server */
    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    $tmp = $this->config->current;

    /* check if there are special server configurations for snapshots */
    if(isset($tmp['SNAPSHOT_SERVER'])){
      $server       = $tmp['SNAPSHOT_SERVER'];
      $user         = $tmp['SNAPSHOT_USER'];
      $password     = $tmp['SNAPSHOT_PASSWORD'];
      $snapldapbase = $tmp['SNAPSHOT_LDAP_BASE'];
      $ldap_to      = new LDAP($user,$password, $server);
      $ldap_to->cd ($snapldapbase);
      show_ldap_error($ldap->get_error(), sprintf(_("Method get deleted snapshots with dn '%s' failed."),$this->dn));
    }else{
      $ldap_to    = $ldap;
    }

    /* Prepare bases */ 
    $base           = $this->config->current['BASE'];
    $snap_base      = $tmp['SNAPSHOT_BASE'];
    $new_base       = preg_replace("/".normalizePreg($base)."$/","",$base_of_object).$snap_base;

    /* Fetch all objects and check if they do not exist anymore */
    $ui = get_userinfo();
    $tmp = array();
    $ldap_to->cd($new_base);
    $ldap_to->ls("(objectClass=gosaSnapshotObject)",$new_base,array("gosaSnapshotType","gosaSnapshotTimestamp","gosaSnapshotDN","description"));
    while($entry = $ldap_to->fetch()){

      $chk =  str_replace($new_base,"",$entry['dn']);
      if(preg_match("/,ou=/",$chk)) continue;

      if(!isset($entry['description'][0])){
        $entry['description'][0]  = "";
      }
      $tmp[] = $entry; 
    }

    /* Check if entry still exists */
    foreach($tmp as $key => $entry){
      $ldap->cat($entry['gosaSnapshotDN'][0]);
      if($ldap->count()){
        unset($tmp[$key]);
      }
    }

    /* Format result as requested */
    if($raw) {
      return($tmp);
    }else{
      $tmp2 = array();
      foreach($tmp as $key => $entry){
        $tmp2[base64_encode($entry['dn'])] = $entry['description'][0]; 
      }
    }
    return($tmp2);
  } 


  /* Restore selected snapshot */
  function restore_snapshot($dn)
  {
    if(!$this->snapshotEnabled()) return(array());

    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    $tmp = $this->config->current;

    /* check if there are special server configurations for snapshots */
    if(isset($tmp['SNAPSHOT_SERVER'])){
      $server       = $tmp['SNAPSHOT_SERVER'];
      $user         = $tmp['SNAPSHOT_USER'];
      $password     = $tmp['SNAPSHOT_PASSWORD'];
      $snapldapbase = $tmp['SNAPSHOT_LDAP_BASE'];
      $ldap_to      = new LDAP($user,$password, $server);
      $ldap_to->cd ($snapldapbase);
      show_ldap_error($ldap->get_error(), sprintf(_("Restore snapshot with dn '%s' failed."),$snapldapbase));
    }else{
      $ldap_to    = $ldap;
    }

    /* Get the snapshot */ 
    $ldap_to->cat($dn);
    $restoreObject = $ldap_to->fetch();

    /* Prepare import string */
    $data  = gzuncompress($ldap_to->get_attribute($dn,'gosaSnapshotData'));

    /* Import the given data */
    $ldap->import_complete_ldif($data,$err,false,false);
    show_ldap_error($ldap->get_error(), sprintf(_("Restore snapshot with dn '%s' failed."),$dn));
  }


  function showSnapshotDialog($base,$baseSuffixe)
  {
    $once = true;
    foreach($_POST as $name => $value){

      /* Create a new snapshot, display a dialog */
      if(preg_match("/^CreateSnapShotDialog_/",$name) && $once){
        $once = false;
        $entry = preg_replace("/^CreateSnapShotDialog_/","",$name);
        $entry = base64_decode(preg_replace("/_[xy]$/","",$entry));
        $this->snapDialog = new SnapShotDialog($this->config,$entry,$this);
      }

      /* Restore a snapshot, display a dialog with all snapshots of the current object */
      if(preg_match("/^RestoreSnapShotDialog_/",$name) && $once){
        $once = false;
        $entry = preg_replace("/^RestoreSnapShotDialog_/","",$name);
        $entry = base64_decode(preg_replace("/_[xy]$/","",$entry));
        $this->snapDialog = new SnapShotDialog($this->config,$entry,$this);
        $this->snapDialog->display_restore_dialog = true;
      }

      /* Restore one of the already deleted objects */
      if(preg_match("/^RestoreDeletedSnapShot_/",$name) && $once){
        $once = false;
        $this->snapDialog = new SnapShotDialog($this->config,$baseSuffixe,$this);
        $this->snapDialog->display_restore_dialog      = true;
        $this->snapDialog->display_all_removed_objects  = true;
      }

      /* Restore selected snapshot */
      if(preg_match("/^RestoreSnapShot_/",$name) && $once){
        $once = false;
        $entry = preg_replace("/^RestoreSnapShot_/","",$name);
        $entry = base64_decode(trim(preg_replace("/_[xy]$/","",$entry)));
        if(!empty($entry)){
          $this->restore_snapshot($entry);
          $this->snapDialog = NULL;
        }
      }
    }

    /* Create a new snapshot requested, check
       the given attributes and create the snapshot*/
    if(isset($_POST['CreateSnapshot'])){
      $this->snapDialog->save_object();
      $msgs = $this->snapDialog->check();
      if(count($msgs)){
        foreach($msgs as $msg){
          print_red($msg);
        }
      }else{
        $this->dn =  $this->snapDialog->dn;
        $this->create_snapshot("snapshot",$this->snapDialog->CurrentDescription);
        $this->snapDialog = NULL;
      }
    }

    /* Restore is requested, restore the object with the posted dn .*/
    if((isset($_POST['RestoreSnapshot'])) && (isset($_POST['SnapShot']))){
    }

    if(isset($_POST['CancelSnapshot'])){
      $this->snapDialog = NULL;
    }

    if($this->snapDialog){
      $this->snapDialog->save_object();
      return($this->snapDialog->execute());
    }
  }


  function plInfo()
  {
    return array();
  }


  function set_acl_base($base)
  {
    $this->acl_base= $base;
  }


  function set_acl_category($category)
  {
    $this->acl_category= "$category/";
  }


  function acl_is_writeable($attribute,$skip_write = FALSE)
  {
    $ui= get_userinfo();
    return preg_match('/w/', $ui->get_permissions($this->acl_base, $this->acl_category.get_class($this), $attribute, $skip_write));
  }


  function acl_is_readable($attribute)
  {
    $ui= get_userinfo();
    return preg_match('/r/', $ui->get_permissions($this->acl_base, $this->acl_category.get_class($this), $attribute));
  }


  function acl_is_createable()
  {
    $ui= get_userinfo();
    return preg_match('/c/', $ui->get_permissions($this->acl_base, $this->acl_category.get_class($this), '0'));
  }


  function acl_is_removeable()
  {
    $ui= get_userinfo();
    return preg_match('/d/', $ui->get_permissions($this->acl_base, $this->acl_category.get_class($this), '0'));
  }


  function acl_is_moveable()
  {
    $ui= get_userinfo();
    return preg_match('/m/', $ui->get_permissions($this->acl_base, $this->acl_category.get_class($this), '0'));
  }


  function acl_have_any_permissions()
  {
  }


  function getacl($attribute,$skip_write= FALSE)
  {
    $ui= get_userinfo();
    return  $ui->get_permissions($this->acl_base, $this->acl_category.get_class($this), $attribute,$skip_write);
  }

  /* Get all allowed bases to move an object to or to create a new object.
     Idepartments also contains all base departments which lead to the allowed bases */
  function acl_get_bases($category = "")
  {
    $ui = get_userinfo();
    $deps = array();
 
    /* Set category */ 
    if(empty($category)){
      $category = $this->acl_category.get_class($this);
    }

    /* Is this a new object ? Or just an edited existing object */
    if(!$this->initially_was_account && $this->is_account){
      $new = true;
    }else{
      $new = false;
    }

    /* Add current base */      
    if(isset($this->base) && isset($this->config->idepartments[$this->base])){
      $deps[$this->base] = $this->config->idepartments[$this->base];
    }else{
      echo "No default base found. <br>";
    }

    foreach($this->config->idepartments as $dn => $name){
      $acl = $ui->get_permissions($dn,$category);
      if($new && preg_match("/c/",$acl)){
        $deps[$dn] = $name;
      }elseif(!$new && preg_match("/m/",$acl)){
        $deps[$dn] = $name;
      }
    }
    return($deps);
  }

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
