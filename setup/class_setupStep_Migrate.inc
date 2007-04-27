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




class Step_Migrate extends setup_step
{
  var $languages      = array();
  var $attributes     = array();
  var $header_image   = "images/monitoring.png";
  var $checks         = array();

  /* Department migration attributes */
  var $dep_migration_dialog = FALSE;
  var $deps_to_migrate      = array();

  /* Department migration attributes */
  var $users_migration_dialog= FALSE;
  var $users_to_migrate      = array();

  /* Create Acl attributes */
  var $acl_create_dialog  = FALSE;
  var $acl_create_type    = "group";
  var $acl_create_selected= ""; // Currently selected element, that should receive admin rights 
  var $acl_create_changes = ""; // Contains ldif information about changes 
  var $acl_create_confirmed= FALSE;

  /* Checks initialised ? */
  var $checks_initialised = FALSE;

  function Step_Migrate()
  {
    $this->update_strings(); 
  }

  function update_strings()
  {
    $this->s_title      = _("LDAP inspection");
    $this->s_title_long = _("LDAP inspection");
    $this->s_info       = _("Analyze your current LDAP for GOsa compatibility");
  }

  function initialize_checks()
  {
    $this->checks = array();
    $this->checks['root']['TITLE']     = _("Checking for root object");
    $this->checks['root']['STATUS']    = FALSE;
    $this->checks['root']['STATUS_MSG']= "";
    $this->checks['root']['ERROR_MSG'] = "";
    $this->checkBase();

    $this->checks['permissions']['TITLE']     = _("Checking permissions on ldap database");
    $this->checks['permissions']['STATUS']    = FALSE;
    $this->checks['permissions']['STATUS_MSG']= "";
    $this->checks['permissions']['ERROR_MSG'] = "";
    $this->check_ldap_permissions();

    $this->checks['deps_visible']['TITLE']     = _("Checking for invisible deparmtments");
    $this->checks['deps_visible']['STATUS']    = FALSE;
    $this->checks['deps_visible']['STATUS_MSG']= "";
    $this->checks['deps_visible']['ERROR_MSG'] = "";
    $this->check_visible_organizationalUnits();

    $this->checks['users_visible']['TITLE']     = _("Checking for invisible user");
    $this->checks['users_visible']['STATUS']    = FALSE;
    $this->checks['users_visible']['STATUS_MSG']= "";
    $this->checks['users_visible']['ERROR_MSG'] = "";
    $this->check_invisible_gosaAccounts();

    $this->checks['acls']['TITLE']     = _("Checking for administrational account");
    $this->checks['acls']['STATUS']    = FALSE;
    $this->checks['acls']['STATUS_MSG']= "";
    $this->checks['acls']['ERROR_MSG'] = "";
    $this->check_acls();
  }



  /* Check ldap accessibility 
   * Create and remove a dummy object, 
   *  to ensure that we have the necessary permissions
   */
  function check_ldap_permissions()
  {
    $cv = $this->parent->captured_values;
    $ldap = new LDAP($cv['admin'],
        $cv['password'],
        $cv['connection'],
        FALSE,
        $cv['tls']);

    /* Create dummy entry 
     */
    $name     = "GOsa_setup_text_entry_".session_id().rand(0,999999);
    $dn       = "ou=".$name.",".$cv['base'];
    $testEntry= array();
    $testEntry['objectClass'][]= "top";
    $testEntry['objectClass'][]= "organizationalUnit";
    $testEntry['objectClass'][]= "gosaDepartment";
    $testEntry['description']= "Created by GOsa setup, this object can be removed.";
    $testEntry['ou']  = $name;

    /* check if simple ldap cat will be successful 
     */
    $res = $ldap->cat($cv['base']);  
    if(!$res){
      $this->checks['permissions']['STATUS']    = FALSE;
      $this->checks['permissions']['STATUS_MSG']= _("Ldap query failed.");
      $this->checks['permissions']['ERROR_MSG'] = _("Possibly the 'root object' is missing.");
      return(false);
    }
  
    /* Try to create dummy object 
     */ 
    $ldap->cd ($dn);
    $ldap->create_missing_trees($dn);
    $res = $ldap->add($testEntry);
    if(!$res){
      gosa_log($ldap->get_error());
      $this->checks['permissions']['STATUS']    = FALSE;
      $this->checks['permissions']['STATUS_MSG']= _("Failed");
      $this->checks['permissions']['ERROR_MSG'] = 
        sprintf(_("The specified user '%s' does not have full access to your ldap database."),$cv['admin']);
      return(false);
    }

    /* Try to remove created entry 
     */
    $res = $ldap->rmDir($dn);
    if(!$res){
      gosa_log($ldap->get_error());
      $this->checks['permissions']['STATUS']    = FALSE;
      $this->checks['permissions']['STATUS_MSG']= _("Failed");
      $this->checks['permissions']['ERROR_MSG'] = 
        sprintf(_("The specified user '%s' does not have full access to your ldap database."),$cv['admin']);
      return(false);
    }

    /* Create & remove of dummy object was successful */
    $this->checks['permissions']['STATUS']    = TRUE;
    $this->checks['permissions']['STATUS_MSG']= _("Ok");
    $this->checks['permissions']['ERROR_MSG'] = "";
    return(true);
  } 


