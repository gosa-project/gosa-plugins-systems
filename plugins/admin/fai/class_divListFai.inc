<?php

class divListFai extends MultiSelectWindow
{
  /* Current base */
  var $selectedBase       = "";
  var $selectedBranch     = "main";
  var $AvailableBranches  = array();
  var $departments        = array();

  /* Regex */
  var $Regex              = "*";
  var $ShowProfiles;
  var $ShowTemplates;
  var $ShowScripts;
  var $ShowHooks;
  var $ShowVariables;
  var $ShowPackages;
  var $ShowPartitions;

  /* Subsearch checkbox */
  var $SubSearch;

  var $parent             ;
  var $ui                 ;

  var $SaveAdditionalVars = array("selectedBranch");

  function divListFai ($config,$parent)
  {
    MultiSelectWindow::MultiSelectWindow($config, "Fai", "fai");
    
    $this->parent       = $parent;
    $this->ui           = get_userinfo();

    /* Set default base */
    if(!isset($_SESSION['CurrentMainBase'])){
      $_SESSION['CurrentMainBase'] = $this->config->current['BASE'];
    }
    $this->selectedBase = $_SESSION['CurrentMainBase'];

    /* Set list strings */
    $this->SetTitle(_("List of FAI classes"));
    $this->SetSummary(_("This table displays all FAI classes in the selected tree."));

    /* Result page will look like a headpage */
    $this->SetHeadpageMode();
    $this->SetInformation(_("This menu allows you to create, delete and edit FAI classes."));

    $this->EnableAplhabet   (true);
  
    /* Disable buttonsm */
    $this->EnableCloseButton(false);
    $this->EnableSaveButton (false);

   /* Dynamic action col, depending on snapshot icons */
    $action_col_size = 50;
    if($this->parent->snapshotEnabled()){
      $action_col_size += 20;
    }

    /* set Page header */
    $this->AddHeader(array("string" => "&nbsp;",                "attach" => "style='text-align:center;width:20px;'"));
    $this->AddHeader(array("string" => _("Name of FAI class"),  "attach" => "style=''"));
    $this->AddHeader(array("string" => _("Class type"),         "attach" => "style='width:200px;'"));
    $this->AddHeader(array("string" => _("Actions"),            "attach" => "style='width:".$action_col_size."px;border-right:0px;text-align:right;'"));

    $this->AddCheckBox("ShowProfiles"   , _("Display FAI profile objects")    ,_("Show profiles")     ,true);
    $this->AddCheckBox("ShowTemplates"  , _("Display FAI template objects")   ,_("Show templates")    ,true);
    $this->AddCheckBox("ShowScripts"    , _("Display FAI scripts")            ,_("Show scripts")      ,true);
    $this->AddCheckBox("ShowHooks"      , _("Display FAI hooks")              ,_("Show hooks")        ,true);
    $this->AddCheckBox("ShowVariables"  , _("Display FAI variables")          ,_("Show variables")    ,true);
    $this->AddCheckBox("ShowPackages"   , _("Display FAI packages")           ,_("Show packages")     ,true);
    $this->AddCheckBox("ShowPartitions" , _("Display FAI partitions")         ,_("Show partitions")   ,true);

    /* Add SubSearch checkbox */
    //$this->AddCheckBox(SEPERATOR);
    //$this->AddCheckBox("SubSearch",  _("Select to search within subtrees"), _("Ignore subtrees"), false);

    /*                  Name                 ,Text                              ,Default  , Connect with alphabet  */
    $this->AddRegex   ("Regex",     _("Display users matching"),"*" , true);
  }

  function AddUserBoxToFilter($position){
    $str = "";
    if($position  == 2){
      $smarty = get_smarty();
      $smarty->assign("selectedBranch",$this->selectedBranch);
      $smarty->assign("branchimage","images/branch.png");
      $smarty->assign("branches",$this->AvailableBranches);
      $str = $smarty->fetch(get_template_path('branch_selector.tpl', TRUE));
    }
    return($str);
  }

