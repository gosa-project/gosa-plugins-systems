<?php

class selectPrinterDialog extends plugin
{
  /* CLI vars */
  var $cli_summary          = "Manage server basic objects";
  var $cli_description      = "Some longer text\nfor help";
  var $cli_parameters       = array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account       = TRUE;
  var $attributes           = array();
  var $objectclasses        = array("whatever");
  var $AlreadyAssigned      = array();  
  var $regex                = "*";
  var $depselect            = "/";
  var $deplist              = array("/");
  var $module               = array("printer");
  var $ui                   = NULL;

  function selectPrinterDialog ($config, $dn= NULL,$alreadyused=array() )
  {
    $this->AlreadyAssigned = $alreadyused;
    plugin::plugin ($config, $dn);

    /* Get all departments within this subtree */
    $base = $this->config->current['BASE'];
    $deps= get_list("(&(|(ou=*)(description=*))(objectClass=gosaDepartment))", $this->module, $base,
                    array("ou", "description"), GL_SIZELIMIT | GL_SUBSEARCH);

    $tmp =array();
    foreach($deps as $dep){
      $tmp[$dep['dn']] = $dep;
    }
    $deps = $tmp;

    /* Load possible departments */
    $this->ui= get_userinfo();
    $department = array();
    foreach($this->module as $module){

      $d = $this->ui->get_module_departments($module);
      foreach($d as $department){
        $departments[$department] = $department;
      }
    }
    $ids = $this->config->idepartments;
    $this->deplist = array();
    foreach($deps as $dep){
      if(isset($ids[$dep['dn']]) && in_array_ics($dep['dn'], $departments)){
        $this->deplist[$dep['dn']] =  $ids[$dep['dn']];
      }
    }

    $this->depselect = key($this->deplist);
  }

  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    /* Fill templating stuff */
    $smarty= get_smarty();
    $display= "";

    if(isset($_POST['dialogissubmitted'])){
      foreach(array('regexPrinter' => 'regex','depselectPrinter'=>'depselect') as $attr => $name){
        if(isset($_POST[$attr])){
          $this->$name      =$_POST[$attr];
        }
      }
    }

    if((isset($_GET['search']))&&(!empty($_GET['search']))){
      $this->regex=$_GET['search']."*";
      $this->regex=preg_replace("/\*\*/","*",$this->regex);
    }

    $smarty->assign("regexPrinter"    ,$this->regex);
    $smarty->assign("deplistPrinter"  ,$this->deplist);
    $smarty->assign("depselectPrinter",$this->depselect);
    $smarty->assign("gotoPrinters",$this->getPrinter());
    $smarty->assign("gotoPrinterKeys",array_flip($this->getPrinter()));
    $smarty->assign("apply", apply_filter());
    $smarty->assign("alphabet", generate_alphabet());
    $smarty->assign("search_image", get_template_path('images/search.png'));
    $smarty->assign("tree_image", get_template_path('images/tree.png'));
    $smarty->assign("infoimage", get_template_path('images/info.png'));
    $smarty->assign("launchimage", get_template_path('images/small_filter.png'));
    $smarty->assign("deplist", $this->config->idepartments);

    $display.= $smarty->fetch(get_template_path('selectPrinterDialog.tpl', TRUE,dirname(__FILE__)));
    return($display);
  }

  function check(){
    /* Call common method to give check the hook */
    $message= plugin::check();

    if(empty($_POST['gotoPrinter'])){
      $message[] = _("Please select a printer or press cancel.");
    }
    return $message;
  }

  /* Save to LDAP */
  function save()
  {
    return($_POST['gotoPrinter']);
  }

  /* This function generates the Printerlist
   * All printers are returned that match regex and and depselect
   */
  function getPrinter($detailed = false)
  {
    $a_return=array();


    $filter = "(&(objectClass=gotoPrinter)(cn=".$this->regex."))";
    $module = $this->module;
    $base   = $this->depselect;
    $attrs  = array("cn","description");
    $res    = get_list($filter,$module,$base,$attrs);

    foreach($res as $printer)  {

      $acl = $this->ui->get_permissions($printer['dn'],"printer","gotoUserPrinter");
      if(!preg_match("/w/",$acl)){
        continue;
      }

      if(isset($this->AlreadyAssigned[$printer['cn'][0]])) continue;

      if($detailed ==true){
        $a_return[$printer['cn'][0]] = $printer;
      }else{
        if(isset($printer['description'][0])){
          $a_return[$printer['cn'][0]] = $printer['cn'][0]." - ".$printer['description'][0];  
        }else{
          $a_return[$printer['cn'][0]] = $printer['cn'][0];  
        }
      }
    }
    return($a_return);
  }
}





// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
