<?php

class gotomasses extends plugin
{
  /* Definitions */
  var $plHeadline     = "System deployment";
  var $plDescription  = "This does something";

  /* attribute list for save action */
  var $attributes= array();
  var $objectclasses= array();

  /* Source file that contains the gotomasses data */
  var $data_file = "Undefined"; #Set in constructor 

  /* Queue tasks */
  var $tasks = array();
  var $current =false;
  var $dialog = FALSE;

  function gotomasses($config, $dn= NULL)
  {
    /* Define source file */
    $this->data_file = CONFIG_DIR."/gotomasses_machines";
  
    /* Include config object */
    $this->config= $config;

    $this->load_gotomasses_data();
  }



  function execute()
  {
    $smarty = get_smarty();
    /************
     * Handle posts 
     ************/

    $s_entry = $s_action = "";
    $arr = array("/^edit_task_/"=>"edit","/^remove_task_/"=>"remove","/^new_task_/"=>"new_task");
    foreach($arr as $regex => $action){
      foreach($_POST as $name => $value){
        if(preg_match($regex,$name)){
          $s_action = $action;
          $s_entry  = preg_replace($regex,"",$name);
          $s_entry  = preg_replace("/_(x|y)$/","",$s_entry);
        }
      }
    }

    /************
     * List posts 
     ************/

    /* Remove entry from list */
    if($s_action == "remove" && isset($this->tasks[$s_entry])){
      if(!$this->acl_is_removeable()){
        print_red(_("You are not allowed to remove a task."));
      }else{
        $smarty->assign("info",_("Your are about to delete a gotomasses task."));
        $this->current = $s_entry;
        return($smarty->fetch(get_template_path('remove.tpl', TRUE)));
      }
    }

    /* Remove entry, remove is confirmed */
    if($this->current != -1 && isset($_POST['delete_confirm'])){
      unset($this->tasks[$this->current]);
      $this->current = -1;
      $this->save();
    }

    /* Edit selected entry */
    if($s_action == "edit" && isset($this->tasks[$s_entry])){
      $entry = $this->tasks[$s_entry];
      $this->dialog = new goto_task($this->config,$this,$entry);
      $this->current = $s_entry;
    }

    /* New entry */
    if($s_action== "new_task" && $this->acl_is_createable()){
      $this->dialog = new goto_task($this->config,$this);
      $this->current = -1;
    }
  
    /* Close dialog */
    if(isset($_POST['close_goto_task'])){
      $this->dialog = FALSE;
      $this->current = -1;
    }

    /* Close dialog */
    if((isset($_POST['save_goto_task']) || isset($_POST['apply_goto_task'])) && is_object($this->dialog) ){
      $this->dialog->save_object();
      $msgs = $this->dialog->check();
      if(count($msgs)){
        foreach($msgs as $msg){
          print_red($msg);  
        }
      }else{
        if(isset($this->tasks[$this->current]) && $this->current != -1){
          $this->tasks[$this->current] = $this->dialog->save();
        }else{
          $this->tasks[] = $this->dialog->save();
        }
        if(!isset($_POST['apply_goto_task'])){
          $this->dialog = FALSE;
          $this->current = -1;
        }
        $this->save();
      }
    }

    /* Display dialogs if currently opened */
    if(is_object($this->dialog)){
      $this->dialog->save_object();
      return($this->dialog->execute());
    }


    /************
     * Handle Divlist 
     ************/

    $plug = $_GET['plug'];
    $divlist = new MultiSelectWindow($this->config,"GotoMasses","gotomassses");
    $divlist->SetSummary(_("Gotomasses tasks"));
    $divlist->SetHeadpageMode();
    $divlist->EnableCloseButton(FALSE);
    $divlist->EnableSaveButton(FALSE);
    $divlist->SetInformation(_("This menu allows you to add, remove and change the properties of gotomasses tasks."));
 
    /* Display add button if allowed */ 
    if($this->acl_is_createable()){ 
      $header = "<div style='padding:5px'>
        <input type='image' src='images/gotomasses_new_task.png' name='new_task' class='center'>
        </div>";
    }else{
      $header = "";
    }

    /* Get Permissions */ 
    $ui = get_userinfo();
    $acls = $this->getacl("");
    foreach($ui->get_module_departments("gotomasses") as $dep){
      $acls .= $ui->get_category_permissions($dep,"gotomasses");
    }
 
    /* Create divlist */
    $divlist->SetListHeader($header);
    $divlist->AddHeader(array("string"=>"<a href='?plug=".$plug."&amp;sort=object'>"._("Target")."</a>"));
    $divlist->AddHeader(array("string"=>"<a href='?plug=".$plug."&amp;sort=schedule'>"._("Schedule")."</a>",
                                      "attach"=>"style='width:100px;'"));
    $divlist->AddHeader(array("string"=>"<a href='?plug=".$plug."&amp;sort=action'>"._("Type")."</a>",
                                      "attach"=>"style='width:80px;'"));
    $divlist->AddHeader(array("string"=>_("Action"),
                                      "attach"=>"style='border-right:0px;width:40px;'"));

    if(!empty($acls)){
      foreach($this->tasks as $key => $task){
        $action = "<input type='image' src='images/edit.png' name='edit_task_".$key."' class='center' alt='"._("Edit")."'>";
        if($this->acl_is_removeable()){
          $action.= "<input type='image' src='images/edittrash.png' name='remove_task_".$key."' class='center' alt='"._("Reomve")."'>";
        }
        $field1 = array("string" => "<div style='width:100%;overflow:hidden;'><nobr>".$this->target_to_string($task)."</nobr></div>");
        $field2 = array("string" => $this->time_to_string($task),"attach" => "style='width:100px;'");
        $field3 = array("string" => $this->action_to_string($task),"attach" => "style='width:80px;'");
        $field4 = array("string" => $action,"attach" => "style='text-align:right;width:40px;border-right:0px;'");
        $divlist->AddElement(array($field1,$field2,$field3,$field4));
      }
    }
 
    return($divlist->Draw());
  }

  
  function target_to_string($data)
  {
    $ret = "";
    if($data['Action'] == "initial_install"){
      foreach($data['Initial_Target'] as $target){
        $ret .= $target['MAC'].", ";
      } 
    }else{
      foreach($data['Target'] as $target){
        $ret .= $target.", ";
      } 
    }
    return(preg_replace("/, $/","",$ret));
  }

  
  function time_to_string($data)
  {
    return($data['Minute']." ".$data['Hour']." ".$data['Day']." ".$data['Month']." ".$data['Weekday']);
  }

  
  function action_to_string($data)
  {
    $tmp = $this->get_actions();
    if(isset($tmp[$data['Action']])){
      return($tmp[$data['Action']]);
    }else{
      return(_("Unknown"));
    }
  }

  
  function load_gotomasses_data()
  {
    $ui = get_userinfo();

    /* Check file existence */
    if(!file_exists($this->data_file) || !is_readable($this->data_file)){
      print_red(sprintf(_("Can't locate or read gotomasses storage file '%s'."),$this->data_file));
      return(FALSE);
    }

    /* check if file is readable */
    $fp = @fopen($this->data_file,"r");
    if(!$fp){
      print_red(sprintf(_("Can't read gotomasses storage file '%s'."),$this->data_file));
      return(FALSE);
    }

    /* Get file contents */
    $data ="";
    while(!feof($fp)){
      $data.= fread($fp,512);
    }

    /* Get lines from file */
    $this->tasks  = array(); 
    $comment      = "";
    $rows         = split("\n",$data);

    /* Walk trough rows and parse data */
    foreach($rows as $row){

      /* Skip empty lines */
      $row = trim($row);
      if(empty($row)){
        continue;
      }

      /* Get comment, if available */     
      if(preg_match("/^#/",$row)){
        $comment = preg_replace("/^#/","",$row);
        continue;
      }

      /* Split row into minutes/ hours ...*/ 
      $row    = preg_replace('/[\t ]/umi'," ",$row);
      $row    = preg_replace('/  */umi'," ",$row);
      $parts  = split(" ",$row);

      if(count($parts) != 8){
        print_red(_("Entry broken, skipped."));
      }else{

        $entry = array();
        $entry['Minute']  = $parts[0];  
        $entry['Hour']    = $parts[1];  
        $entry['Day']     = $parts[2];  
        $entry['Month']   = $parts[3];  
        $entry['Weekday'] = $parts[4];  
        $entry['Action']  = $parts[5];  
        $entry['OGroup']  = $parts[6];  
        if($entry['Action'] == "initial_install"){
          $tmp2 = split(";",$parts[7]);
          foreach($tmp2 as $target){
            $tmp = split(",",$target);
            $entry['Initial_Target'][]  = array("MAC" => $tmp[0],"IP"=>$tmp[1],"NAME" => $tmp[2]);
          }
          $entry['Target']  = array();
        }else{
          $entry['Initial_Target']  = array();
          $entry['Target']  = split(";",$parts[7]);
        }
        $entry['Comment'] = $comment;  
        $this->tasks []   = $entry;
      }
    } 
  }


