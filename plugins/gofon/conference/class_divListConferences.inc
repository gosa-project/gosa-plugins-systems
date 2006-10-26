<?php

class divListConference extends MultiSelectWindow
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

  function divListConference ($config,$parent)
  {
    MultiSelectWindow::MultiSelectWindow($config,"Conferences", "gofon");
    
    $this->parent       = $parent;
    $this->ui           = get_userinfo();

    /* Set list strings */
    $this->SetTitle(_("List of conference rooms"));
    $this->SetSummary(_("List of conference rooms"));

    /* Result page will look like a headpage */
    $this->SetHeadpageMode();
    $this->SetInformation(_("This menu allows you to create, delete and edit selected phone conferences. Having a large number of phone conferences, you might prefer the range selectors on top of the conferences list."));

    $this->EnableAplhabet(true);
  
    /* Disable buttonsm */
    $this->EnableCloseButton(false);
    $this->EnableSaveButton (false);

    /* Dynamic action col, depending on snapshot icons */
    $action_col_size = 50;
    if($this->parent->snapshotEnabled()){
      $action_col_size += 20;
    }

    /* set Page header */
    $this->AddHeader(array("string" => "&nbsp;", "attach" => "style='text-align:center;width:20px;'"));
    $this->AddHeader(array("string" =>_("Name - Number"), "attach" => "style=''"));
    $this->AddHeader(array("string" => _("Owner"), "attach" => "style='width:200px;'"));
    $this->AddHeader(array("string" => _("PIN"), "attach" => "style='width:50px;'"));
    $this->AddHeader(array("string" =>_("Actions"), "attach" => "style='width:".$action_col_size."px;border-right:0px;text-align:right;'"));

    /* Add Checkboxes / SubSearch checkbox */
    $this->AddCheckBox("SubSearch",  _("Select to search within subtrees"), _("Ignore subtrees"), false);

    /*                  Name                 ,Text                              ,Default  , Connect with alphabet  */
    $this->AddRegex   ("Regex",      _("Regular expression for matching conference names"),"*" , true);
  }

  function GenHeader()
  {
    /* Prepare departments,
       which are shown in the listbox on top of the listbox
     */
    $options= "";

    /* Get all departments within this subtree */
    $base = $this->config->current['BASE'];
    $deps= get_list("(&(|(ou=*)(description=*))(objectClass=gosaDepartment))", $this->module, $base,
                    array("ou", "description"), GL_SIZELIMIT | GL_SUBSEARCH);

    /* Load possible departments */
    $ui= get_userinfo();
    $tdeps= $ui->get_module_departments("gofon");
    $ids = $this->config->idepartments;

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

    $acls = $ui->get_permissions($this->selectedBase,"gofon/conference");

    /* NEW LIST MANAGMENT */
    $listhead = "<div style='background:#F0F0F9;padding:5px;'>".
      " <input class='center' type='image' src='images/list_root.png' align='middle' 
      title='"._("Go to root department")."' name='dep_root' alt='".         _("Root")."'>&nbsp;".
      " <input class='center' type='image' align='middle' src='images/list_back.png' 
      title='"._("Go up one department")."' alt='"._("Up")."'                name='dep_back'>&nbsp;".
      " <input class='center' type='image' align='middle' src='images/list_home.png' 
      title='"._("Go to users department")."' alt='"._("Home")."'            name='dep_home'>&nbsp;".
      " <input class='center' type='image' src='images/list_reload.png' align='middle' 
      title='"._("Reload list")."' name='submit_department' alt='".        _("Submit")."'>&nbsp;";

    /* Only display create new conference if we are allowed to create new confe... */
    if(preg_match("/c/",$acls)){
      $listhead .=" <img class='center' src='images/list_seperator.png' align='middle' alt='' height='16' width='1'>&nbsp;".
        " <input class='center' type='image' align='middle' src='images/list_new_conference.png' 
        title='"._("Create new conference")."' alt='"._("New conference")."' name='user_new'>&nbsp;".
        " <img class='center' src='images/list_seperator.png' align='middle' alt='' height='16' width='1'>&nbsp;";
    }

    /* Only display snapshot settings if we are able to write && create new entries */
    if(preg_match("/[cw]/",$acls)){
      $listhead .= $this->get_snapshot_header($this->selectedBase);
    }
  
    /* Add the rest (base select ....)*/
    $listhead .= _("Base")."&nbsp;<select name='CurrentMainBase' onChange='mainform.submit()' class='center'>$options</select>".
      " <input class='center' type='image' src='images/list_submit.png' align='middle' 
      title='"._("Submit department")."' name='submit_department' alt='".  _("Submit").     "'>&nbsp;".
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
    $linkopen= "<a href='?plug=".$_GET['plug']."&amp;act=dep_open&amp;dep_id=%s'>%s</a>";

    $userimg  = "<img class='center' src='images/select_conference.png' alt='User' title='%s'>";
    $editlink = "<a href='?plug=".$_GET['plug']."&amp;id=%s&amp;act=edit_entry'>%s</a>";

    /* Dynamic action col, depending on snapshot icons */
    $action_col_size = 50;
    if($this->parent->snapshotEnabled()){
      $action_col_size += 20;
    }

    /* Insert conferneces*/
    foreach($list as $conferencekey => $conference ){

      $acl = $this->ui -> get_permissions($conference['dn'],"gofon/conference");

      /* You will need at least read access for the 
          current conference informations to display it */  
      if(!preg_match("/r/",$acl)){
        continue;
      }

      /* Add edit icon - This is allowed when we have at least read access. */
      $actions= "<input class='center' type='image' src='images/edit.png'     
                    alt='"._("edit")."'    name='conf_edit_%KEY%' title='"._("Edit this entry")."'>";

      /* Add snapshot icon - This is allowed when we have create and write access for the current entry */
      if(preg_match("/[cw]/",$acl)){
        $actions.= $this->GetSnapShotActions($conference['dn']);
      }

      /* Create delete link - Only if we are allowed to delete this entry */
      if(preg_match("/d/",$acl)){
        $actions.= "<input class='center' type='image' src='images/edittrash.png' 
          alt='"._("delete")."'   name='conf_del_%KEY%'  title='"._("Delete this entry")."'>";
      }

      /* Display PIN icon, only if we are the owner of the given conference */
      $owner = $this->parent->ui->dn==$conference['goFonConferenceOwner'][0];
      $p_acl = $this->ui->get_permissions($conference['dn'],"gofon/conference","goFonPIN");
      if((isset($conference['goFonPIN'][0])) && $owner && preg_match("/r/",$p_acl)){
        $pin = "<img class='center' src='images/list_password.png' title='PIN : ".$conference['goFonPIN'][0]."' alt='PIN'>";
      }else{
        $pin = "&nbsp;";
      }

      /* Display numer of the conference if we are allowed to view it */
      $n_acl = $this->ui->get_permissions($conference['dn'],"gofon/conference","telephoneNumber");
      if(isset($conference['telephoneNumber'][0]) && preg_match("/r/",$n_acl)){
        $number = " - ".$conference['telephoneNumber'][0];
      }else{
        $number = "&nbsp; - ?";
      }

      /* Get conference owner name */
      $ldap= $this->parent->config->get_ldap_link();
      $ldap->cat($conference['goFonConferenceOwner'][0], array('cn'));
      $data = $ldap->fetch();
      if(isset($data['cn'][0])){
        $cn = $data['cn'][0];
      }else{
        $cn = _("Unknown"); 
      }  

      /* Create title */
      $title = " title='".preg_replace("/ /","&nbsp;",@LDAP::fix($data['dn']))."' ";

      $a_field1 = array("string"=>sprintf($userimg,_("Conference")), "attach" => $title." style='text-align:center;width:20px;'");
      $a_field2 = array("string"=>sprintf($editlink,$conferencekey,$conference['cn'][0].$number), "attach" => $title." style=''");
      $a_field3 = array("string"=> $cn , "attach" =>  $title." style='width:200px;'");
      $a_field4 = array("string"=> $pin, "attach" =>  $title." style='width:50px;'");
      $a_field5 = array("string"=> preg_replace("/%KEY%/",$conferencekey,$actions), 
                        "attach"=> $title."style='width:52px;border-right:0px;text-align:right;'");

      $this->AddElement(array($a_field1,$a_field2,$a_field3,$a_field4,$a_field5));
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
