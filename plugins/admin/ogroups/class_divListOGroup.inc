<?php

class divListOGroup extends MultiSelectWindow
{

  /* Current base */
  var $selectedBase       = "";
  var $departments        = array();

  /* Regex */
  var $Regex              = "*";

  /* CheckBoxes, to change default values modify $this->AddCheckBox */
  var $UserGroups         ;
  var $GroupGroups        ;
  var $ApplicationGroups  ;
  var $DepartmentGroups   ; 
  var $ServerGroups       ;
  var $WorkstationGroups  ;
  var $TerminalGroups     ;
  var $PrinterGroups      ;
  var $PhoneGroups        ;
  

  /* Subsearch checkbox */
  var $SubSearch          ;
  var $parent             ;
  var $ui                 ;

  function divListOGroup ($config,$parent)
  {
    MultiSelectWindow::MultiSelectWindow($config, "OGroups", "ogroups");
    
    $this->parent       = $parent;
    $this->ui           = get_userinfo();

    /* Set list strings */
    $this->SetTitle(_("List of object groups"));
    $this->SetSummary(_("List of object groups"));
    $this->EnableAplhabet(true);

    /* Result page will look like a headpage */
    $this->SetHeadpageMode();
    $this->SetInformation(_("This menu allows you to add, edit or remove selected groups. You may want to use the range selector on top of the group listbox, when working with  a large number of groups."));
  
    /* Disable buttonsm */
    $this->EnableCloseButton(false);
    $this->EnableSaveButton (false);

    /* Dynamic action col, depending on snapshot icons */
    $action_col_size = 80;
    if($this->parent->snapshotEnabled()){
      $action_col_size += 38; 
    }

    /* Toggle all selected / deselected */
    $chk = "<input type='checkbox' id='select_all' name='select_all'
               onClick='toggle_all_(\"^item_selected_[0-9]*$\",\"select_all\");' >";

    /* set Page header */
    $this->AddHeader(array("string"=> $chk,          "attach"=>"style='width:20px;'"));
    $this->AddHeader(array("string" => "&nbsp;", "attach" => "style='text-align:center;width:20px;'"));
    $this->AddHeader(array("string" => _("Name of object groups")." / "._("Departments"), "attach" => "style=''"));
    $this->AddHeader(array("string" => _("Properties"), "attach" => "style='width:136px;'"));
    $this->AddHeader(array("string" => _("Actions"), "attach" => "style='width:".$action_col_size."px;border-right:0px;text-align:right;'"));

    /*                  Text        ,Value    ,Name         ,Is selected */
    $this->AddCheckBox("UserGroups" ,       _("Select to see groups containing users")       , _("Show groups containing users"), true);
    $this->AddCheckBox("GroupGroups" ,      _("Select to see groups containing groups")      , _("Show groups containing groups"), true);
    $this->AddCheckBox("ApplicationGroups", _("Select to see groups containing applications"), _("Show groups containing applications"), true);
    $this->AddCheckBox("DepartmentGroups" , _("Select to see groups containing departments") , _("Show groups containing departments"), true);
    $this->AddCheckBox("ServerGroups" ,     _("Select to see groups containing servers")     , _("Show groups containing servers"), true);
    $this->AddCheckBox("WorkstationGroups", _("Select to see groups containing workstations"), _("Show groups containing workstations"), true);
    $this->AddCheckBox("TerminalGroups" ,   _("Select to see groups containing terminals")   , _("Show groups containing terminals"), true);
    $this->AddCheckBox("PrinterGroups" ,    _("Select to see groups containing printer")     , _("Show groups containing printer"), true);
    $this->AddCheckBox("PhoneGroups" ,      _("Select to see groups containing phones")      , _("Show groups containing phones"), true);

    /* Add SubSearch checkbox */    
    $this->AddCheckBox(SEPERATOR);
    $this->AddCheckBox("SubSearch",  _("Select to search within subtrees"), _("Ignore subtrees"), false);

    /*                  Name                 ,Text                              ,Default  , Connect with alphabet  */
    $this->AddRegex   ("Regex",     _("Regular expression for matching group names"), "*" , true);
  }