  /* Check if there are users which will 
   *  be invisible for GOsa 
   */
  function check_invisible_gosaAccounts()
  {
    /* Remember old list of ivisible users, to be able to set 
     *  the 'html checked' status for the checkboxes again 
     */
    $cnt_ok = 0;
    $old    = $this->users_to_migrate;
    $this->users_to_migrate = array();

    /* Get collected configuration settings */
    $cv = $this->parent->captured_values;

    /* Establish ldap connection */
    $ldap = new LDAP($cv['admin'],
        $cv['password'],
        $cv['connection'],
        FALSE,
        $cv['tls']);

    /* Get all invisible users 
     */
    $ldap->cd($cv['base']); 
    $res =$ldap->search("(&(|(objectClass=posixAccount)(objectClass=inetOrgPerson)(objectClass=organizationalPerson))(!(objectClass=gosaAccount)))",array("sn","givenName","cn","uid"));
    while($attrs = $ldap->fetch()){
      if(!preg_match("/,dc=addressbook,/",$attrs['dn'])){
        $attrs['checked'] = FALSE;
        $attrs['before']  = "";
        $attrs['after']   = "";

        /* Set objects to selected, that were selected before reload */
        if(isset($old[base64_encode($attrs['dn'])])){
          $attrs['checked'] = $old[base64_encode($attrs['dn'])]['checked'];
        }
        $this->users_to_migrate[base64_encode($attrs['dn'])] = $attrs;
      }
    }

    /* No invisible */
    if(!$res){
      $this->checks['users_visible']['STATUS']    = FALSE;
      $this->checks['users_visible']['STATUS_MSG']= _("Ldap query failed.");
      $this->checks['users_visible']['ERROR_MSG'] = _("Possibly the 'root object' is missing.");
    }elseif(count($this->users_to_migrate) == 0){
      $this->checks['users_visible']['STATUS']    = TRUE;
      $this->checks['users_visible']['STATUS_MSG']= _("Ok");
      $this->checks['users_visible']['ERROR_MSG'] = "";
    }else{
      $this->checks['users_visible']['STATUS']    = FALSE;
      $this->checks['users_visible']['STATUS_MSG']= "";
      $this->checks['users_visible']['ERROR_MSG'] = sprintf(_("Found %s users that will not be visible in GOsa."), 
          count($this->users_to_migrate));
      $this->checks['users_visible']['ERROR_MSG'] .= "<input type='submit' name='users_visible_migrate' value='"._("Migrate")."'>";
    }
  }