  function GenHeader()
  {
    $ui = get_userinfo();
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

        /* Create listhead, it will be shown on top of the divlist.
     * It provides general navigation and object creation
     */
    $listhead =
      "<div style='background:#F0F0F9;padding:5px;'>&nbsp;".

      "<input class='center' type='image' src='images/list_root.png' align='middle'
      title='"._("Go to root department")."' name='dep_root' alt='".      _("Root")."'>&nbsp;".

      "<input class='center' type='image' src='images/list_up.png' align='middle'
      title='"._("Go up one department")."' name='dep_back' alt='"._("Up"). "'>&nbsp;".

      "<input class='center' type='image' src='images/list_home.png' align='middle'
      title='"._("Go to users home department")."' name='dep_home' alt='"._("Home")."'>&nbsp;".

      " <input class='center' type='image' src='images/list_reload.png' align='middle' title='"._("Reload list")."' name='submit_department' alt='".
_("Submit")."'>&nbsp;".

      "<img class='center' src=\"images/list_seperator.png\" alt=\"\" align=\"middle\" height=\"16\" width=\"1\">&nbsp;";

    $acl = $ui->get_permissions($this->selectedBase,"fai/faiProfile");
    if(preg_match("/c/",$acl)){
      $listhead .=" <input class='center' type='image' src='images/fai_new_profile.png' align='middle'
        title='"._("New profile")."' name='Create_profile' alt='"._("P")."'>&nbsp;";
    }

    $listhead .= "<img class='center' src=\"images/list_seperator.png\" alt=\"\" align=\"middle\" height=\"16\" width=\"1\">&nbsp;";
    $listhead .= $this->get_snapshot_header($this->selectedBase);


    $arr = array(
        array("images/fai_new_partitionTable.png" , _("New partition table")  ,"Create_partition" , _("PT") , "faiPartitionTable"),
        array("images/fai_new_script.png"         , _("New scripts")          ,"Create_script"    , _("S")  , "faiScript"),
        array("images/fai_new_hook.png"           , _("New hooks")            ,"Create_hook"      , _("H")  , "faiHook"),
        array("images/fai_new_variable.png"       , _("New variables")        ,"Create_variable"  , _("V")  , "faiVariable"),
        array("images/fai_new_template.png"       , _("New templates ")       ,"Create_template"  , _("I")  , "faiTemplate"),
        array("images/fai_new_packages.png"       , _("New package list")     ,"Create_package"   , _("PK") , "faiPackage"));

    foreach($arr as $ar){

      $acl = $ui->get_permissions($this->selectedBase,"fai/".$ar[4]);
      if(preg_match("/c/",$acl)){
        $listhead .=" <input class='center' type='image' src='".$ar[0]."' align='middle' title='".$ar[1]."' name='".$ar[2]."' alt='".$ar[3]."'>&nbsp;";
      }
    }
    $listhead .=" <img class='center' src='images/list_seperator.png' align='middle' alt='' height='16' width='1'>&nbsp;".

      _("Base")."&nbsp;<select name='CurrentMainBase' onChange='mainform.submit()' class='center'>$options</select>".
      " <input class='center' type='image' src='images/list_submit.png' align='middle' title='"._("Submit department")."' name='submit_department' alt='".  _("Submit")."'>&nbsp;".

      "</div>";

    $this->SetListHeader($listhead);
  }

  /* so some basic settings */
  function execute()
  {
    $this->ClearElementsList();
    $this->GenHeader();
    $this->AvailableBranches = $this->parent->getBranches();
  }