  function GenHeader()
  {
    /* Prepare departments,
       which are shown in the listbox on top of the listbox
     */
    $options= "";

    /* Get all departments within this subtree */
    $ui= get_userinfo();
    $first = "";
    $found = FALSE;
    $base = $this->config->current['BASE'];

    /* Add base */
    $tmp = array();
    $tmp[] = array("dn"=>$this->config->current['BASE']);
    $tmp=  array_merge($tmp,get_list("(&(|(ou=*)(description=*))(objectClass=gosaDepartment))", $this->module, $base,
                    array("ou", "description"), GL_SIZELIMIT | GL_SUBSEARCH));

    $deps = array();
    foreach($tmp as $tm){
      $deps[$tm['dn']] = $tm['dn'];
    }

    /* Load possible departments */
    $ui= get_userinfo();
    $tdeps= $ui->get_module_departments("ogroups");
    $ids = $this->config->idepartments;
    $first = "";
    $found = FALSE;
    foreach($ids as $dep => $name){
      if(isset($deps[$dep]) && in_array_ics($dep, $tdeps)){

        /* Keep first base dn in mind, we could need this
         *  info if no valid base was found
         */
        if(empty($first)) {
          $first = $dep['dn'];
        }

        $value = $ids[$dep];
        if ($this->selectedBase == $dep){
          $found = TRUE;
          $options.= "<option selected='selected' value='".$dep."'>$value</option>";
        } else {
          $options.= "<option value='".$dep."'>$value</option>";
        }
      }
    }

    /* The currently used base is not visible with your acl setup.
     * Set base to first useable base.
     */
    if(!$found){
      $this->selectedBase = $first;
    }

    /* Add a seperator after displaying c&p and snapshot icons ? */
    $add_sep = false;
  
    /* Get copy & paste icon */
    $acls  = $ui->get_permissions($this->selectedBase,"ogroups/ogroup");
    $acl_all= $ui->has_complete_category_acls($this->selectedBase,"ogroups");
    $Copy_Paste ="";
    if(preg_match("/(c.*w|w.*c)/",$acl_all) &&  $this->parent->CopyPasteHandler){
      $Copy_Paste = $this->parent->CopyPasteHandler->generatePasteIcon();
      $add_sep = true;
    }

    /* Add default header */
    $listhead = MultiSelectWindow::get_default_header();

    /* Add snapshot restore icons */ 
    if(preg_match("/(c.*w|w.*c)/",$acl_all)){
      $listhead .= $this->get_snapshot_header($this->selectedBase);
      $add_sep = true;
    }

    /* Create new ogroup icon */
    if(preg_match("/c/",$acls)) {
      $listhead .=" <input class='center' type='image' align='middle' src='images/list_new_ogroup.png' 
        title='"._("Create new object group")."' alt='"._("new")."' name='group_new'>";
      $add_sep = true;
    }

    /* Add copy & paste icon */
    $listhead .=  $Copy_Paste;
    
    /* Should we add a seperator here ? */
    if($add_sep){
      $listhead .=" <img class='center' src='images/list_seperator.png' align='middle' alt='' height='16' width='1'>&nbsp;";
    }
  
    /* Add department selector */
    $listhead .= _("Base")."&nbsp;<select name='CurrentMainBase' onChange='mainform.submit()' class='center'>$options</select>".
      " <input class='center' type='image' src='images/list_submit.png' align='middle' 
      title='"._("Submit department")."' name='submit_department' alt='"._("Submit")."'>&nbsp;";

    /* Multiple options */
    $listhead .= "&nbsp;<input class='center' type='image' align='middle' src='images/edittrash.png'
        title='"._("Remove selected object groups")."' alt='"._("Remove object groups")."' name='remove_multiple_ogroups'>&nbsp;";

    $listhead .="</div>";

    $this->SetListHeader($listhead);
  }