  /* Start user account migration 
   */  
  function migrate_gosaAccounts($only_ldif = FALSE)
  {
    /* Get collected configuration settings */
    $cv = $this->parent->captured_values;

    /* Establish ldap connection */
    $ldap = new LDAP($cv['admin'],
        $cv['password'],
        $cv['connection'],
        FALSE,
        $cv['tls']);

    /* Add gosaAccount objectClass to the selected users  
     */
    foreach($this->users_to_migrate as $key => $dep){
      if($dep['checked']){

        /* Get old objectClasses */
        $ldap->cat($dep['dn'],array("objectClass"));
        $attrs      = $ldap->fetch();

        /* Create new objectClass array */
        $new_attrs  = array();
        $new_attrs['objectClass']= array("gosaAccount","inetOrgPerson","organizationalPerson");
        for($i = 0 ; $i < $attrs['objectClass']['count']; $i ++ ){
          if(!in_array_ics($attrs['objectClass'][$i], $new_attrs['objectClass'])){
            $new_attrs['objectClass'][]   = $attrs['objectClass'][$i];
          }
        }

        /* Set info attributes for current object, 
         *  or write changes to the ldap database 
         */
        if($only_ldif){
          $this->users_to_migrate[$key]['before'] = $this->array_to_ldif($attrs);
          $this->users_to_migrate[$key]['after']  = $this->array_to_ldif($new_attrs);
        }else{
          $ldap->cd($attrs['dn']);
          if(!$ldap->modify($new_attrs)){
            print_red(sprintf(_("Failed to migrate the department '%s' into GOsa, error message is as follows '%s'."),$attrs['dn'],$ldap->get_error()));
            return(false);
          }
        }
      }
    }
    return(TRUE);
  }


  /* Check if there are invisible organizational Units 
   */
  function check_visible_organizationalUnits()
  {
    $cnt_ok = 0;
    $old = $this->deps_to_migrate;
    $this->deps_to_migrate = array();

    /* Get collected configuration settings */
    $cv = $this->parent->captured_values;

    /* Establish ldap connection */
    $ldap = new LDAP($cv['admin'],
        $cv['password'],
        $cv['connection'],
        FALSE,
        $cv['tls']);

    /* Skip GOsa internal departments */
    $skip_dns = array("/^ou=people,/","/^ou=groups,/","/(,|)ou=configs,/","/(,|)ou=systems,/",
        "/^ou=apps,/","/^ou=mime,/","/^ou=aclroles,/","/^ou=incoming,/",
        "/ou=snapshots,/","/(,|)dc=addressbook,/","/^(,|)ou=machineaccounts,/",
        "/(,|)ou=winstations,/");


    /* Get all invisible departments */
    $ldap->cd($cv['base']); 
    $res = $ldap->search("(&(objectClass=organizationalUnit)(!(objectClass=gosaDepartment)))",array("ou","description","dn"));
    while($attrs = $ldap->fetch()){
      $attrs['checked'] = FALSE;
      $attrs['before']  = "";
      $attrs['after']   = "";

      /* Set objects to selected, that were selected before reload */
      if(isset($old[base64_encode($attrs['dn'])])){
        $attrs['checked'] = $old[base64_encode($attrs['dn'])]['checked'];
      }
      $this->deps_to_migrate[base64_encode($attrs['dn'])] = $attrs;
    }

    /* Filter returned list of departments and ensure that 
     *  GOsa internal departments will not be listed 
     */
    foreach($this->deps_to_migrate as $key => $attrs){
      $dn = $attrs['dn'];
      $skip = false;
      foreach($skip_dns as $skip_dn){
        if(preg_match($skip_dn,$dn)){
          $skip = true;
        }
      }
      if($skip){
        unset($this->deps_to_migrate[$key]);
      }
    }

    /* If we have no invisible departments found  
     *  tell the user that everything is ok 
     */
    if(!$res){
      $this->checks['deps_visible']['STATUS']    = FALSE;
      $this->checks['deps_visible']['STATUS_MSG']= _("Ldap query failed.");
      $this->checks['deps_visible']['ERROR_MSG'] = _("Possibly the 'root object' is missing.");
    }elseif(count($this->deps_to_migrate) == 0 ){
      $this->checks['deps_visible']['STATUS']    = TRUE;
      $this->checks['deps_visible']['STATUS_MSG']= _("Ok");
      $this->checks['deps_visible']['ERROR_MSG'] = "";
    }else{
      $this->checks['deps_visible']['STATUS']    = FALSE;
      $this->checks['deps_visible']['STATUS_MSG']= "";//sprintf(_("%s entries found"),count($this->deps_to_migrate));
      $this->checks['deps_visible']['ERROR_MSG'] = sprintf(_("Found %s departments that will not be visible in GOsa."),count($this->deps_to_migrate));
      $this->checks['deps_visible']['ERROR_MSG'] .= "<input type='submit' name='deps_visible_migrate' value='"._("Migrate")."'>";
    }
  }