  function setEntries($list)
  {
    /********************
      Variable init
     ********************/

     $objects = array(
           "FAIpartitionTable"  => array("IMG"=> "images/fai_partitionTable.png", "NAME"=>"Partition table","KZL"=> "PT", "VAR"=>"ShowPartitions"),
           "FAIpackageList"     => array("IMG"=> "images/fai_packages.png",       "NAME"=>"Package list" ,  "KZL"=> "PL", "VAR"=>"ShowPackages"),
           "FAIscript"          => array("IMG"=> "images/fai_script.png",         "NAME"=>"Scripts" ,       "KZL"=> "S",  "VAR"=>"ShowScripts"),
           "FAIvariable"        => array("IMG"=> "images/fai_variable.png",       "NAME"=>"Variables" ,     "KZL"=> "V",  "VAR"=>"ShowVariables"),
           "FAIhook"            => array("IMG"=> "images/fai_hook.png",           "NAME"=>"Hooks",          "KZL"=> "H",  "VAR"=>"ShowHooks"),
           "FAIprofile"         => array("IMG"=> "images/fai_profile.png",        "NAME"=>"Profile" ,       "KZL"=> "P",  "VAR"=>"ShowProfiles"),
           "FAItemplate"        => array("IMG"=> "images/fai_template.png",       "NAME"=>"Templates" ,     "KZL"=> "T",  "VAR"=>"ShowTemplates"));

    $editlink ="<a href='?plug=".$_GET['plug']."&amp;edit_entry=%KEY%' title='%TITLE%'>%NAME%</a>";

    /* Dynamic action col, depending on snapshot icons */
    $action_col_size = 50;
    if($this->parent->snapshotEnabled()){
      $action_col_size += 20;
    }

    /********************
      Attach objects
     ********************/

     foreach($list as $key => $value){
       $info     = "";
       $img      = "";
       $type     = $value['type'];
       $abort    =false;

       if(isset($objects[$type])){
         $img   = "<img class='center' src='".$objects[$type]['IMG']."' title='".$objects[$type]['NAME']."' alt='".$objects[$type]['KZL']."'>";
         $info  = $objects[$type]['NAME'];
         $var   = $objects[$type]['VAR'];
       }else{
         $img   = "<img class='center' src='images/empty.png' alt=''>";
         $info  = "";
         $var   = "";
       }

       if((isset($value['description']))&&(!empty($value['description']))){
         $desc= " [".$value['description']."]";
       }else{
         $desc= "";
       }


       if($value['FAIstate'] == "freeze"){
         $action=  "<img src='images/closedlock.png' alt='F' class='center'><img src='images/empty.png' width='38' height='2' class='center' alt=''>";
         $action.= "<input class='center' type='image' src='images/edit.png'  alt='"._("edit")."'
           name='entry_edit_%KEY%' title='"._("Edit class")."'>";
       }else{
         $action= "<input class='center' type='image' src='images/edit.png'  alt='"._("edit")."'
           name='entry_edit_%KEY%' title='"._("Edit class")."'>";

         if(preg_match("/(c&w)/",$value['acl'])){
           $action.= $this->GetSnapShotActions($value['dn']);;
         }

         if(preg_match("/d/",$value['acl'])){
           $action.= "<input class='center' type='image' src='images/edittrash.png' alt='"._("delete")."'
             name='entry_delete_%KEY%' title='"._("Delete class")."'>";
         }
       }

       $edi = $editlink;
       $acti = $action;

       $field1 = array("string" => $img , "attach" => "style='text-align:center;width:20px;'");
       $field2 = array("string" => preg_replace(array("/%KEY%/","/%NAME%/","/%TITLE%/"),array($key,$value['cn'].$desc,preg_replace('/ /', '&nbsp;', @LDAP::fix($value['dn']))),$edi) , "attach" => "style=''");
       $field3 = array("string" => $info, "attach" => "style='width:200px;'");
       $field4 = array("string" => preg_replace("/%KEY%/",$key,$acti) , "attach" => "style='width:".$action_col_size."px;border-right:0px;text-align:right;'");
       $this->AddElement(array($field1,$field2,$field3,$field4));
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
    $_SESSION['faifilter']['branch'] = $this->selectedBranch; 
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
