<?php

class divListMacro extends MultiSelectWindow
{

  /* Current base */
  var $selectedBase       = "";
  var $departments        = array();

  /* Regex */
  var $Regex              = "*";

  /* checkboxes */
  var $ShowSendBocklists      ;
  var $ShowReceiveMacros  ;

  /* Subsearch checkbox */
  var $SubSearch;

  var $parent             ;
  var $ui                 ;

  function divListMacro ($config,$parent)
  {
    MultiSelectWindow::MultiSelectWindow($config,"Macros", "gofonmacro");
    
    $this->parent       = $parent;
    $this->ui           = get_userinfo();

    /* Set list strings */
    $this->SetTitle(_("List of macros"));
    $this->SetSummary(_("List of macros"));

    /* Result page will look like a headpage */
    $this->SetHeadpageMode();
    $this->SetInformation(_("This menu allows you to add, edit and remove selected macros. You may want to use the range selector on top of the macro listbox, when working with a large number of macros."));

    $this->EnableAplhabet   (true);
  
    /* Disable buttonsm */
    $this->EnableCloseButton(false);
    $this->EnableSaveButton (false);

    /* Dynamic action col, depending on snapshot icons */
    $action_col_size = 60;
    if($this->parent->snapshotEnabled()){
      $action_col_size += 20;
    }

    /* Toggle all selected / deselected */
    $chk = "<input type='checkbox' id='select_all' name='select_all'
               onClick='toggle_all_(\"^item_selected_[0-9]*$\",\"select_all\");' >";

    /* set Page header */
    $this->AddHeader(array("string"=> $chk,          "attach"=>"style='width:20px;'"));
    $this->AddHeader(array("string" => "&nbsp;", "attach" => "style='text-align:center;width:20px;'"));
    $this->AddHeader(array("string" => _("Macro name")." / "._("Department"), "attach" => "style=''"));
    $this->AddHeader(array("string" => _("Visible"), "attach" => "style='width:50px;'"));
    $this->AddHeader(array("string" => _("Actions"), "attach" => "style='width:".$action_col_size."px;border-right:0px;text-align:right;'" ));

    /* Add Checkboxes / SubSearch checkbox */
    $this->AddCheckBox("SubSearch",  _("Select to search within subtrees"), _("Ignore subtrees"), false);

    /*                  Name                 ,Text                              ,Default  , Connect with alphabet  */
    $this->AddRegex   ("Regex",     _("Regular expression for matching macro names"),"*" , true);
  }