  /* Start deparmtment migration */  
  function migrate_organizationalUnits($only_ldif = FALSE)
  {
    /* Get collected configuration settings */
    $cv = $this->parent->captured_values;

    /* Establish ldap connection */
    $ldap = new LDAP($cv['admin'],
        $cv['password'],
        $cv['connection'],
        FALSE,
        $cv['tls']);

    /* Add gosaDepartment objectClass to each selected entry 
     */
    foreach($this->deps_to_migrate as $key => $dep){
      if($dep['checked']){

        /* Get current objectClasses */
        $ldap->cat($dep['dn'],array("objectClass","description"));
        $attrs      = $ldap->fetch();

        /* Create new objectClass attribute including gosaDepartment*/
        $new_attrs  = array();
        for($i = 0 ; $i < $attrs['objectClass']['count']; $i ++ ){
          $new_attrs['objectClass'][]   = $attrs['objectClass'][$i];
        }
        $new_attrs['objectClass'][] = "gosaDepartment";

        /* Append description it is missing */
        if(!isset($attrs['description'])){
          $new_attrs['description'][] = "GOsa department";
        }

        /* Depending on the parameter >only_diff< we save the changes as ldif
         *  or we write our changes directly to the ldap database
         */
        if($only_ldif){
          $this->deps_to_migrate[$key]['before'] = $this->array_to_ldif($attrs);
          $this->deps_to_migrate[$key]['after']  = $this->array_to_ldif($new_attrs);
        }else{
          $ldap->cd($attrs['dn']);
          if(!$ldap->modify($new_attrs)){
            print_red(sprintf(_("Failed to migrate the department '%s' into GOsa, error message is as follows '%s'."),$attrs['dn'],$ldap->get_error()));
            return(false);
          }
        }
      }
    }
    return(TRUE);
  }


  /* Check Acls if there is at least one object with acls defined 
   */
  function check_acls()
  {
    /* Establish ldap connection */
    $cv = $this->parent->captured_values;
    $ldap = new LDAP($cv['admin'],
        $cv['password'],
        $cv['connection'],
        FALSE,
        $cv['tls']);

    /* Search for gosaAcls */ 
    $ldap->cd($cv['base']);
    $res = $ldap->search("(&(objectClass=gosaAccount)(|(objectClass=posixAccount)".     
                           "(objectClass=inetOrgPerson)(objectClass=organizationalPerson)))");
    if(!$res){
      $this->checks['acls']['STATUS']    = FALSE;
      $this->checks['acls']['STATUS_MSG']= _("Ldap query failed.");
      $this->checks['acls']['ERROR_MSG'] = _("Possibly the 'root object' is missing.");
    }elseif($ldap->count()){
      $this->checks['acls']['STATUS']    = TRUE;
      $this->checks['acls']['STATUS_MSG']= _("Ok");
    }else{
      $this->checks['acls']['STATUS']    = FALSE;
      $this->checks['acls']['STATUS_MSG']= _("Failed");
      $this->checks['acls']['ERROR_MSG'].= "<input type='submit' name='create_acls' value='"._("Create adminitrational account")."'>";
    }
    return($ldap->count()>=1);
  }


  function get_user_list()
  {
    /* Get collected configuration settings */
    $cv = $this->parent->captured_values;

    /* Establish ldap connection */
    $ldap = new LDAP($cv['admin'],
        $cv['password'],
        $cv['connection'],
        FALSE,
        $cv['tls']);
    
    $ldap->cd($cv['base']);
    $ldap->search("(objectClass=gosaAccount)",array("dn"));
  
    $tmp = array();
    while($attrs = $ldap->fetch()){
      $tmp[base64_encode($attrs['dn'])] = @LDAP::fix($attrs['dn']);
    }

    return($tmp);
  }

  function get_group_list()
  {
    /* Get collected configuration settings */
    $cv = $this->parent->captured_values;

    /* Establish ldap connection */
    $ldap = new LDAP($cv['admin'],
        $cv['password'],
        $cv['connection'],
        FALSE,
        $cv['tls']);
    
    $ldap->cd($cv['base']);
    $ldap->search("(objectClass=posixGroup)",array("dn"));
  
    $tmp = array();
    while($attrs = $ldap->fetch()){
      $tmp[base64_encode($attrs['dn'])] = @LDAP::fix($attrs['dn']);
    }

    return($tmp);
  }



