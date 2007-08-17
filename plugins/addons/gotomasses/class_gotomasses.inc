<?php

class gotomasses extends plugin
{
  /* Definitions */
  var $plHeadline     = "System mass deployment";
  var $plDescription  = "This does something";

  /* attribute list for save action */
  var $attributes= array();
  var $objectclasses= array();

  /* Source file that contains the csv data */
  var $file_to_read = "Undefined"; #Set in constructor 

  /* Queue tasks */
  var $tasks = array();
  var $current =false;
  var $dialog = NULL;

  function gotomasses($config, $dn= NULL)
  {
    /* Define source file */
    $this->file_to_read = CONFIG_DIR."/gotomasses_machines";
  
    /* Include config object */
    $this->config= $config;

    $this->load_csv_data();
  }



  function execute()
  {
    /************
     * Handle posts 
     ************/

    $s_entry = $s_action = "";
    $arr = array("/^edit_/"=>"edit","/^remove_/"=>"remove");
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
      unset($this->tasks[$s_entry]);
    }

    /* Edit selected entry */
    if($s_action == "edit" && isset($this->tasks[$s_entry])){
      $entry = $this->tasks[$s_entry];
      $this->dialog = new goto_task($this->config,$this,$entry);
      $this->current = $s_entry;
    }
  
    /* Close dialog */
    if(isset($_POST['close_goto_task'])){
      $this->dialog = NULL;
    }

    /* Close dialog */
    if(isset($_POST['save_goto_task'])){
      $this->dialog->save_object();
      $msgs = $this->dialog->check();
      if(count($msgs)){
        foreach($msgs as $msg){
          print_red($msg);  
        }
      }else{
        $this->tasks[$this->current] = $this->dialog->save();
        $this->dialog = NULL;
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

    $smarty = get_smarty();
    $plug = $_GET['plug'];
    $divlist = new divlist("gotomasses");
    $divlist->SetWidth("100%");
    $divlist->SetHeader(array(  array("string"=>"<a href='?plug=".$plug."&amp;sort=object'>"._("Target")."</a>"),
                                array("string"=>"<a href='?plug=".$plug."&amp;sort=schedule'>"._("Schedule")."</a>",
                                      "attach"=>"style='width:150px;'"),
                                array("string"=>"<a href='?plug=".$plug."&amp;sort=action'>"._("Type")."</a>",
                                      "attach"=>"style='width:50px;'"),
                                array("string"=>_("Action"),
                                      "attach"=>"style='border-right:0px;width:50px;'")));

   
    foreach($this->tasks as $key => $task){
  
      $action = "<input type='image' src='images/edit.png' name='edit_".$key."' class='center' alt='"._("Edit")."'>";
      $action.= "<input type='image' src='images/edittrash.png' name='remove_".$key."' class='center' alt='"._("Reomve")."'>";

      $field1 = array("string" => $this->target_to_string($task));
      $field2 = array("string" => $this->time_to_string($task));
      $field3 = array("string" => $this->action_to_string($task));
      $field4 = array("string" => $action,"attach" => "style='border-right:0px;'");
      
 
      $divlist->AddEntry(array($field1,$field2,$field3,$field4));
    }
 
    $smarty->assign("divlist" , $divlist->DrawList());
    $smarty->assign("contents_modified" , TRUE );
    return ($smarty->fetch (get_template_path('contents.tpl', TRUE)));
  }

  
  function target_to_string($data)
  {
    $ret = "";
    foreach($data['Target'] as $target){
      $ret .= preg_replace("/^[^:]+:/i","",$target).", ";
    } 
    return(preg_replace("/, $/","",$ret));
  }

  
  function time_to_string($data)
  {
    return($data['Minute']." ".$data['Hour']." ".$data['Day']." ".$data['Month']." ".$data['Weekday']);
  }

  
  function action_to_string($data)
  {
    switch($data['Action']){

      case 'reinstall' : return("Reinstall");break;
      case 'install' : return("Install");break;
      case 'reboot' : return("Restart");break;
      case 'update' : return("Update");break;
      default : return("Unknown");
    }
  }

  
  function load_csv_data()
  {
    $ui = get_userinfo();

    
    if(!file_exists($this->file_to_read) || !is_readable($this->file_to_read)){
      print_red(sprintf(_("Can't locate or read csv storage file '%s'."),$this->file_to_read));
      return(FALSE);
    }

    $fp = @fopen($this->file_to_read,"r");
    if(!$fp){
      print_red(sprintf(_("Can't read csv storage file '%s'."),$this->file_to_read));
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
        $entry['Target']  = split(",",$parts[7]);  
        $entry['Comment'] = $comment;  
        $this->tasks []   = $entry;
      }
    } 
  }


  function save_csv_data()
  {
  }


  function save_object()
  {
    if(isset($_POST['gotomasses'])){
    }
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


  function get_actions()
  {
    /* Prepare list of available actions */
    $actions = array( "reboot"          => _("Reboot"),
                            "localboot"       => _("Localboot"),
                            "halt"            => _("Halt system"),
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
