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
  var $action_col_size;
  

  function divListACL ($config,$parent)
  {
    MultiSelectWindow::MultiSelectWindow($config, "ACLs", "acl");
    
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

    /* Dynamic action col */
    $this->action_col_size = 45;

    /* set Page header */
    $this->AddHeader(array("string"=>"&nbsp;",          "attach"=>"style='width:20px;'"));
    $this->AddHeader(array("string"=>_("Summary")));
    $this->AddHeader(array("string"=>_("Actions"),      "attach" => "style='width:".$this->action_col_size."px;border-right:0px;text-align:right;'"));

    /* Set dynamically filter checkboxes */
    $this->set_filter_objectclasses();   
 
    /* Add SubSearch checkbox */
    $this->AddCheckBox("SubSearch",  _("Select to search within subtrees"), _("Ignore subtrees"), false);


    /*                  Name                 ,Text                              ,Default  , Connect with alphabet  */
    $this->AddRegex   ("Regex",     _("Display acls matching"),"*" , true);
  }


  function GenHeader()
  {
     /* Prepare departments,
       which are shown in the listbox on top of the listbox
     */
    $options= "";

    /* Get all departments within this subtree */
    $base = $this->config->current['BASE'];
    $deps= get_list("(&(|(ou=*)(description=*))(objectClass=gosaDepartment))", "acl", $base,
                    array("ou", "description"), GL_SIZELIMIT | GL_SUBSEARCH);

    /* Load possible departments */
    $ui     = get_userinfo();
    $tdeps  = $ui->get_module_departments("acl");
    $ids    = $this->config->idepartments;
    foreach($deps as $dep){
      if(isset($ids[$dep['dn']]) && in_array_ics($dep['dn'], $tdeps)){
        $value = $ids[$dep['dn']];
        if ($this->selectedBase == $dep['dn']){
          $options.= "<option selected='selected' value='".$dep['dn']."'>$value</option>";
        } else {
          $options.= "<option value='".$dep['dn']."'>$value</option>";
        }
      }
    }

    /* Get copy & paste icon */
    $acl      = $ui->get_permissions($this->selectedBase,"acl/aclManagement");

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
    $editlink       = "<a href='?plug=".$_GET['plug']."&amp;id=%s&amp;act=list_edit_entry'>%s</a>";

    foreach($list as $key => $acl_entry){

      /* Get copy & paste icon */
      $acl      = $this->ui->get_permissions($acl_entry['dn'],"acl/aclManagement");

      $action = "";

      /* Add actions depending on acls */
      if(preg_match("/r/", $acl)){
        $action.= "<input class='center' type='image' src='images/edit.png' alt='"._("edit")."'     
          name='list_acl_edit_%KEY%' title='"._("Edit system")."'>";
      }
      if(preg_match("/d/", $acl)){
        $action.= "<input class='center' type='image' src='images/edittrash.png' alt='"._("delete")."' 
          name='list_acl_del_%KEY%' title='"._("Delete system")."'>";
      }

      $field1 = array("string" => $list_left_icon, "attach"=> "style='text-align:center;width:20px;'");
      $field2 = array("string" => sprintf($editlink,$key,htmlentities(utf8_decode($acl_entry['dn'])))."&nbsp;","attach" => "");
      $field3 = array("string" => preg_replace("/%KEY%/",$key,$action), "attach"=> "style='text-align:center;width:".$this->action_col_size."px;border-right:0px;'");
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
    MultiSelectWindow::save_object();   
  }


  /* Add some additional checkboxes to the filter part, this should be generated dynamically. */
  function set_filter_objectclasses()
  { 
  }
  
  /* Return objectClass names used in filter part */  
  function get_filter_objectclasses()
  {
    return(array("gosaAccount","gosaDepartment"));
  }

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