  function create_admin($only_ldif = FALSE)
  {
    /* Reset '' */
    $this->acl_create_changes="";

    /* Object that should receive admin acls */
    $dn = $this->acl_create_selected;

    /* Get collected configuration settings */
    $cv = $this->parent->captured_values;

    /* Establish ldap connection */
    $ldap = new LDAP($cv['admin'],
        $cv['password'],
        $cv['connection'],
        FALSE,
        $cv['tls']);

    /* Get current base attributes */
    $ldap->cd($cv['base']);
    $ldap->cat($cv['base'],array("dn","objectClass","gosaAclEntry"));
    $attrs = $ldap->fetch();

    /* Add acls for the selcted user to the base */
    $attrs_new['objectClass'] = array("gosaACL");

    for($i = 0; $i < $attrs['objectClass']['count']; $i ++){
      if(!in_array_ics($attrs['objectClass'][$i],$attrs_new['objectClass'])){
        $attrs_new['objectClass'][] = $attrs['objectClass'][$i];
      }
    }

    $acl = "0:sub:".base64_encode($dn).":all;cmdrw";    
    $attrs_new['gosaAclEntry'][] = $acl;
    if(isset($attrs['gosaAclEntry'])){
      for($i = 0 ; $i < $attrs['gosaAclEntry']['count']; $i ++){
          
        $prio = preg_replace("/[:].*$/","",$attrs['gosaAclEntry'][$i]);
        $rest = preg_replace("/^[^:]/","",$attrs['gosaAclEntry'][$i]);
 
        $data = ($prio+1).$rest;
        $attrs_new['gosaAclEntry'][] = $data;
      }
    }

    if($only_ldif){
      $this->acl_create_changes ="\n".$cv['base']."\n";
      $this->acl_create_changes.=$this->array_to_ldif($attrs)."\n";
      $this->acl_create_changes.="\n".$cv['base']."\n";
      $this->acl_create_changes.=$this->array_to_ldif($attrs_new);
    }else{
   
      $ldap->cd($cv['base']);
      if(!$ldap->modify($attrs_new)){
        print_red(sprintf(_("Adding acls for user '%s' failed, ldap says '%s'."),$dn,$ldap->get_error()));
      }
    }
  }
 
  
  function create_admin_user()
  {
    if(isset($_POST['new_user_password']) && !empty($_POST['new_user_password'])){
      $pwd = $_POST['new_user_password'];
    }else{
      print_red(_("Please specify a valid password for the new GOsa admin user."));
      return(FALSE);
    }
    
    /* Establish ldap connection */
    $cv = $this->parent->captured_values;
    $ldap = new LDAP($cv['admin'],
        $cv['password'],
        $cv['connection'],
        FALSE,
        $cv['tls']);

    /* Get current base attributes */
    $ldap->cd($cv['base']);
  
    if($cv['peopledn'] == "cn"){
      $dn = "cn=System Administrator,".$cv['peopleou'].",".$cv['base'];
    }else{
      $dn = "uid=admin,".$cv['peopleou'].",".$cv['base'];
    }

    $methods = @passwordMethod::get_available_methods_if_not_loaded();
    $p_m = $methods[$cv['encryption']];
    $p_c = new $p_m(array());
    $hash = $p_c->generate_hash($pwd);

    $new_user=array();
    $new_user['objectClass']= array("gosaAccount","organizationalPerson","inetOrgPerson");
    $new_user['givenName']  = "System";
    $new_user['sn']  = "Administrator";
    $new_user['cn']  = "System Administrator";
    $new_user['uid'] = "admin";
    $new_user['userPassword'] = $hash;
    
    $ldap->cd($cv['base']);
    $ldap->create_missing_trees(preg_replace("/^[^,]+,/","",$dn));
    $ldap->cd($dn);  
    $res = $ldap->add($new_user);
    $this->acl_create_selected = $dn;
    $this->create_admin();
    
    if(!$res){
      print_red($ldap->get_error());
    }
  
    $this->acl_create_dialog=FALSE;        
    $this->check_acls();
  }
 