  function save_gotomasses_data()
  {
    $str = "#GOsa generated file, please just modify if you realy know what you do.";
    foreach($this->tasks as $task){
      $str .= "\n#".trim($task['Comment']);
      $str .= "\n";
      $str .= str_pad($task['Minute'] ,5," ")." ";
      $str .= str_pad($task['Hour']   ,5," ")." ";
      $str .= str_pad($task['Day']    ,5," ")." ";
      $str .= str_pad($task['Month']  ,5," ")." ";
      $str .= str_pad($task['Weekday'],5," ")." ";
      $str .= str_pad($task['Action'] ,5," ")." ";
      $str .= str_pad($task['OGroup'] ,5," ")." ";
      if($task['Action'] == "initial_install"){
        foreach($task['Initial_Target'] as $target){
          $str .= $target['MAC'].",".$target['IP'].",".$target['NAME'].";";
        }
      }else{
        foreach($task['Target'] as $target){
          $str .= $target.";";
        }
      }
      $str = preg_replace("/;$/","",$str);
    }

    /* check file existence*/
    $ui = get_userinfo();
    if(!file_exists($this->data_file) || !is_writeable($this->data_file)){
      print_red(sprintf(_("Can't locate or write gotomasses storage file '%s'."),$this->data_file));
      return(FALSE);
    }

    /* check permissions */
    $fp = @fopen($this->data_file,"w");
    if(!$fp){
      print_red(sprintf(_("Can't read gotomasses storage file '%s'."),$this->data_file));
      return(FALSE);
    }

    /* Write contents */
    $str .= "\n";
    fwrite($fp,$str);
    fclose($fp);
  }


