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

class divListSystem extends MultiSelectWindow
{

  /* Current base */
  var $selectedBase       = "";
  var $departments        = array();

  /* Regex */
  var $Regex              = "*";
  var $UserRegex          = "*";

  /* CheckBoxes, to change default values modify $this->AddCheckBox */
  var $ShowServers;
  var $ShowTerminals;
  var $ShowWorkstations;
  var $ShowWinWorkstations;
  var $ShowPrinters;
  var $ShowDevices;
  var $ShowPhones;

  /* Subsearch checkbox */
  var $SubSearch;

  var $parent             ;
  var $ui                 ;

  function divListSystem (&$config,$parent)
  {
    MultiSelectWindow::MultiSelectWindow($config, "System", array("server",
                                                                  "workstation",
                                                                  "terminal",
                                                                  "phone",
                                                                  "incoming",
                                                                  "printer"));

    $this->parent       = $parent;
    $this->ui           = get_userinfo();

    /* Set list strings */
    $this->SetTitle(_("List of systems"));
    $this->SetSummary(_("List of systems"));

    /* Result page will look like a headpage */
    $this->SetHeadpageMode();

    $this->EnableAplhabet   (true);

    /* Disable buttonsm */
    $this->EnableCloseButton(false);
    $this->EnableSaveButton (false);

    /* Dynamic action col, depending on snapshot icons */
    $action_col_size = 120;
    if($this->parent->snapshotEnabled()){
      $action_col_size += 38;
    }

    /* Toggle all selected / deselected */
    $chk = "<input type='checkbox' id='select_all' name='select_all' title='"._("Select all")."'
               onClick='toggle_all_(\"^item_selected_[0-9]*$\",\"select_all\");' >";

    /* set Page header */
    $this->AddHeader(array("string"=> $chk,          "attach"=>"style='width:20px;'"));
    $this->AddHeader(array("string"=>"&nbsp;","attach"=>"style='width:20px;'"));
    $this->AddHeader(array("string"=>_("Systems")." / "._("Department")));

    /* Add header for selected FAI release, if fai is active */
    if($this->parent->fai_activated){
      $this->AddHeader(array("string"=>_("Release"),"attach"=>"style='width:100px;'"));
    }

    $this->AddHeader(array("string"=>_("Actions"),"attach"=>"style='width:".$action_col_size."px;border-right:0px;'"));

    /*                  Text        ,Value    ,Name         ,Is selected */

    if(class_available("servtabs")){
      $this->AddCheckBox("ShowServers",        
          msgPool::selectToView(  _("servers")),            
          sprintf(_("Show %s"),   _("servers")),true);
    }
    if(class_available("termtabs")){
      $this->AddCheckBox("ShowTerminals",       
          msgPool::selectToView(  _("Linux terminals")),    
          sprintf(_("Show %s"),   _("terminals")),true);
    }
    if(class_available("worktabs")){
      $this->AddCheckBox("ShowWorkstations",
          msgPool::selectToView(  _("Linux workstations")), 
          sprintf(_("Show %s"),   _("workstations")),true);
    }
    if(class_available("wintabs")){
      $this->AddCheckBox("ShowWinWorkstations", 
          msgPool::selectToView(  _("MicroSoft Windows based workstations")), 
          sprintf(_("Show %s"),   _("windows based workstations")),true);
    }
    if(class_available("printtabs")){
      $this->AddCheckBox("ShowPrinters",
          msgPool::selectToView(  _("network printers")),   
          sprintf(_("Show %s"),   _("network printers")) ,true);
    }
    if(class_available("phonetabs")){
      $this->AddCheckBox("ShowPhones",
          msgPool::selectToView(  _("VoIP phones")),        
          sprintf(_("Show %s"),   _("phones")) ,true);
    }
    if(class_available("componenttabs")){
      $this->AddCheckBox("ShowDevices",
          msgPool::selectToView(  _("network devices")),    
          sprintf(_("Show %s"),   _("network devices")),  true);
    }

    /* Add SubSearch checkbox */
    $this->AddCheckBox(SEPERATOR);
    $this->AddCheckBox("SubSearch",  msgPool::selectToView("","subsearch"), msgPool::selectToView("","subsearch_small"), false);

    /*                  Name                 ,Text                              ,Default  , Connect with alphabet  */
    $this->AddRegex   ("Regex",     _("Display systems matching"),"*" , true);
    $this->AddRegex   ("UserRegex", _("Display systems of user"), "*" , false, "images/lists/search-user.png");
  }