  function execute()
  {
    /* Initialise checks if this is the first call */
    if(!$this->checks_initialised || isset($_POST['reload'])){
      $this->initialize_checks();
      $this->checks_initialised = TRUE;
    }
 
    /*************
     * Root object check  
     *************/
  
    if(isset($_POST['retry_root_create'])){
      $this->checkBase(FALSE);
    }
 
    /*************
     * User Migration handling 
     *************/

    if(isset($_POST['retry_acls'])){
      $this->check_acls();
    }

    if(isset($_POST['create_acls'])){
      $this->acl_create_dialog = TRUE;
      $this->dialog = TRUE;
    }
  
    if(isset($_POST['create_acls_cancel'])){
      $this->acl_create_dialog = FALSE;
      $this->dialog = FALSE;
    }

    if(isset($_POST['create_acls_create_confirmed'])){
      $this->create_admin();
    }

    if(isset($_POST['create_acls_create'])){
      $this->create_admin(TRUE);
    }

    if(isset($_POST['create_admin_user'])){
      $this->create_admin_user();
    }

    if($this->acl_create_dialog){
      $smarty = get_smarty();
      $smarty->assign("new_user_password",@$_POST['new_user_password']);
      $smarty->assign("users" ,$this->get_user_list());
      $smarty->assign("users_cnt" ,count($this->get_user_list()));
      $smarty->assign("groups",$this->get_group_list());
      $smarty->assign("groups_cnt",count($this->get_group_list()));
      $smarty->assign("type"  ,$this->acl_create_type);
      $smarty->assign("method","create_acls");
      $smarty->assign("acl_create_selected",$this->acl_create_selected);
      $smarty->assign("what_will_be_done_now",$this->acl_create_changes);
      return($smarty->fetch(get_template_path("setup_migrate.tpl",TRUE,dirname(__FILE__))));
    }

    /*************
     * User Migration handling 
     *************/

    /* Refresh list of deparments */
    if(isset($_POST['users_visible_migrate_refresh'])){
      $this->check_invisible_gosaAccounts();
    }

    /* Open migration dialog */
    if(isset($_POST['users_visible_migrate'])){
      $this->users_migration_dialog = TRUE;
      $this->dialog =TRUE;
    }

    /* Close migration dialog */
    if(isset($_POST['users_visible_migrate_close'])){
      $this->users_migration_dialog = FALSE;
      $this->dialog =FALSE;
    }

    /* Start migration */
    if(isset($_POST['users_visible_migrate_migrate'])){
      if($this->migrate_gosaAccounts()){
        $this->check_invisible_gosaAccounts();
      }
    }

    /* Start migration */
    if(isset($_POST['users_visible_migrate_whatsdone'])){
      $this->migrate_gosaAccounts(TRUE);
    }

    /* Display migration dialog */
    if($this->users_migration_dialog){
      $smarty = get_smarty();
      $smarty->assign("users_to_migrate",$this->users_to_migrate);
      $smarty->assign("method","migrate_users");
      return($smarty->fetch(get_template_path("setup_migrate.tpl",TRUE,dirname(__FILE__))));
    }


    /*************
     * Department Migration handling 
     *************/

    /* Refresh list of deparments */
    if(isset($_POST['deps_visible_migrate_refresh'])){
      $this->check_visible_organizationalUnits();
    }

    /* Open migration dialog */
    if(isset($_POST['deps_visible_migrate'])){
      $this->dep_migration_dialog = TRUE;
      $this->dialog =TRUE;
    }

    /* Close migration dialog */
    if(isset($_POST['deps_visible_migrate_close'])){
      $this->dep_migration_dialog = FALSE;
      $this->dialog =FALSE;
    }

    /* Start migration */
    if(isset($_POST['deps_visible_migrate_migrate'])){
      if($this->migrate_organizationalUnits()){
        $this->check_visible_organizationalUnits();
      }
    }

    /* Start migration */
    if(isset($_POST['deps_visible_migrate_whatsdone'])){
      $this->migrate_organizationalUnits(TRUE);
    }

    /* Display migration dialog */
    if($this->dep_migration_dialog){
      $smarty = get_smarty();
      $smarty->assign("deps_to_migrate",$this->deps_to_migrate);
      $smarty->assign("method","migrate_deps");
      return($smarty->fetch(get_template_path("setup_migrate.tpl",TRUE,dirname(__FILE__))));
    }

    $smarty = get_smarty();
    $smarty->assign("checks",$this->checks);
    $smarty->assign("method","default");
    return($smarty->fetch(get_template_path("setup_migrate.tpl",TRUE,dirname(__FILE__))));
  }


