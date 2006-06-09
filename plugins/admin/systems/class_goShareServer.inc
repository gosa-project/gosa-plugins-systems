<?php

class goShareServer extends plugin{

  var $cli_summary      = "This plugin is used within the ServerService Pluign \nand indicates that this server supports shares.";
  var $cli_description  = "Some longer text\nfor help";
  var $cli_parameters   = array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* This plugin only writes its objectClass */
  var $objectclasses    = array("goShareServer");
  var $attributes       = array("goExportEntry");
  var $StatusFlag       = "goShareServerStatus";

  /* This class can't be assigned twice so it conflicts with itsself */
  var $conflicts        = array("goShareServer");

  var $DisplayName      = "";
  var $dn               = NULL;
  var $acl;
  var $cn                   = "";
  var $goShareServerStatus  = "";
  var $goExportEntry        = array();
  var $allow_mounts         = false;
  var $mounts_to_remove     = array();
  var $mounts_to_add        = array();

  function goShareServer($config,$dn)
  {
    plugin::plugin($config,$dn);

    $this->DisplayName = _("File service");

    $tmp =array();
    if(isset($this->attrs['goExportEntry'])){
      if(isset($this->attrs['goExportEntry']['count'])){
        for($i= 0; $i<$this->attrs['goExportEntry']['count']; $i++){
          $entry= $this->attrs['goExportEntry'][$i];
          $tmp[preg_replace('/\|.*$/', '', $entry)]= $entry;
        }
      }
    } 
    $this->goExportEntryList = $tmp;

    $ldap = $this->config->get_ldap_link();
    $avl_objectclasses = $ldap->get_objectclasses();
    if (isset($avl_objectclasses["mount"])) {
      $this->allow_mounts = true;
    }
  }


  function execute()
  { 
    $smarty = get_smarty(); 


    if((isset($_POST['DelNfsEnt']))&&(isset($_POST['goExportEntryList']))){
      if($this->allow_mounts){
        foreach($_POST['goExportEntryList'] as $entry){
          $this->deleteFromMountList($this->goExportEntryList[$entry]);
        }
      }
      foreach($_POST['goExportEntryList'] as $entry){
        $this->deleteFromList($entry);
      }
    }

    if(isset($_POST['NewNfsAdd'])){
      $this->oldone = NULL;
      $this->o_subWindow = new servnfs($this->config,$this->acl, $this->allow_mounts, $this->dn);
      $this->dialog = true;
    }

    if((isset($_POST['NewNfsEdit']))&&(isset($_POST['goExportEntryList']))){
      $entry = $this->goExportEntryList[$_POST['goExportEntryList'][0]];
      $add_mount=isset($this->mounts_to_add[$entry]);
      $this->oldone=$entry;
      $this->o_subWindow = new servnfs($this->config,$this->acl,$this->allow_mounts,$this->dn,$entry,$add_mount);
      $this->dialog = true;
    }
    if(isset($this->o_subWindow)){
      $this->o_subWindow->save_object(TRUE);
    }

    /* Save NFS setup */
    if(isset($_POST['NFSsave'])){
      if(count($this->o_subWindow->check())>0){
        foreach($this->o_subWindow->check() as $msg) {
          print_red($msg);
        }
      }else{
        $this->o_subWindow->save_object();
        $newone = $this->o_subWindow->save();
        $this->addToList($newone);
        if($this->allow_mounts){
          if($this->oldone) {
            $this->deleteFromMountList($this->oldone);
          }
          if ($this->o_subWindow->should_create_mount()) {
            $this->addToMountList($newone);
          }
        }
        unset($this->o_subWindow);
        $this->dialog = false;
      }
    }

    /* Cancel NFS setup */
    if(isset($_POST['NFScancel'])){
      $this->oldone = NULL;
      unset($this->o_subWindow);
      $this->dialog = false;
    }

    /* Execute NFS setup dialog*/
    if(isset($this->o_subWindow)){
      return $this->o_subWindow->execute();
    }





    foreach($this->attributes as $attr){
      $smarty->assign($attr,$this->$attr);
      $smarty->assign($attr."ACL",chkacl($this->acl,$attr));
    }
    $tellSmarty= array();
    ksort($this->goExportEntryList);
    foreach($this->goExportEntryList as $name=>$values){
      $tmp = split("\|",$values);
      $tellSmarty[$name] = $tmp[0]." ".$tmp[4]." (".$tmp[2].")";
    }
    $smarty->assign("goExportEntry",array_keys($tellSmarty));
    $smarty->assign("goExportEntryKeys",($tellSmarty));
    $smarty->assign("goExportEntryACL", chkacl($this->acl, "goExportEntry"));


    return($smarty->fetch(get_template_path("goShareServer.tpl",TRUE,dirname(__FILE__))));
  }


  function getListEntry()
  {
    $this->updateStatusState();
    $flag = $this->StatusFlag;
    $fields['Status']     = $this->$flag;
    $fields['Message']    = _("Shares");
    $fields['AllowStart'] = true;
    $fields['AllowStop']  = true;
    $fields['AllowRestart'] = true;
    $fields['AllowRemove']= true;
    $fields['AllowEdit']  = true;
    return($fields);
  }


