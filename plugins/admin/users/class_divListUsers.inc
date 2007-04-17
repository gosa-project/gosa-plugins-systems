<?php

class divListUsers extends MultiSelectWindow
{

  /* Current base */
  var $selectedBase       = "";
  var $departments        = array();

  /* Regex */
  var $Regex              = "*";

  /* CheckBoxes */
  var $ShowTemplates ;
  var $ShowFunctionalUsers;
  var $ShowUnixUsers;
  var $ShowMailUsers;
  var $ShowSambaUsers;
  var $ShowProxyUsers;

  /* Subsearch checkbox */
  var $SubSearch              = false;

  var $parent             ;
  var $ui                 ;

  function divListUsers ($config,$parent)
  {
    MultiSelectWindow::MultiSelectWindow($config, "Users", "users");
    
    $this->parent       = $parent;
    $this->ui           = get_userinfo();

    /* Set list strings */
    $this->SetTitle(_("List of users"));
    $this->SetSummary(_("List of users"));

    /* Result page will look like a headpage */
    $this->SetHeadpageMode();
    $this->SetInformation(_("This menu allows you to create, edit and delete selected users. Having a great number of users, you may want to use the range selectors on top of the user list."));

    $this->EnableAplhabet   (true);
  
    /* Disable buttonsm */
    $this->EnableCloseButton(false);
    $this->EnableSaveButton (false);

    /* Dynamic action col, depending on snapshot icons */
    $action_col_size = 104;
    if($this->parent->snapshotEnabled()){
      $action_col_size += 38;
    }

    /* Toggle all selected / deselected */
    $chk = "<input type='checkbox' id='select_all' name='select_all' 
               onClick='toggle_all_(\"^item_selected_[0-9]*$\",\"select_all\");' >";

    /* set Page header */
    $this->AddHeader(array("string"=> $chk,          "attach"=>"style='width:20px;'"));
    $this->AddHeader(array("string"=>_("Username")." / "._("Department")));
    $this->AddHeader(array("string"=>_("Properties"),   "attach" => "style='width:166px;'"));
    $this->AddHeader(array("string"=>_("Actions"),      "attach" => "style='width:".$action_col_size."px;border-right:0px;text-align:right;'"));
    
    $this->AddCheckBox("ShowTemplates"      ,_("Select to see template pseudo users")               ,_("Show templates")        , false);
    $this->AddCheckBox("ShowFunctionalUsers",_("Select to see users that have only a GOsa object"),_("Show functional users") , true);
    $this->AddCheckBox("ShowUnixUsers"      ,_("Select to see users that have posix settings")      ,_("Show unix users")       , true);
    $this->AddCheckBox("ShowMailUsers"      ,_("Select to see users that have mail settings")       ,_("Show mail users")       , true);
    $this->AddCheckBox("ShowSambaUsers"     ,_("Select to see users that have samba settings")      ,_("Show samba users")      , true);
    $this->AddCheckBox("ShowProxyUsers"     ,_("Select to see users that have proxy settings")      ,_("Show proxy users")      , true);

    /* Add SubSearch checkbox */
    $this->AddCheckBox(SEPERATOR);
    $this->AddCheckBox("SubSearch",  _("Select to search within subtrees"), _("Ignore subtrees"), false);

    /*                  Name                 ,Text                              ,Default  , Connect with alphabet  */
    $this->AddRegex   ("Regex",     _("Display users matching"),"*" , true);
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
    $tdeps= $ui->get_module_departments("users");
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

    /* Get copy & paste icon */
    $acl_all  = $ui->has_complete_category_acls($this->selectedBase,"users") ;
    $acl      = $ui->get_permissions($this->selectedBase,"users/user");
    if(preg_match("/(c.*w|w.*c)/",$acl_all) &&  $this->parent->CopyPasteHandler){
      $Copy_Paste = $this->parent->CopyPasteHandler->generatePasteIcon();
    }else{
      $Copy_Paste ="";
    }

    /* Add default header */
    $listhead = MultiSelectWindow::get_default_header();
    
    if(preg_match("/(c.*w|w.*c)/",$acl_all)){ 
      $listhead .= $this->get_snapshot_header($this->selectedBase);
    }

    if(preg_match("/c/",$acl)) {
      $listhead .= " <input class='center' type='image' align='middle' src='images/list_new_user.png' 
        title='"._("Create new user")."' alt='"._("New user")."' name='user_new'>&nbsp;";
      $listhead .= " <input class='center' type='image' align='middle' src='images/list_new.png' 
        title='"._("Create new template")."' alt='"._("New template")."'        name='user_tplnew'>&nbsp;";
    }

    $listhead .= $Copy_Paste;
    
    $listhead .=
      _("Base")."&nbsp;<select name='CurrentMainBase' onChange='mainform.submit()' class='center'>$options</select>".
      " <input class='center' type='image' src='images/list_submit.png' align='middle' 
      title='"._("Submit department")."' name='submit_department' alt='".  _("Submit")."'>&nbsp;";

  
    /* Multiple options */ 
    $listhead .= "&nbsp;<input class='center' type='image' align='middle' src='images/edittrash.png'
        title='"._("Remove selected user")."' alt='"._("Remove user")."' name='remove_multiple_user'>&nbsp;";
    
    $listhead .="</div>";;
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
    /********************
      Variable init
     ********************/

    /* Variable initialation */
    $enviro     = $posix = $maila = $faxac = $samba = $netatalk = "";
    $empty      = "<img class='center' src='images/empty.png' style='width:16px;height:16px;' alt='-'>";
    $userimg    = "<img class='center' src='images/select_user.png' alt='User' title='%s'>";
    $tplimg     = "<img class='center' src='images/select_template.png' alt='Template' title='%s'>";
    $editlink   = "<a href='?plug=".validate($_GET['plug'])."&amp;id=%s&amp;act=edit_entry'>%s</a>";

    /* Dynamic action col, depending on snapshot icons */
    $action_col_size = 104;
    if($this->parent->snapshotEnabled()){
      $action_col_size += 38;
    }

    /* Possilbe objectClass image combinations */  
    $possibleAccounts = array(
            "posixAccount"    => array("VAR"=>"posix"     ,"IMG"=>"posiximg"),
            "gotoEnvironment" => array("VAR"=>"enviro"    ,"IMG"=>"eviroimg"),
            "gosaMailAccount" => array("VAR"=>"maila"     ,"IMG"=>"mailimg"),
            "goFaxAccount"    => array("VAR"=>"faxac"     ,"IMG"=>"faximg"),
            "sambaSamAccount" => array("VAR"=>"samba"     ,"IMG"=>"sambaimg"),
            "apple-user"      => array("VAR"=>"netatalk"  ,"IMG"=>"netatalkimg"));

     /* Pictures for Extensions */
    $usrimg   ="<input class='center' type='image' src='images/select_user.png' alt='"._("GOsa")."'
                    name='user_edit_%KEY%-user' title='"._("Edit generic properties")."'>";
    $posiximg = "<input class='center' type='image' src='images/penguin.png' alt='"._("Posix")."'
                    name='user_edit_%KEY%-posixAccount' title='"._("Edit UNIX properties")."'>";
    $eviroimg = "<input class='center' type='image' src='images/smallenv.png' alt='"._("Environment")."'
                    name='user_edit_%KEY%-environment' title='"._("Edit environment properties")."'>";
    $mailimg  = "<input class='center' type='image' src='images/mailto.png' alt='"._("Mail")."'
                    name='user_edit_%KEY%-mailAccount' title='"._("Edit mail properties")."'>";
    $fonimg   = "<input class='center' type='image' src='images/%image%' alt='"._("Phone")."'
                    name='user_edit_%KEY%-phoneAccount' title='"._("Edit phone properties")."%title%'>";
    $faximg   = "<input class='center' type='image' src='images/fax_small.png' alt='"._("Fax")."'
                    name='user_edit_%KEY%-gofaxAccount' title='"._("Edit fax properies")."'>";
    $sambaimg = "<input class='center' type='image' src='images/select_winstation.png' alt='"._("Samba")."'
                    name='user_edit_%KEY%-sambaAccount' title='"._("Edit samba properties")."'>";
    $netatalkimg = "<input class='center' type='image' src='images/select_netatalk.png' alt='"._("Netatalk")."'
                    name='user_edit_%KEY%-netatalk' title='"._("Edit netatalk properties")."'>";
    $tplcreateuserimg  = "<input type='image' class='center' src='images/list_new.png' alt='"._("Create user from template")."'
                    name='userfrom_tpl_%KEY%' title='"._("Create user with this template")."'>";

    /********************
      END :: Variable init
     ********************/

 
    /********************
      Append entries to divlist 
     ********************/
 
    $ui = get_userinfo(); 
  
    // Test Every Entry and generate divlist Array
    foreach($list as $key => $val){

      /* Create action icons */
      $action= "";

      /* Add copy & cut icons */     
      $acl_all  = $ui->has_complete_category_acls($this->selectedBase,"users") ;
      $acl      = $ui->get_permissions($val['dn'],"users/user");
      if(preg_match("/(c.*w|w.*c)/",$acl_all) && $this->parent->CopyPasteHandler){
        $action .= "<input class='center' type='image'
          src='images/editcut.png' alt='"._("cut")."' name='cut_%KEY%' title='"._("Cut this entry")."'>&nbsp;";
        $action.= "<input class='center' type='image'
          src='images/editcopy.png' alt='"._("copy")."' name='copy_%KEY%' title='"._("Copy this entry")."'>&nbsp;";
      }

      /* Generate edit icon */
      $action.= "<input class='center' type='image' src='images/edit.png' alt='"._("edit")."'
        name='user_edit_%KEY%' title='"._("Edit user")."'>";

      /* Add Password change icon, if we are able to write users/password */
      if(!in_array_ics("gosaUserTemplate",$val['objectClass']) && preg_match("/w/",$ui->get_permissions($val['dn'],"users/password"))) {
        $action.= "<input class='center' type='image' src='images/list_password.png' alt='"._("password")."'
          name='user_chgpw_%KEY%' title='"._("Change password")."'>";
      }else{
        $action.= "<img class='center' src='images/empty.png' alt='&nbsp;' 
          title='"._("You are not allowed to change the password for this user.")."'>";
      }

      /* Add snapshot icon */
      if(preg_match("/(c.*w|w.*c)/",$acl_all)){
        $action.= $this->GetSnapShotActions($val['dn']);
      }

      /* Add remove icon, if we are allowed to remove the current user */
      if(preg_match("/d/",$ui->get_permissions($val['dn'],"users/user"))) {
        $action.= "<input class='center' type='image' src='images/edittrash.png' alt='"._("delete")."'
          name='user_del_%KEY%' title='"._("Delete user")."'>";
      }else{
        $action.= "<img class='center' src='images/empty.png' alt='&nbsp;'  
          title='"._("You are not allowed to remove this user.")."'>";
      }

      /* Create phonaccopunt informationm, if conencted && is phoneAccount */ 
      if((in_array("goFonAccount"    ,$val['objectClass']))){

        /* Set defaults */
        $fonac = preg_replace("/%image%/", "select_phone.png", $fonimg);
        $fonac = preg_replace("/%KEY%/", "$key", $fonac);
        $fonac = preg_replace("/%title%/", "", $fonac);

      }else{
        $fonac=$empty;
      }

      /* Set images for different types of objectClasses */
      foreach($possibleAccounts as $objectClass => $Settigns){ 
        if(in_array($objectClass ,$val['objectClass'])){
          $$Settigns['VAR'] = preg_replace("/%KEY%/", "$key", $$Settigns['IMG']);
        }else{
          $$Settigns['VAR'] = $empty;
        }
      }

      /* Create userimg */
      if(in_array("gosaUserTemplate",$val['objectClass'])){
        $tpl                          = preg_replace("/%KEY%/", "$key", $tplimg);
        $s_img_create_from_template   = preg_replace("/%KEY%/", "$key", $tplcreateuserimg);
      }else{
        $s_img_create_from_template   = "";
        $tpl                          = $userimg;
      }

      /* Insert key into userimg */
      $usrimg2 = preg_replace("/%KEY%/", "$key", $usrimg);

      // Generate caption for rows
      if (isset($val["sn"]) && isset($val["givenName"])){
        $display= $val["sn"][0].", ".$val["givenName"][0]." [".$val["uid"][0]."]";
      } else {
        $display= "[".$val["uid"][0]."]";
      }

      /* Connect all images */
      $UseImg = $usrimg2."&nbsp;".$posix."&nbsp;".$enviro."&nbsp;".$maila."&nbsp;".$fonac."&nbsp;".$faxac."&nbsp;".$samba."&nbsp;".$netatalk;

      /* Create each field */
      $field0 = array("string" => "<input type='checkbox' id='item_selected_".$key."' name='item_selected_".$key."' >");
      $field1 = array("string" => sprintf($tpl,$val['dn']), "attach" => "style='text-align:center;width:20px;'");
      $field2 = array("string" => sprintf($editlink,$key,$display), "attach" => "style='' title='".preg_replace('/ /', '&nbsp;', @LDAP::fix($val['dn']))."'");
      $field3 = array("string" => $UseImg, "attach" => "style='width:166px;'");
      $field4 = array("string" => $s_img_create_from_template.preg_replace("/%KEY%/", "$key", $action),
                      "attach" => "style='width:".$action_col_size."px;border-right:0px;    text-align:right;'");
      /* Add to list */
      $add = array($field0,$field1,$field2,$field3,$field4);
      $this->AddElement($add);

      // Template or User
      if(in_array("gosaUserTemplate",$val['objectClass'])){
        $tpls[strtolower( $val['sn']['0'].$val['uid']['0'])]=$add;
      }else{
        $users[strtolower( $val['sn']['0'].$val['uid']['0'])]=$add;
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
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
