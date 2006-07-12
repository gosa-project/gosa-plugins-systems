<?php

class divListACL extends MultiSelectWindow
{

  /* Current base */
  var $selectedBase       = "";
  var $departments        = array();

  /* Regex */
  var $Regex              = "*";

  /* CheckBoxes */

  /* Subsearch checkbox */
  var $SubSearch              = false;

  var $parent             ;
  var $ui                 ;

  

  function divListACL ($config,$parent)
  {
    MultiSelectWindow::MultiSelectWindow($config,"ACLs");
    
    $this->parent       = $parent;
    $this->ui           = get_userinfo();

    /* Set list strings */
    $this->SetTitle(_("List of acl"));
    $this->SetSummary(_("List of acls"));

    /* Result page will look like a headpage */
    $this->SetHeadpageMode();
    $this->SetInformation(_("This menu allows you to edit and delete selected acls."));

    $this->EnableAplhabet   (true);
  
    /* Disable buttonsm */
    $this->EnableCloseButton(false);
    $this->EnableSaveButton (false);

    /* Dynamic action col, depending on snapshot icons */
    $action_col_size = 80;

    /* set Page header */
    $this->AddHeader(array("string"=>"&nbsp;",          "attach"=>"style='width:20px;'"));
    $this->AddHeader(array("string"=>_("Summary")));
    $this->AddHeader(array("string"=>_("Actions"),      "attach" => "style='width:".$action_col_size."px;border-right:0px;text-align:right;'"));

    /* Set dynamically filter checkboxes */
    $this->set_filter_objectclasses();   
 
    /* Add SubSearch checkbox */
    $this->AddCheckBox(SEPERATOR);
    $this->AddCheckBox("SubSearch",  _("Select to search within subtrees"), _("Ignore subtrees"), false);


    /*                  Name                 ,Text                              ,Default  , Connect with alphabet  */
    $this->AddRegex   ("Regex",     _("Display acls matching"),"Not implemented yet" , true);
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

    /* Create header with selected base */
    $listhead = "<div style='background:#F0F0F9;padding:5px;'>".
      " <input class='center' type='image' src='images/list_root.png' align='middle' 
      title='"._("Go to root department")."' name='dep_root' alt='"._("Root")."'>&nbsp;".
      " <input class='center' type='image' align='middle' src='images/list_back.png' 
      title='"._("Go up one department")."' alt='"._("Up")."'                name='dep_back'>&nbsp;".
      " <input class='center' type='image' align='middle' src='images/list_home.png' 
      title='"._("Go to users department")."' alt='"._("Home")."'            name='dep_home'>&nbsp;".
      " <input class='center' type='image' src='images/list_reload.png' align='middle' 
      title='"._("Reload list")."' name='submit_department' alt='".        _("Submit")."'>&nbsp;".
      " <img   class='center' src='images/list_seperator.png' align='middle' alt='' height='16' width='1'>&nbsp;";
    $listhead .= $this->get_snapshot_header($this->selectedBase);
    $listhead .=  
      _("Base")."&nbsp;<select name='CurrentMainBase' onChange='mainform.submit()' class='center'>$options</select>".
      " <input class='center' type='image' src='images/list_submit.png' align='middle' 
      title='"._("Submit department")."' name='submit_department' alt='".  _("Submit")."'>&nbsp;".
      "</div>";

    $this->SetListHeader($listhead);
  }

  /* so some basic settings */
  function execute()
  {
    $this->ClearElementsList();
    $this->GenHeader();
  }

  function setEntries($list)
  {
    $list_left_icon = "<img src='images/select_acl.png' alt='"._("Acl")."'>";

    foreach($list as $key => $acl_entry){

      $skip = true;
      foreach($this->get_filter_objectclasses() as $oc){
        if(in_array($oc,$acl_entry['objectClass']) && $this->$oc){
          $skip = false;
        }
      }
    
      if(!$skip){
        $action = "<input class='center' type='image' src='images/edit.png' alt='"._("edit")."'     
                    name='acl_edit_%KEY%' title='"._("Edit system")."'>";
        $action.= $this->GetSnapShotActions($acl_entry['dn']);
        $action.= "<input class='center' type='image' src='images/edittrash.png' alt='"._("delete")."' 
                    name='acl_del_%KEY%' title='"._("Delete system")."'>";

        $field1 = array("string" => $list_left_icon, "attach"=> "style='text-align:center;width:20px;'");
        $field2 = array("string" => htmlentities(utf8_decode($acl_entry['dn']))."&nbsp;","attach" => "");
        $field3 = array("string" => preg_replace("/%KEY%/",$key,$action), "attach"=> "style='text-align:center;width:80px;border-right:0px;'");
        $this->AddElement(array($field1,$field2,$field3));
      }
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


  /* Add some additional checkboxes to the filter part, this should be generated dynamically. */
  function set_filter_objectclasses()
  { 
    #FIXME This should be created dynamically 
    $this->gosaAccount = true;
    $this->AddCheckBox("gosaAccount",     _("Select to display users"),       _("Display user")       , true);
    $this->gosaDepartment = true;
    $this->AddCheckBox("gosaDepartment",  _("Select to display departments"), _("Display departments"), true);
  }

  
  /* Return objectClass names used in filter part */  
  function get_filter_objectclasses()
  {
    return(array("gosaAccount","gosaDepartment"));
  }

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