  function remove_from_parent()
  {
    plugin::remove_from_parent();
    /* Check if this is a new entry ... add/modify */
    $ldap = $this->config->get_ldap_link();
    $ldap->cat($this->dn,array("objectClass"));
    if($ldap->count()){
      $ldap->cd($this->dn);
      $ldap->modify($this->attrs);
    }else{
      $ldap->cd($this->dn);
      $ldap->add($this->attrs);
    }
    show_ldap_error($ldap->get_error());
    $this->handle_post_events("remove");
  }


  function save()
  {
    plugin::save();

    /* Arrays */
    foreach (array("goExportEntryList"=>"goExportEntry") as $source => $destination){
      $this->attrs[$destination]= array();
      foreach ($this->$source as $element){
        $this->attrs[$destination][]= $element;
      }
    }


    /* Process netatalk mounts */
    if($this->allow_mounts) {
      $this->process_mounts();
    }


    /* Check if this is a new entry ... add/modify */
    $ldap = $this->config->get_ldap_link();
    $ldap->cat($this->dn,array("objectClass"));
    if($ldap->count()){
      $ldap->cd($this->dn);
      $ldap->modify($this->attrs);
    }else{
      $ldap->cd($this->dn);
      $ldap->add($this->attrs);
    }
    show_ldap_error($ldap->get_error());
    if($this->initially_was_account){
      $this->handle_post_events("modify");
    }else{
      $this->handle_post_events("add");
    }
  }


  /* Directly save new status flag */
  function setStatus($value)
  {
    if($value == "none") return;
    if(!$this->initially_was_account) return;
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->dn);
    $ldap->cat($this->dn,array("objectClass"));
    if($ldap->count()){

      $tmp = $ldap->fetch();
      for($i = 0; $i < $tmp['objectClass']['count']; $i ++){
        $attrs['objectClass'][] = $tmp['objectClass'][$i];
      }
      $flag = $this->StatusFlag;
      $attrs[$flag] = $value;
      $this->$flag = $value;
      $ldap->modify($attrs);
      show_ldap_error($ldap->get_error());
      $this->action_hook();
    }
  }


  function check()
  { 
    $message = plugin::check();
    return($message);
  }


  function save_object()
  {
    if(isset($_POST['goShareServerPosted'])){
      plugin::save_object();
    }
  } 

  function addToList($entry){
    $key =  key($entry);
    $this->goExportEntryList[$key]=$entry[$key];
  }

  function deleteFromList($id){
    unset($this->goExportEntryList[$id]);
  }

   function process_mounts() {

    $clip = "cn=" . $this->cn . ",ou=servers,ou=systems,";
    $mountsdn = "cn=mounts," . substr($this->dn, strlen($clip));

    $mounts = array(
      "objectClass" => "container",
      "cn" => "mounts"
    );

    # load data from mounts container
    $ldap = $this->config->get_ldap_link();
    $ldap->cat($mountsdn, array('dn'));
    $attrs = $ldap->fetch();

    # mounts container not present yet, so we create it
    if (count($attrs) == 0) {
        $ldap->cd($mountsdn);
        $ldap->add($mounts);
        show_ldap_error($ldap->get_error(), _("Creating mount container failed"));
        gosa_log("Mount container '$mountsdn' has been created");
    }

    # remove deleted mounts from the container
    foreach ($this->mounts_to_remove as $entry) {
      $mount=$this->returnMountEntry($entry);
      $mountdn = "cn=".$mount["cn"].","."$mountsdn";

      $ldap->cat($mountdn, array('dn'));
      $attrs = $ldap->fetch();

      if (count($attrs) != 0) {
        $ldap->rmdir($mountdn);
        show_ldap_error($ldap->get_error(), _("Removing mount container failed"));
        gosa_log("Mount object '".$mountdn."' has been removed");
      }
    }

    # add new mounts to the container
    foreach ($this->mounts_to_add as $entry) {

      $mount=$this->returnMountEntry($entry);
      $mountdn = "cn=".$mount["cn"].","."$mountsdn";
      $ldap->cd($mountdn);
      $ldap->add($mount);
      show_ldap_error($ldap->get_error(), _("Saving mount container failed"));
      gosa_log("Mount object '".$mountdn."' has been added");
    }
  }

   function action_hook($add_attrs= array())
  {
    /* Find postcreate entries for this class */
    $command= search_config($this->config->data['MENU'], get_class($this), "ACTION_HOOK");
    if ($command == "" && isset($this->config->data['TABS'])){
      $command= search_config($this->config->data['TABS'], get_class($this), "ACTION_HOOK");
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

      /* If there are still some %.. in our command, try to fill these with some other class vars */
      if(preg_match("/%/",$command)){
        $attrs = get_object_vars($this);
        foreach($attrs as $name => $value){
          if(!is_string($value)) continue;
          $command= preg_replace("/%$name/", $value, $command);
        }
      }

      if (check_command($command)){
        @DEBUG (DEBUG_SHELL, __LINE__, __FUNCTION__, __FILE__,
            $command, "Execute");

        exec($command);
      } else {
        $message= sprintf(_("Command '%s', specified as ACTION_HOOK for plugin '%s' doesn't seem to exist."), $command, get_class($this));
        print_red ($message);
      }
    }
  }

  /* Get updates for status flag */
  function updateStatusState()
  {
    if(empty($this->StatusFlag)) return;

    $attrs = array();
    $flag = $this->StatusFlag;
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->cn);
    $ldap->cat($this->dn,array($flag));
    if($ldap->count()){
      $attrs = $ldap->fetch();
    }
    if(isset($attrs[$flag][0])){
      $this->$flag = $attrs[$flag][0];
    }
  }

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
