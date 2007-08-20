<?php

class goto_task extends plugin
{
  /* Definitions */
  var $plHeadline     = "System mass deployment";
  var $plDescription  = "This does something";
 
  var $parent   = NULL;
  var $Expert   = FALSE;
 
  var $Minute   = "*";
  var $Hour     = "*";
  var $Day      = "*";
  var $Month    = "*";
  var $Weekday  = "*";
  var $Action   = "install";
  var $Comment  = "";
  var $Target   = array();

  var $Actions  = array();
  var $OGroup   = "";
  var $OGroups  = array();
  var $attributes     = array("OGroup","Minute","Hour","Day","Month","Weekday","Action","Comment","Target");


  function goto_task($config,$parent,$data = array())
  {
    /* Set parent object */
    $this->parent = $parent;

    /* Intialize plugin */
    $this->config = $config;
    if(count($data)){
    
      $tmp = $this->get_array_values();
      foreach($this->attributes as $attr){
        if(!isset($data[$attr])){
          trigger_error("Missing parameter: '".$attr."' for goto_task contruction.");
        }else{
          $this->$attr = $data[$attr];
  
          /* Check if given value will be selectable via dropdown menus, else enable expert mode */
          if(in_array($attr, array("Minute","Hour","Day","Month","Weekday")) && !isset($tmp[$attr][$data[$attr]])){
            $this->Expert = TRUE;
          }
        }
      }
    }

    /* Create ogroup select list */
    $this->OGroups = array("keep_current"     => "["._("Keep current")."]",
                           "unset_membership" => "["._("Quit group membership")."]");
    $this->OGroups = array_merge($this->OGroups,$this->parent->get_object_groups());

    /* Prepare list of available actions */
    $this->Actions = $this->parent->get_actions();
  }


  function execute()
  {
    /********
     * Handle Posts  
     ********/
  
    /* Handle Target options */
    foreach($_POST as $name => $value){
      if(preg_match("/^remove_/",$name)){
        $value = preg_replace("/^remove_([0-9]*)_(x|y)$/i","\\1",$name);
        if(isset($this->Target[$value])){
          unset($this->Target[$value]);
        }
      }
    }

    /* Add target */
    if(isset($_POST['add_target']) && !empty($_POST['target_text'])){
      $target = get_post("target_text");
      if($this->is_valid_target($target) && !in_array($target,$this->Target)){
        $this->Target[] = $target;
      }
    }
 
 
    /********
     * Add target from list 
     ********/

    /* If add from list is was requsted, display this list */ 
    if(isset($_POST['add_from_list'])){
      $this->dialog = new target_list($this->config,$this->Target);
    }

    /* Save selected objects as target */
    if(isset($_POST['SaveMultiSelectWindow'])){
      $this->dialog->save_object();
      $ret = $this->dialog->save();
      foreach($ret as $entry){
        $this->Target[] = $entry['cn'][0];
      }
      $this->dialog = NULL;
    }

    /* Cancel object listing */
    if(isset($_POST['CloseMultiSelectWindow'])){
      $this->dialog = NULL;
    }

    /* Display object Listing */
    if($this->dialog){
      $this->dialog->save_object();
      return($this->dialog->execute());
    }


    /********
     * Display this plugin  
     ********/

    $divlist = new divlist("goto_task");
    $divlist->SetWidth("100%");
    $divlist->SetHeight("160");
    $divlist->SetEntriesPerPage(0);

    $divlist->SetHeader(array(
                            array("string" => "Target"),
                            array("string" => "Actions" , "attach" => "style='width:40px;'")));

    $acl_target = $this->parent->getacl("Target");
    foreach($this->Target as $key => $target){

      $field1 = array("string" => $target);
      if(preg_match("/w/i",$acl_target)){
        $field2 = array("string" => "<input type='image' src='images/edittrash.png' name='remove_".$key."'>" , 
                                    "attach" => "style='width:44px;'");
      }else{
        $field2 = array("string" => "",
                                    "attach" => "style='width:44px;'");
      }

      $divlist->AddEntry(array($field1,$field2));
    } 

    $smarty = get_smarty();

    foreach($this->attributes as $attr){
      $smarty->assign($attr."ACL", $this->parent->getacl($attr));
      $smarty->assign($attr,$this->$attr);
    }

    $tmp = $this->get_array_values();
    $smarty->assign("Minutes" , $tmp['Minute']);
    $smarty->assign("Hours"   , $tmp['Hour']);
    $smarty->assign("Days"    , $tmp['Day']);
    $smarty->assign("Months"  , $tmp['Month']);
    $smarty->assign("Weekdays", $tmp['Weekday']);

    $smarty->assign("OGroups"  , $this->OGroups);
    $smarty->assign("Expert"  , $this->Expert);

    $smarty->assign("Actions"     , $this->Actions);
    $smarty->assign("Target_list" , $divlist->DrawList());
    return ($smarty->fetch (get_template_path('goto_task.tpl', TRUE)));
  }

  
  /* check given values */
  function check()
  {
    $message = plugin::check();
    $tmp = array(
      "OGroup" => _("Object group") ,"Minute" => _("Minute"),
      "Hour"   => _("Hour")         ,"Day"    => _("Day"),
      "Month"  => _("Month")        ,"Weekday"=> _("Week day"),
      "Action" => _("Action")       ,"Comment"=> _("Description"),
      "Target" => _("Target objects")); 
    
    foreach($tmp as $name => $desc){
      if(empty($this->$name)){
        $message[] = sprintf(_("The given value for attribute '%s' is invalid."),$desc);
      }
    }
    return($message);
  }
  