  function execute()
  {
    $this->ClearElementsList();
    $this->GenHeader();
  }

  function setEntries($list)
  {
    /* Add Copy & Paste buttons if copy&paste is enabled
     */
    // Defining Links
    $linkopen = "<a href='?plug=".$_GET['plug']."&amp;act=dep_open&amp;dep_id=%s'>%s</a>";
    $editlink = "<a href='?plug=".$_GET['plug']."&amp;id=%s&amp;act=edit_entry'>%s</a>";
    $userimg  = "<img class='center' src='images/select_groups.png' alt='User'    title='%s'>";
    $mailimg  = "<img class='center' src='images/mailto.png'             alt='M'  title='"._("Mail")   ."'>";
    $empty    = "<img class='center' src='images/empty.png' style='width:16px;height:16px;' alt=''>";

    /* Dynamic action col, depending on snapshot icons */
    $action_col_size = 80;
    if($this->parent->snapshotEnabled()){
      $action_col_size += 38; 
    }
  
    $ui = get_userinfo();

    // Assigning ogroups
    foreach($list as $key => $val){

      /* Create action icons */
      $actions= "";
      $acl_all= $ui->has_complete_category_acls($val['dn'],"ogroups");
      if(($this->parent->CopyPasteHandler) && preg_match("/(c.*w|w.*c)/",$acl_all)){
        $actions.= "<input class='center' type='image'
          src='images/editcut.png' alt='"._("cut")."' name='cut_%KEY%' title='"._("Cut this entry")."'>&nbsp;";
        $actions.= "<input class='center' type='image'
          src='images/editcopy.png' alt='"._("copy")."' name='copy_%KEY%' title='"._("Copy this entry")."'>&nbsp;";
      }

      $actions.= "<input class='center' type='image'
        src='images/edit.png' alt='"._("edit")."' name='group_edit_%KEY%' title='"._("Edit this entry")."'>";
    
      if(preg_match("/(c.*w|w.*c)/", $acl_all)){
        $actions.= $this->GetSnapShotActions($val['dn']);
      }

      if(preg_match("/d/",$ui->get_permissions($val['dn'],"ogroups/ogroup"))) {
        $actions.= "<input class='center' type='image'
          src='images/edittrash.png' alt='"._("delete")."' name='group_del_%KEY%' title='"._("Delete this entry")."'>";
      }else{
        $actions.= "<img class='center' src='images/empty.png' alt='&nbsp;'
          title='"._("You are not allowed to remove this entry.")."'>";
      }


      if(isset($val['mail'])){
        $mail = $mailimg;
      }else{
        $mail = $empty;
      }
      $title = "title='".preg_replace('/ /', '&nbsp;', @LDAP::fix($val['dn']))."'";
      if(!isset($val['description'][0])){
        $desc = "";
      }else{
        $desc = " - [ ".$val['description'][0]." ]";
      }
      /* Create each field */
      $field0 = array("string" => "<input type='checkbox' id='item_selected_".$key."' name='item_selected_".$key."'>" ,
                      "attach" => "style='width:20px;'");
      $field1 = array("string" => "<img src='images/list_ogroup.png' alt='"._("Object group")."' ".$title.">", "attach" => "style='text-align:center;width: 20px;'");
      $field2 = array("string" => sprintf($editlink,$key,($val['cn']['0'].$desc)), "attach" => "style='' ".$title);
      $field3 = array("string" => preg_replace("/%KEY/", $key, $this->parent->convert_list($val))."&nbsp;".$mail, "attach" => "style='width:136px;'");
      $field4 = array("string" => preg_replace("/%KEY%/", $key, $actions), "attach" => "style='width:".$action_col_size."px;border-right:0px;text-align:right;'");

      $this->AddElement(array($field0,$field1,$field2,$field3,$field4));
    }
  }

  function Save()
  {
    MultiSelectWindow :: Save();  
  }

  function save_object()
  {
    /* Save automatic created POSTs like regex, checkboxes */
    MultiSelectWindow::save_object();   
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