  function GenHeader()
  {
    /* Prepare departments,
       which are shown in the listbox on top of the listbox
     */
    $options= "";

    /* Get all departments within this subtree */
    $base = $this->config->current['BASE'];
    $first = "";
    $found = FALSE;

    /* Add base */
    $deps[] = array("dn"=>$this->config->current['BASE']);
    $deps= array_merge($deps,get_list("(&(|(ou=*)(description=*))(objectClass=gosaDepartment))", $this->module, $base,
                    array("ou", "description"), GL_SIZELIMIT | GL_SUBSEARCH));

    /* Load possible departments */
    $ui= get_userinfo();
    $tdeps= $ui->get_module_departments("gofonmacro");

    $ids = $this->config->idepartments;

    foreach($deps as $dep){
      if(isset($ids[$dep['dn']]) && in_array_ics($dep['dn'], $tdeps)){

        /* Keep first base dn in mind, we could need this
         *  info if no valid base was found
         */
        if(empty($first)) {
          $first = $dep['dn'];
        }

        $value = $ids[$dep['dn']];
        if ($this->selectedBase == $dep['dn']){
          $options.= "<option selected='selected' value='".$dep['dn']."'>$value</option>";
          $found = TRUE;
        } else {
          $options.= "<option value='".$dep['dn']."'>$value</option>";
        }
      }
    }

    /* The currently used base is not visible with your acl setup.
     * Set base to first useable base.
     */
    if(!$found){
      $this->selectedBase = $first;
    }

    $acls  = $ui->get_permissions($this->selectedBase,"gofonmacro/macro");

    /* Add default header */
    $listhead = MultiSelectWindow::get_default_header();

    if(preg_match("/w/",$acls) && preg_match("/c/",$acls)){
      $listhead .= $this->get_snapshot_header($this->selectedBase);
    }
    if(preg_match("/c/",$acls)){
      $listhead .=  " <input class='center' type='image' align='middle' src='images/list_new_macro.png' 
        title='"._("Create new phone macro")."' alt='"._("New")."' name='goFonMacro_new'>&nbsp;";
      $listhead .= " <img class='center' src='images/list_seperator.png' align='middle' alt='' height='16' width='1'>&nbsp;";
    }
    $listhead .= _("Base")."&nbsp;<select name='CurrentMainBase' onChange='mainform.submit()' class='center'>$options</select>".
      " <input class='center' type='image' src='images/list_submit.png' align='middle' 
      title='"._("Submit department")."' name='submit_department' alt='"._("Submit")."'>&nbsp;";
    
    /* Multiple options */
    $listhead .= "&nbsp;<input class='center' type='image' align='middle' src='images/edittrash.png'
        title='"._("Remove selected macros")."' alt='"._("Remove macros")."' name='remove_multiple_macros'>&nbsp;";

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
  
    $empty      = "<img class='center' src='images/nothing.png' style='width:16px;height:16px;' alt=''>";
    $macroimg   = "<img class='center' src='images/list_macro.png' alt='"._("Macro")."' title='%s'>";
    $visible    = "<img class='center' src='images/true.png' alt='"._("yes")."' title='"._("visible")."'>";
    $invisible  = "<img class='center' src='images/false.png' alt='"._("no")."'title='"._("invisible")."'>";
    $editlink   = "<a href='?plug=".$_GET['plug']."&amp;id=%s&amp;act=edit_entry'>%s</a>";

    /* Dynamic action col, depending on snapshot icons */
    $action_col_size = 60;
    if($this->parent->snapshotEnabled()){
      $action_col_size += 20;
    }

    $ui = get_userinfo();

    foreach($list as $key => $val){
       
      $sacl  = $ui->get_permissions($val['dn'],"gofonmacro/macro");

      $action = "";
      if(!preg_match("/r/",$sacl)){
        continue;
      }

      $action.= "<input class='center' type='image' src='images/edit.png' alt='"._("edit")."'     
        name='goFonMacro_edit_%KEY%' title='"._("Edit macro")."'>";

      if(preg_match("/c/",$sacl) && preg_match("/w/",$sacl)){
        $action.= $this->GetSnapShotActions($val['dn']); 
      }

      if(preg_match("/d/",$sacl)){
        $action.= "<input class='center' type='image' src='images/edittrash.png' alt='"._("delete")."'   
          name='goFonMacro_del_%KEY%' title='"._("Delete macro")."'>";
      }

      if(isset($val['goFonMacroVisible']['0'])&&($val['goFonMacroVisible']['0'] == "1")){
        $pic1 = $visible;
      }else{
        $pic1 = $invisible;
      }

      $display= $val["displayName"][0]."&nbsp;(".$val["cn"][0].")";
      if(isset($val['description'][0])){

        $desc = $val['description'][0];
        if(strlen($desc) > 55){
          $desc = substr($desc,0,52)."...";
        }

        $display .= "&nbsp;[".$desc."]";
      }

      /* Create each field */
      $field0 = array("string" => "<input type='checkbox' id='item_selected_".$key."' name='item_selected_".$key."'>" ,
                      "attach" => "style='width:20px;'");
      $field1 = array("string" => sprintf($macroimg,$val['dn']), "attach" => "style='text-align:center;width:20px;'");
      $field2 = array("string" => sprintf($editlink,$key,$display), "attach" => "style='' title='".preg_replace('/ /', '&nbsp;', @LDAP::fix($val['dn']))."'");
      $field3 = array("string" => $pic1, "attach" => "style='width:50px;'");
      $field4 = array("string" => preg_replace("/%KEY%/", "$key", $action), "attach" => "style='width:".$action_col_size."px;border-right:0px;text-align:right;'");

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
    MultiSelectWindow :: save_object();   
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