  /* Map acl function, to be able to use plugin::save_object() */
  function acl_is_writeable($str)
  {
    return($this->parent->acl_is_writeable($str));
  }


  function save_object()
  {
    if(isset($_POST['goto_task_posted'])){
      plugin::save_object();

      if(isset($_POST['ToggleExpertMode'])){
        $this->Expert = !$this->Expert;
      }
    }
  }  

  
  /* Check if given target is vald.
   * It must either be a valid MAC address or an existing object group
   */
  function is_valid_target($str)
  {
    if(is_mac($str)){
      return(TRUE);
    }else{
      $ldap = $this->config->get_ldap_link();
      $ldap->cd($this->config->current['BASE']);
      $ldap->search("(&(objectClassgosaGroupOfNames)(cn=".$str."))",array("cn"));
      if($ldap->count()){
        return(TRUE);
      }
    }
  }

  function save()
  {
    $tmp = array();
    foreach($this->attributes as $attr){
      $tmp[$attr] = $this->$attr;
    }
    return($tmp);
  }


  /* Return values for listboxes. 
   */
  function get_array_values()
  {
    $ret = array();    

    /* Create minute array */
    $Minute = array( "*"    => "*",
                      "*/1"  => "*/1",
                      "*/3"  => "*/3",
                      "*/5"  => "*/5",
                      "*/10" => "*/10",
                      "*/15" => "*/15",
                      "*/30" => "*/30",
                      "*/45" => "*/45",
                      "*/60" => "*/60"); 
    for($i = 0; $i < 60 ; $i ++){
      $Minute[$i] = $i;
    }

    /* Create hour array */
    $Hour = array(   "*" => "*");
    for($i = 1 ; $i < 24 ; $i ++ ){
      $Hour["*/".$i] = "*/".$i;
    } 
    for($i = 0 ; $i < 24 ; $i ++ ){
      $Hour[$i] = $i;
    } 

    /* Create hour array */
    $Day = array( "*" => "*");
    for($i = 1 ; $i < 32 ; $i ++ ){
      $Day["*/".$i] = "*/".$i;
    } 
    for($i = 1 ; $i < 32 ; $i ++ ){
      $Day[$i] = $i;
    } 
   
    /* Create month array */
    $Month = array( "*" => "*");
    for($i = 1 ; $i <= 12 ; $i ++ ){
      $Month["*/".$i] = "*/".$i;
    } 
    for($i = 1 ; $i <= 12 ; $i ++ ){
      $Month[$i] = $i;
    } 
   
    /* Create week day array */
    $Weekday = array( "*" => "*");
    for($i = 1 ; $i <= 7 ; $i ++ ){
      $Weekday["*/".$i] = "*/".$i;
    } 
    for($i = 0 ; $i <= 7 ; $i ++ ){
      $Weekday[$i] = $i;
    } 
   
    foreach(array("Minute","Weekday","Hour","Day","Month") as $var){
      $ret[$var] = $$var;
    }
    return($ret);
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