  function save_object()
  {
    /* Get "create acl" dialog posts */
    if($this->acl_create_dialog){
      if(isset($_POST['create_acls_create'])){
        if(isset($_POST['create_acls_selected'])){
          $this->acl_create_selected = base64_decode($_POST['create_acls_selected']);
        }else{
          $this->acl_create_selected = ""; 
        }
      }

      if(isset($_POST['create_acls_create_abort'])){
        $this->acl_create_selected = "";
      }

      if(isset($_POST['acl_create_type'])){
        $this->acl_create_type = $_POST['acl_create_type'];
      }
    }

    /* Get selected departments */
    if($this->dep_migration_dialog){
      foreach($this->deps_to_migrate as $id => $data){
        if(isset($_POST['migrate_'.$id])){
          $this->deps_to_migrate[$id]['checked'] = TRUE;
        }else{
          $this->deps_to_migrate[$id]['checked'] = FALSE;
        }
      }
    }

    /* Get selected users */
    if($this->users_migration_dialog){
      foreach($this->users_to_migrate as $id => $data){
        if(isset($_POST['migrate_'.$id])){
          $this->users_to_migrate[$id]['checked'] = TRUE;
        }else{
          $this->users_to_migrate[$id]['checked'] = FALSE;
        }
      }
    }
  }


  // checks for valid base entry
  function checkBase($just_check = TRUE)
  {
    /* Get collected setup informations */
    $cv = $this->parent->captured_values;

    /* Establish ldap connection */
    $ldap = new LDAP($cv['admin'],
        $cv['password'],
        $cv['connection'],
        FALSE,
        $cv['tls']);

    /* Check if root object exists */
    $ldap->cd($cv['base']);
    $res = $ldap->search("(objectClass=*)");
    $err = ldap_errno($ldap->cid); 

    if( !$res || 
        $err == 0x20 ||  # LDAP_NO_SUCH_OBJECT
        $err == 0x40) {  # LDAP_NAMING_VIOLATION

      /* Root object doesn't exists 
       */
      if($just_check){
        $this->checks['root']['STATUS']    = FALSE;
        $this->checks['root']['STATUS_MSG']= _("Failed");
        $this->checks['root']['ERROR_MSG'].=  "<input type='submit' name='retry_root_create' value='"._("Try to create root object")."'>";
        return(FALSE);
      }else{

        /* Add root object */ 
        $ldap->cd($cv['base']);
        $res = $ldap->create_missing_trees($cv['base']);
  
        /* If adding failed, tell the user */
        if(!$res){
          $this->checks['root']['STATUS']    = FALSE;
          $this->checks['root']['STATUS_MSG']= _("Failed");
          $this->checks['root']['ERROR_MSG'] = _("Root object couldn't be created, you should try it on your own.");
          $this->checks['root']['ERROR_MSG'].= "<input type='submit' name='retry_root_create' value='"._("Try to create root object")."'>";
          return($res);;
        }
      }
    }

    /* Create & remove of dummy object was successful */
    $this->checks['root']['STATUS']    = TRUE;
    $this->checks['root']['STATUS_MSG']= _("Ok");
  }


  /* Return ldif information for a 
   * given attribute array 
   */
  function array_to_ldif($atts)
  {
    $ret = "";
    unset($atts['count']);
    unset($atts['dn']);
    foreach($atts as $name => $value){
      if(is_numeric($name)) {
        continue;
      }
      if(is_array($value)){
        unset($value['count']);
        foreach($value as $a_val){
          $ret .= $name.": ". $a_val."\n";
        }
      }else{
        $ret .= $name.": ". $value."\n";
      }
    }
    return(preg_replace("/\n$/","",$ret));
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