  function save_object()
  {
  }


  /* Return list of object groups */
  function get_object_groups()
  {
    $ret = array();
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    $ldap->search("(&(objectClass=gosaGroupOfNames)(cn=*))",array("cn"));
    while($attrs = $ldap->fetch()){
      $ret[$attrs['cn'][0]] = $attrs['cn'][0];
    }
    return($ret); 
  }

  
  function save()
  {
    $this->save_gotomasses_data();
  }


  function get_actions()
  {
    /* Prepare list of available actions */
    $actions = array(       "reboot"          => _("Reboot"),
                            "localboot"       => _("Localboot"),
                            "halt"            => _("Halt system"),
                            "initial_install" => _("Initial installation"),
                            "update"          => _("Update"),
                            "reinstall"       => _("(Re)Install"),
                            "rescan"          => _("Rescan"),
                            "memcheck"        => _("Memory check"));
    return($actions);
  }


  function plInfo()
  {
    return (array(
        "plShortName"   => _("System mass deployment"),
        "plDescription" => _("Provide a mechanism to automatically activate a set of systems"),
        "plSelfModify"  => FALSE,
        "plDepends"     => array(),
        "plPriority"    => 0,
        "plSection"     => array("addon"),
        "plCategory"    => array("gotomasses" => array("objectClass" => "none", "description" => _("System mass deployment"))),
        "plProvidedAcls" => array("Comment"   => _("Description"), 
                                  "Action"    => _("Action"),
                                  "Day"       => _("Day"),
                                  "Minute"    => _("Minute"),
                                  "Hour"      => _("Hour"),
                                  "Month"     => _("Month"),
                                  "Weekday"   => _("Week day"),
                                  "Target"    => _("Target"))
        ));
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
