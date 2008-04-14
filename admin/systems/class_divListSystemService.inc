<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id$$
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

class divListSystemService extends MultiSelectWindow
{
  /* CheckBoxes, to change default values modify $this->AddCheckBox */
  var $parent             ;
  var $ui                 ;

  function divListSystemService ($config,$parent)
  {
    MultiSelectWindow::MultiSelectWindow($config,"SystemService", "server");
    
    $this->parent       = $parent;
    $this->ui           = get_userinfo();
    $this->EnableJSLeaveMsg(TRUE);

    /* Set list strings */
    $this->SetTitle(_("Installed services"));
    $this->SetSummary(_("Installed services"));

    /* Result page will look like a headpage */
    $this->SetInformation(_("Add, remove and configure the properties of system services here."));

    /* Disable buttonsm */
    $this->EnableCloseButton(false);
    $this->EnableSaveButton (false);

    /* set Page header */
    $this->AddHeader(array("string"=>"&nbsp;","attach"=>"style='width:20px;'"));
    $this->AddHeader(array("string"=>_("Name")));
    $this->AddHeader(array("string"=>_("Actions"),"attach"=>"style='width:100px;border-right:0px;'"));
    $this->HideFilterPart();
  }

  function GenHeader()
  {
    /* Prepare departments,
       which are shown in the listbox on top of the listbox
     */
    $listhead = "<div style='background:#F0F0F9;height:16px;padding:0px;'>".
    " <input type='image' align='middle' src='images/select_new_server.png' class='center' 
        name='addNewService' alt='"._("Add service")."' title='"._("Add new service")."'>&nbsp;".
    " <img class='center' src='images/list_seperator.png' align='middle' alt='' height='16' width='1'>&nbsp;".
    " <input type='image' align='middle' src='images/status_start_all.png' class='center'
        name='startServices' alt='"._("Start all")."' title='"._("Start all services")."'>&nbsp;".
    " <input type='image' align='middle' src='images/status_stop_all.png'  class='center'   
        name='stopServices' alt='"._("Stop service")."' title='"._("Stop all services")."'>&nbsp;".
    " <input type='image' align='middle' src='images/status_restart_all.png'    class='center'
        name='restartServices' alt='"._("Restart service")."' title='"._("Restart all services")."'>&nbsp;".
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
    $tmp = array();
    foreach($list as $name => $entry){
      $tmp[$name] = $entry['Message']; 
    }

    natcasesort($tmp);
    $tmp2 = array();
    foreach($tmp as $name => $value){
      $tmp2[$name] = $list[$name];
    }

    $ui = get_userinfo();

    foreach($tmp2 as $name => $entry){
 
      switch($entry['Status']){
        case ''      : $str ="&nbsp;";break;
        case SERVICE_STOPPED  : $str ="<img src='images/status_stopped.png' title='"._("Stopped")."'>"; break;
        case SERVICE_STARTED  : $str ="<img src='images/status_running.png' title='"._("Started")."'>"; break;
        case SERVICE_RESTARTED : $str ="<img src='images/status_restarting.png' title='"._("Restarting")."' alt='R'>"; break;
        default: $str= "<img src='images/select_user.png' alt='".$entry['Status']."' title='"._("User status")." : ".$entry['Status']."'>";
      }

      /* Get acls */
      $acl = $ui->get_permissions($this->parent->dn,"server/".$name);
      if(preg_match("/w/i",$acl) && !preg_match("/r/i",$acl)){
        continue;
      }
  
      $WasAccount = $this->parent->plugins[$name] -> initially_was_account;    
  
      $plug = $_GET['plug'];
      $link = "<a href='?plug=".$plug."&amp;act=open&amp;id=".$name."'>".$entry['Message']."</a>";

      $field1 = array("string" => $str ,"attach" => "style='width:20px;'");
      $field2 = array("string" => $link);

      $actions ="";
      if($entry['AllowStart'] && $WasAccount){
        $actions .= "&nbsp;<input type='image' name='StartSingleService_".$name."' src='images/status_start.png'
                            title='"._("Start service")."'>";
      }else{
        $actions .= "&nbsp;<img src='images/empty.png' width='16' alt=''>";
      }
      if($entry['AllowStart'] && $WasAccount){
        $actions .= "&nbsp;<input type='image' name='StopSingleService_".$name."' src='images/status_stop.png'
                            title='"._("Stop service")."'>";
      }else{
        $actions .= "&nbsp;<img src='images/empty.png' width='16' alt=''>";
      }
      if($entry['AllowStart'] && $WasAccount){
        $actions .= "&nbsp;<input type='image' name='RestartSingleService_".$name."' src='images/lists/reload.png'
                            title='"._("Restart service")."'>";
      }else{
        $actions .= "&nbsp;<img src='images/empty.png' width='16' alt=''>";
      }

      /* Check if edit is enabled and allowed for current service */
      if($entry['AllowEdit'] && preg_match("/(r|w)/i",$acl)){
        $actions .= "&nbsp;<input type='image' name='EditSingleService_".$name."' src='images/edit.png'
                            title='"._("Edit service")."'>";
      }else{
        $actions .= "&nbsp;<img src='images/empty.png' width='16' alt=''>";
      }

      /* Check if remove is enabled and allowed for current service */
      if($entry['AllowRemove'] && preg_match("/d/i",$acl)){
        $actions .= "&nbsp;<input type='image' name='RemoveSingleService_".$name."' src='images/lists/trash.png'
                            title='"._("Remove service")."'>";
      }else{
        $actions .= "&nbsp;<img src='images/empty.png' width='16' alt=''>";
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