  function GenHeader()
  {
   /* Prepare departments,
       which are shown in the listbox on top of the listbox
     */
    $options= "";

    /* Get all departments within this subtree */
    $base     = $this->config->current['BASE'];
    $options  = $this->create_department_list($this->module);

    /* If there is at least one c (Create) and one w (Write) acl in this combination
        display the snapshot paste icon. BUT check the correct acls in the management plugin */
    $all_module_acls = "";
    $ui = get_userinfo();
    foreach($this->module as $module){
      $all_module_acls .= $ui->has_complete_category_acls($this->selectedBase,$module);
    }

    /* Must we add an additional seperator */
    $add_sep = false;
   
    /* Add default header */
    $listhead = MultiSelectWindow::get_default_header();

    /* Add base selection */
    $listhead .= _("Base")."&nbsp; <select name='CurrentMainBase' onChange='mainform.submit()' class='center'>$options</select>".
      " <input class='center' type='image' src='images/lists/submit.png' align='middle'
      title='"._("Submit department")."' name='submit_department' alt='".           _("Submit")."'>&nbsp;";

    /* Handle create icons */ 
    $s = ".|"._("Actions")."|\n";
    $s.= "..|<img src='images/lists/new.png' alt='' border='0' class='center'>".
      "&nbsp;"._("Create")."|\n";

    if(class_available("termtabs") && preg_match("/c/",$ui->get_permissions($this->selectedBase,"terminal/termgeneric"))){
      $s.= "...|<img src='plugins/systems/images/select_terminal.png' alt='' border='0' class='center'>".
        "&nbsp;"._("Terminal")."|"."newsystem_terminal|\n";
    }
    if(class_available("worktabs") && preg_match("/c/",$ui->get_permissions($this->selectedBase,"workstation/workgeneric"))){
      $s.= "...|<img src='plugins/systems/images/select_workstation.png' alt='' border='0' class='center'>".
        "&nbsp;"._("Workstation")."|"."newsystem_workstation|\n";
    }
    if(class_available("servtabs") && preg_match("/c/",$ui->get_permissions($this->selectedBase,"workstation/servgeneric"))){
      $s.= "...|<img src='plugins/systems/images/select_server.png' alt='' border='0' class='center'>".
        "&nbsp;"._("Server")."|"."newsystem_server|\n";
    }
    if(class_available("phonetabs") && preg_match("/c/",$ui->get_permissions($this->selectedBase,"phone/phoneGeneric"))){
      $s.= "...|<img src='plugins/systems/images/select_phone.png' alt='' border='0' class='center'>".
        "&nbsp;"._("Phone")."|"."newsystem_phone|\n";
    }
    if(class_available("printtabs") && preg_match("/c/",$ui->get_permissions($this->selectedBase,"printer/printgeneric"))){
      $s.= "...|<img src='plugins/systems/images/select_printer.png' alt='' border='0' class='center'>".
        "&nbsp;"._("Printer")."|"."newsystem_printer|\n";
    }
    if(class_available("componenttabs") && preg_match("/c/",$ui->get_permissions($this->selectedBase,"component/componentGeneric"))){
      $s.= "...|<img src='plugins/systems/images/select_component.png' alt='' border='0' class='center'>".
        "&nbsp;"._("Component")."|"."newsystem_component|\n";
    }
    if(class_available("opsiGeneric") && preg_match("/c/",$ui->get_permissions($this->selectedBase,"opsi/opsiGeneric"))){
      $s.= "...|<img src='plugins/systems/images/select_winstation.png' alt='' border='0' class='center'>".
        "&nbsp;"._("Opsi")."|"."newsystem_opsi_client|\n";
    }

    if($this->parent->si_active){
      $events  = DaemonEvent::get_event_types(SYSTEM_EVENT);

      $s.= "..|<img src='images/rocket.png' alt='' border='0' class='center'>".
        "&nbsp;"._("Trigger action")."|\n";
      foreach($events['TRIGGERED'] as $name => $event){
        $s.= "...|".$event['MenuImage']."&nbsp;".$event['s_Menu_Name']."|trigger_event_".$name."\n";
      }

      $s.= "..|<img src='plugins/systems/images/clock.png' alt='' border='0' class='center'>".
        "&nbsp;"._("Schedule action")."|\n";
      foreach($events['SCHEDULED'] as $name =>  $event){
        $s.= "...|".$event['MenuImage']."&nbsp;".$event['s_Menu_Name']."|schedule_event_".$name."\n";
      }
    }

    $s.= "..|<img src='images/lists/trash.png' alt='' border='0' class='center'>".
      "&nbsp;"._("Remove")."|"."remove_multiple|\n";

    $s.= "..|<img src='images/lists/unlocked.png' alt='' border='0' class='center'>".
      "&nbsp;"._("Activate systems")."|activate_multiple|\n";

    /* Add Copy & Paste header */
    $s .= $this->parent->get_copypaste_header($this->selectedBase,$this->module);

    /* Add snapshot icons */
    $s .= $this->parent->get_snapshot_header($this->selectedBase,$this->module);

    $this->SetDropDownHeaderMenu($s);
    $this->SetListHeader($listhead);
  }

