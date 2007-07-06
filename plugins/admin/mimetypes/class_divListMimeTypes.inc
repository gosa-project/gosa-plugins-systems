<?php

class divListMimeTypes extends MultiSelectWindow
{
  /* Current base */
  var $selectedBase       = "";
  var $selectedRelease    = "main";
  var $AvailableReleases  = array();
  var $departments        = array();
  var $parent               ;
  var $ui                   ;

  /* Regex */
  var $Regex              = "*";

  /* Subsearch checkbox */
  var $SubSearch;

  /* These vars should be saved too */
  var $SaveAdditionalVars = array("selectedRelease");


  function divListMimeTypes ($config,$parent)
  {
    /* Create divlist and setup */
    MultiSelectWindow::MultiSelectWindow($config, "Mimetypes", "mimetypes");
  
    /* initialize required attributes */ 
    $this->selectedRelease  = "ou=mime,".$_SESSION['CurrentMainBase'];
    $this->parent           = $parent;
    $this->ui               = get_userinfo();
    $this->AvailableReleases= $this->parent->getReleases($this->selectedBase);

    /* Set list strings */
    $this->SetTitle       (_("List of defined mime types"));
    $this->SetSummary     (_("List of defined mime types"));
    $this->SetInformation (_("This menu allows you to add, edit and remove selected mime types. You may want to use the range selector on top of the mime type listbox, when working with a large number of mime types."));

    /* Result page will look like a headpage */
    $this->SetHeadpageMode();
    $this->EnableAplhabet(true);
  
    /* Disable buttonsm */
    $this->EnableCloseButton(false);
    $this->EnableSaveButton (false);

    /* set Page header */
    $action_col_size = 80;
    if($this->parent->snapshotEnabled()){
      $action_col_size += 38;
    }

    /* Toggle all selected / deselected */
    $chk = "<input type='checkbox' id='select_all' name='select_all'
               onClick='toggle_all_(\"^item_selected_[0-9]*$\",\"select_all\");' >";

    /* set Page header */
    $this->AddHeader(array("string"=> $chk,          "attach"=>"style='width:20px;'"));
    $this->AddHeader(array("string" => "&nbsp;",                "attach" => "style='text-align:center;width:20px;'"));
    $this->AddHeader(array("string" => _("Mime type name")." / "._("Department"), "attach" => "style=''"));
    $this->AddHeader(array("string" => _("Actions"),            "attach" => "style='width:".$action_col_size."px;border-right:0px;text-align:right;'"));

    /* Add SubSearch checkbox */
    $this->AddCheckBox("SubSearch",  _("Select to search within subtrees"), _("Search in subtrees"), false);

    /*                  Name                 ,Text                              ,Default  , Connect with alphabet  */
    $this->AddRegex   ("Regex",     _("Display mime types matching"),"*" , true);
  }


  /* This function allows us to add a user defined filter part at position $position*/
  function AddUserBoxToFilter($position)
  {
    $str = "";
    if(($position  == 2) && ($this->parent->IsReleaseManagementActivated($this->config))){
      $smarty = get_smarty();
      $smarty->assign("selectedRelease",$this->selectedRelease);
      $smarty->assign("branchimage","images/branch.png");
      $smarty->assign("releases",$this->AvailableReleases);
      $str = $smarty->fetch(get_template_path('release_select.tpl', TRUE,dirname(__FILE__)));
    }
    return($str);
  }


  /* Create list header, with create / copy & paste etc*/
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
    $tdeps= $ui->get_module_departments($this->module);
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

    /* Get acls */
    $ui       = get_userinfo();
    $acl      = $ui->get_permissions("cn=dummy,ou=mimetypes,".$this->selectedBase,"mimetypes/mimetype");
    $acl_all  = $ui->has_complete_category_acls($this->selectedBase,"mimetypes") ;


    /* If this is true we add an additional seperator. Just look a few lines below */  
    $add_sep = false;
 
    /* Get copy & paste icon */
    $Copy_Paste ="";
    if(preg_match("/(c.*w|w.*c)/",$acl_all) &&  $this->parent->CopyPasteHandler){
      $Copy_Paste = $this->parent->CopyPasteHandler->generatePasteIcon();
      $add_sep = true;
    }

    /* Add default header */
    $listhead = MultiSelectWindow::get_default_header();

    /* Create snapshot icons */
    if(preg_match("/(c.*w|w.*c)/",$acl_all)){
      $listhead .= $this->get_snapshot_header($this->selectedBase);
      $add_sep = true;
    }
    
    /* Add create icon */
    if(preg_match("/c/",$acl)){
      $listhead .= " <input class='center' type='image' align='middle' src='images/list_new_mime.png' alt='"._("new").
        "' title='"._("Create new mime type")."' name='mime_new'>&nbsp;";
      $add_sep = true;
    }
  
    /* Add copy & paste icons */
    $listhead .= $Copy_Paste;

    /* Add a seperator ? */
    if($add_sep) {
      $listhead .= " <img class='center' src='images/list_seperator.png' align='middle' alt='' height='16' width='1'>&nbsp;";
    }
    
