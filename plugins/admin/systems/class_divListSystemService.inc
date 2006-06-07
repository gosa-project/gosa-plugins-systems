<?php

class divListSystemService extends MultiSelectWindow
{
  /* CheckBoxes, to change default values modify $this->AddCheckBox */
  var $parent             ;
  var $ui                 ;

  function divListSystemService ($config,$parent)
  {
    MultiSelectWindow::MultiSelectWindow($config,"SystemService");
    
    $this->parent       = $parent;
    $this->ui           = get_userinfo();

    /* Set list strings */
    $this->SetTitle(_("Installed services"));
    $this->SetSummary(_("Installed services"));

    /* Result page will look like a headpage */
    $this->SetHeadpageMode();
    $this->SetInformation(_("This menu allows you to add, remove and configure the properties of a specific service."));

    /* Disable buttonsm */
    $this->EnableCloseButton(false);
    $this->EnableSaveButton (false);

    /* set Page header */
    $this->AddHeader(array("string"=>"&nbsp;","attach"=>"style='width:20px;'"));
    $this->AddHeader(array("string"=>_("Service name")));
    $this->AddHeader(array("string"=>_("Actions"),"attach"=>"style='width:100px;border-right:0px;'"));
    $this->HideFilterPart();
    $this->SetHeight(500);
  }

  function GenHeader()
  {
    /* Prepare departments,
       which are shown in the listbox on top of the listbox
     */
    $options= "";
    foreach ($this->config->idepartments as $key => $value){
      if ($this->selectedBase == $key){
        $options.= "<option selected='selected' value='$key'>$value</option>";
      } else {
        $options.= "<option value='$key'>$value</option>";
      }
    }
    $listhead = "<div style='background:#F0F0F9;padding:5px;'>".
    " <input class='center' type='image' align='middle' src='images/select_new_server.png' class='center' 
        name='addNewService' alt='"._("Add service")."' title='"._("Add new service")."'>&nbsp;".
    " <img class='center' src='images/list_seperator.png' align='middle' alt='' height='16' width='1'>&nbsp;".
    " <input class='center' type='image' align='middle' src='images/status_start_all.png' class='center'
        name='startServices' alt='"._("Start all")."' title='"._("Start all services")."'>&nbsp;".
    " <input class='center' type='image' align='middle' src='images/status_stop_all.png'  class='center'   
        name='stopServices' alt='"._("Stop service")."' title='"._("Stop all services")."'>&nbsp;".
    " <input class='center' type='image' align='middle' src='images/status_restart_all.png'    class='center'
        name='resetServices' alt='"._("Reset service")."' title='"._("Reset all services")."'>&nbsp;".
    "</div>";
    $this->SetListHeader($listhead);
  }

  function execute()
  {
    $this->ClearElementsList();
    $this->GenHeader();
  }

  function setEntries($list)
  {
    foreach($list as $name => $entry){
 
      switch($entry['Status']){
        case ''      : $str ="";break;
        case SERVICE_STOPPED  : $str ="<img src='images/status_stopped.png' title='"._("Stopped")."'>"; break;
        case SERVICE_STARTED  : $str ="<img src='images/status_running.png' title='"._("Started")."'>"; break;
        case SERVICE_RESETTED : $str ="<img src='images/status_restarting.png' title='"._("Restarting")."' alt='R'>"; break;
        default: $str= "<img src='images/select_user.png' alt='".$entry['Status']."' title='"._("User status")." : ".$entry['Status']."'>";
      }

      $field1 = array("string" => $str ,"attach" => "style='width:20px;'");
      $field2 = array("string" => $entry['Message'] );

      $actions ="";
      if($entry['AllowStart']){
        $actions .= "&nbsp;<input type='image' name='StartSingleService_".$name."' src='images/status_start.png'>";
      }else{
        $actions .= "&nbsp;<img src='images/empty' width='16' alt=''>";
      }
      if($entry['AllowStart']){
        $actions .= "&nbsp;<input type='image' name='StopSingleService_".$name."' src='images/status_stop.png'>";
      }else{
        $actions .= "&nbsp;<img src='images/empty' width='16' alt=''>";
      }
      if($entry['AllowStart']){
        $actions .= "&nbsp;<input type='image' name='ResetSingleService_".$name."' src='images/status_restart.png'>";
      }else{
        $actions .= "&nbsp;<img src='images/empty' width='16' alt=''>";
      }
      if($entry['AllowEdit']){
        $actions .= "&nbsp;<input type='image' name='EditSingleService_".$name."' src='images/edit.png'>";
      }else{
        $actions .= "&nbsp;<img src='images/empty' width='16' alt=''>";
      }
      if($entry['AllowRemove']){
        $actions .= "&nbsp;<input type='image' name='RemoveSingleService_".$name."' src='images/edittrash.png'>";
      }else{
        $actions .= "&nbsp;<img src='images/empty' width='16' alt=''>";
      }

      $field3 = array("string" => $actions ,"attach" => "style='width:100px;border-right:0px;'");
      $this->AddElement(array($field1,$field2,$field3));
    }
  }

  function Save()
  {
    MultiSelectWindow :: Save();  
  }

  function save_object()
  {
    /* Save automatic created POSTs like regex, checkboxes */
    MultiSelectWindow :: save_object();   
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