  function execute()
  {
    $this->ClearElementsList();
    $this->GenHeader();
  }

  function setEntries($terminals)
  {
    $empty    ="<img src='images/empty.png' style='width:16px ; height: 16px;' class='center' alt=''>";

    /* Dynamic action col, depending on snapshot icons */
    $action_col_size = 120;
    if($this->parent->snapshotEnabled()){
      $action_col_size += 38;
    }

    // User and Template  Images
    $editlink = "<a href='?plug=".$_GET['plug']."&amp;id=%s&amp;act=edit_entry'>%s</a>";

    /* ACL mapping */
    $ui = get_userinfo();
    $tabs = $this->parent->get_tab_defs();

    $cnts = array("component" => 0 , "terminal" => 0 , "workstation" => 0 , 
                  "printer" => 0 , "phone" => 0 , "server" => 0,"opsi_client" => 0,
                  "NewDevice" => 0, "winstation"=> 0);

    /* Get list of all objectGroups and their member and release configuration 
     */
    $ogroups = array();
    if($this->parent->fai_activated){
      $ogroups = get_sub_list("(&(objectClass=gosaGroupOfNames)(FAIclass=*)(member=*))",array("ogroups"),
          get_ou("ogroupou"),$this->config->current['BASE'],array("FAIclass","member","cn"),GL_NO_ACL_CHECK | GL_SUBSEARCH);
      foreach($ogroups as $ogroup){
        for($i = 0 ; $i < $ogroup['member']['count'] ; $i++){
          if(preg_match("/:/",$ogroup['FAIclass'][0])) {
            if(!isset($og_list[$ogroup['member'][$i]])){
              $ogs_release[$ogroup['member'][$i]]['STR'] = ""; 
              $ogs_release[$ogroup['member'][$i]]['CN'] = ""; 
            }
            $ogs_release[$ogroup['member'][$i]]['STR'] .= preg_replace("/^.*:/","",$ogroup['FAIclass'][0]).", ";
            $ogs_release[$ogroup['member'][$i]]['CN']  .= $ogroup['cn'][0].", ";
          }
        }
      }
    }

    // Test Every Entry and generate divlist Array
    foreach($terminals as $key => $val){

      /* Get system type, it is used to fetch the acls for the current object.
          "winstation" acls are stored as 2winworkstation", so we have to map this here */

      /* Map NewDevice acls to workstation acls */
      $type     = $this->parent->get_system_type($val);
      if($type == "NewDevice" || $type == "ArpNewDevice"){
        $type     = "incoming";
      }
      $acl_type = $tabs[$type]['ACLC'];
      $s_acl    = $tabs[$type]['ACL'];
      $category = preg_replace("/\/.*$/","",$s_acl);
      $class    = preg_replace("/^.*\//","",$s_acl);

      /* Get complete category acls */
      $acl_all  = $ui->has_complete_category_acls($val['dn'],$acl_type) ;
  
      /* Get specific generic acls */
      $acl      = $ui->get_permissions($val['dn'],$tabs[$type]['ACL']);

      /* Add copy & cut functionality */
      $action= $this->parent->get_copypaste_action($val['dn'],$category,$class);

      $action.= "<input class='center' type='image' src='images/lists/edit.png' 
                    alt='"._("edit")."'     name='system_edit_%KEY%' title='"._("Edit system")."'>";

      $action.= $this->parent->get_snapshot_action($val['dn'],$acl_type);

      if(preg_match("/d/",$acl)){
        $action.= "<input class='center' type='image' src='images/lists/trash.png'
          alt='"._("delete")."'   name='system_del_%KEY%' title='"._("Delete system")."'>";
      }else{
        $action.= $empty;
      }

      $val['cn'][0]= preg_replace('/\$$/', '', $val['cn'][0]);

      // Generate Array to Add
      $dsc= "";
      if (isset($val['description'][0])){
        $dsc= " [".$val['description'][0]."]";
      }
      if((isset($val['is_new']))&&(!empty($val['is_new']))){
        $display= $val["cn"][0]." ".$val['is_new'].$dsc;
      }else{
        $display= $val["cn"][0].$dsc;
      }

      /* Check if this is a terminal/workstation && if we are allowed to change the gotoRootPasswd */
      $display_key_for = array("terminal","workstation","server","component");
      $pwd_acl = $ui->get_permissions($val['dn'],$tabs[$type]['ACL'],"userPassword");
      if(preg_match("/w/",$pwd_acl) && in_array($type,$display_key_for)){
        $action2 = "<input class='center' type='image' src='images/lists/key.png' alt='"._("Password")."'   name='system_setpwd_%KEY%' title='"._("Set password")."'>";
      }else{
        $action2 = $empty;
      }

      if(in_array("gotoWorkstation",$val['objectClass'])){
        $action2= "<input class='center' type='image' src='plugins/systems/images/cdrom.png' alt='"._("Create CD")."'   
                      name='gen_cd_%KEY%' title='"._("Create FAI CD")."'>&nbsp;".$action2;
      }else{
        $action2= $empty.$action2;
      }

      if(isset($val['message'])){
        $display.= "  (".$val['message'].")";
      }

      $img    = $this->parent->convert_list($val);

      /* Cutted objects should be displayed in light grey */
      if($this->parent->CopyPasteHandler){
        foreach($this->parent->CopyPasteHandler->queue as $queue_key => $queue_data){
          if($queue_data['dn'] == $val['dn']) {
            $display = "<font color='#999999'>".$display."</font>";
            break;
          }
        }
      }

      $type = $this->parent->get_system_type($val);
      if($type == "ArpNewDevice"){
        $type = "NewDevice";
      }
      $cnts[$type] ++;

      /* Create each field */
      $field0 = array("string" => "<input type='checkbox' id='item_selected_".$key."' name='item_selected_".$key."'>" ,
          "attach" => "style='width:20px;'");
      $field1 = array("string" => sprintf($img['img'],$val['dn']),
          "attach" => "style='text-align:center;width:20px;'");
      $field2 = array("string" => sprintf($editlink,$key,$display),
          "attach" => "style='' title='".preg_replace('/ /', '&nbsp;', @LDAP::fix($val['dn']))."'");
      $field3 = array("string" => preg_replace("/%KEY%/", "$key", $action2.$action),
          "attach" => "style='width:".$action_col_size."px;border-right:0px;text-align:right;'");

      if($this->parent->fai_activated){

        $release ="&nbsp;";
        $release_attach = ""; 
        if(isset($val['FAIclass'][0]) && preg_match("/:/",$val['FAIclass'][0])){
          $release        = preg_replace("/^.*:/","",$val['FAIclass'][0]);
          $release_attach = ""; 
        }elseif(isset($ogs_release[$val['dn']])){
          $release = trim($ogs_release[$val['dn']]['STR'],", ").
            "&nbsp;<img src='plugins/ogroups/images/select_ogroup.png' class='center' alt='' 
            title='".sprintf(_("Inherited from %s"),trim($ogs_release[$val['dn']]['CN'],", "))."'>";
          $release_attach = "title='".sprintf(_("Inherited from %s"),trim($ogs_release[$val['dn']]['CN'],", "))."'"; 
        }
        $field2a = array("string" => $release,"attach"=>$release_attach." style='width:100px;'");
        $this->AddElement( array($field0,$field1,$field2,$field2a,$field3));
      }else{
        $this->AddElement( array($field0,$field1,$field2,$field3));
      }
    }

    /* Create summary string for list footer */
    $num_deps=0;
    if(!$this->SubSearch){
      $num_deps = count($this->Added_Departments);
    }

    $num_ser_str = sprintf(_("Number of listed '%s'"),_("servers"));
    $num_ws_str  = sprintf(_("Number of listed '%s'"),_("workstations"));
    $num_ter_str = sprintf(_("Number of listed '%s'"),_("terminals"));
    $num_pho_str = sprintf(_("Number of listed '%s'"),_("phones"));
    $num_prt_str = sprintf(_("Number of listed '%s'"),_("printers"));
    $num_com_str = sprintf(_("Number of listed '%s'"),_("network components"));
    $num_new_str = sprintf(_("Number of listed '%s'"),_("new devices"));
    $num_wws_str = sprintf(_("Number of listed '%s'"),_("windows workstations"));
    $num_dep_str = sprintf(_("Number of listed '%s'"),_("departments"));

    $str = "<img class='center' src='plugins/systems/images/select_server.png'
      title='".$num_ser_str."' alt='".$num_ser_str."'>&nbsp;".$cnts['server']."&nbsp;&nbsp;&nbsp;&nbsp;";
    $str.= "<img class='center' src='plugins/systems/images/select_workstation.png'
      title='".$num_ws_str."' alt='".$num_ws_str."'>&nbsp;".$cnts['workstation']."&nbsp;&nbsp;&nbsp;&nbsp;";
    $str.= "<img class='center' src='plugins/systems/images/select_terminal.png'
      title='".$num_ter_str."' alt='".$num_ter_str."'>&nbsp;".$cnts['terminal']."&nbsp;&nbsp;&nbsp;&nbsp;";
    $str.= "<img class='center' src='plugins/systems/images/select_phone.png'
      title='".$num_pho_str."' alt='".$num_pho_str."'>&nbsp;".$cnts['phone']."&nbsp;&nbsp;&nbsp;&nbsp;";
    $str.= "<img class='center' src='plugins/systems/images/select_printer.png'
      title='".$num_prt_str."' alt='".$num_prt_str."'>&nbsp;".$cnts['printer']."&nbsp;&nbsp;&nbsp;&nbsp;";
    $str.= "<img class='center' src='plugins/systems/images/select_component.png'
      title='".$num_com_str."' alt='".$num_com_str."'>&nbsp;".$cnts['component']."&nbsp;&nbsp;&nbsp;&nbsp;";
    $str.= "<img class='center' src='plugins/systems/images/select_newsystem.png'
      title='".$num_new_str."' alt='".$num_new_str."'>&nbsp;".$cnts['NewDevice']."&nbsp;&nbsp;&nbsp;&nbsp;";
    $str.= "<img class='center' src='plugins/systems/images/select_winstation.png'
      title='".$num_wws_str."' alt='".$num_wws_str."'>&nbsp;".$cnts['winstation']."&nbsp;&nbsp;&nbsp;&nbsp;";
    $str.= "<img class='center' src='images/lists/folder.png'
      title='".$num_dep_str."' alt='".$num_dep_str."'>&nbsp;".$num_deps."&nbsp;&nbsp;&nbsp;&nbsp;";

    $this->set_List_Bottom_Info($str);
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