    /* And at least add a department selection box */
    $listhead .=  _("Base")."&nbsp;<select name='CurrentMainBase' onChange='mainform.submit()' class='center'>$options</select>".
      " <input class='center' type='image' src='images/list_submit.png' align='middle' 
        title='"._("Submit department")."' name='submit_department' alt='"._("Submit")."'>&nbsp;";

        /* Multiple options */
    $listhead .= "&nbsp;<input class='center' type='image' align='middle' src='images/edittrash.png'
        title='"._("Remove selected mimetype")."' alt='"._("Remove mimetype")."' name='remove_multiple_mimetypes'>&nbsp;";

    /* Add multiple copy & cut icons */
    if(is_object($this->parent->CopyPasteHandler)){
      $listhead .= "&nbsp;<input class='center' type='image' align='middle' src='images/editcopy.png'
        title='"._("Copy selected object")."' alt='"._("Copy object")."' name='multiple_copy_mimetypes'>&nbsp;";
      $listhead .= "&nbsp;<input class='center' type='image' align='middle' src='images/editcut.png'
        title='"._("cut selected object")."' alt='"._("Cut object")."' name='multiple_cut_mimetypes'>&nbsp;";
    }

    $listhead .="</div>";;

    $this->SetListHeader($listhead);
  }


  /* Some basic settings */
  function execute()
  {
    $this->ClearElementsList();
    $this->GenHeader();
  }


  function setEntries($list)
  {
    /********************
      Variable init
     ********************/

    /* Create links */
    $linkopen = "<a href='?plug=".$_GET['plug']."&amp;act=dep_open&amp;dep_id=%s'>%s</a>";
    $editlink = "<a href='?plug=".$_GET['plug']."&amp;id=%s&amp;act=edit_entry'>%s</a>";
    $userimg  = "<img class='center' src='images/select_groups.png' alt='User'    title='%s'>";
    $mimeimg  = "<img class='center' src='images/select_mimetype.png' alt='A'  title='"._("Mime type")."'>";
    $empty    = "<img class='center' src='images/empty.png' style='width:16px;height:16px;' alt=''>";

    /* set Page header */
    $action_col_size = 80;
    if($this->parent->snapshotEnabled()){
      $action_col_size += 38;
    }

    /********************
      Attach objects
     ********************/

    foreach($list as $key => $val){

      $ui       = get_userinfo();
      $acl      = $ui->get_permissions($val['dn'],"mimetypes/mimetype");
      $acl_all  = $ui->has_complete_category_acls($val['dn'],"mimetypes") ;

      /* Create action icons */
      $actions = "";
      if(preg_match("/(c.*w|w.*c)/",$acl_all)){
        $actions .= $this->GetSnapShotActions($val['dn']);
      }

      /* Get copy Paste icons */
      if(($this->parent->CopyPasteHandler) && preg_match("/(c.*w|w.*c)/",$acl_all)){
        $actions.= "<input class='center' type='image'
          src='images/editcut.png' alt='"._("cut")."' name='cut_%KEY%' title='"._("Cut this entry")."'>&nbsp;";
        $actions.= "<input class='center' type='image'
          src='images/editcopy.png' alt='"._("copy")."' name='copy_%KEY%' title='"._("Copy this entry")."'>&nbsp;";
      }

      $actions.= "<input class='center' type='image'
        src='images/edit.png' alt='"._("edit")."' name='mime_edit_%KEY%' title='"._("Edit this entry")."'>";

      /* Add delete button */
      if(preg_match("/d/",$acl)){
        $actions.= "<input class='center' type='image'
          src='images/edittrash.png' alt='"._("delete")."' name='mime_del_%KEY%' title='"._("Delete this entry")."'>";
      }else{
        $actions.= "<img src='images/empty.png' alt='&nbsp;' class='center'>";
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
      $field1 = array("string" => sprintf($mimeimg,$val['dn']), "attach" => "style='text-align:center;width:20px;'");
      $field2 = array("string" => sprintf($editlink,$key,($val['cn']['0'].$desc)), "attach" => "style='' ".$title);
      $field3 = array("string" => preg_replace("/%KEY%/", $key, $actions), "attach" => "style='width:".$action_col_size."px;border-right:0px;text-align:right;'");
      $this->AddElement(array($field0,$field1,$field2,$field3));
    }
  }

  function Save()
  {
    MultiSelectWindow::Save();  
  }

  function save_object()
  {
    /* Save automatic created POSTs like regex, checkboxes */
    MultiSelectWindow::save_object(); 

    /* check if returned selectedRelease is a valid release.
        If it isn't set to a valid release */
    $this->AvailableReleases = $this->parent->getReleases($this->selectedBase);
    if(!isset($this->AvailableReleases[$this->selectedRelease])){
      $this->selectedRelease =key($this->AvailableReleases);
    }
    $_SESSION['mimefilter']['release']  = $this->selectedRelease;
  }

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
