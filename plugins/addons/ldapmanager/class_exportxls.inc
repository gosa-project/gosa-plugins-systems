<?php


class xlsexport extends plugin
{
  /* Definitions */
  var $plHeadline= "XLS import";
  var $plDescription= "This does something";
  var $access= "";

  /* attribute list for save action */
  var $attributes= array();
  var $objectclasses= array();
  var $ui;

  function xlsexport ($config, $dn= NULL)
  {
    /* Include config object */
    $this->config= $config;

    $this->ui = get_userinfo();
  }

  function execute()
  {
	/* Call parent execute */
	plugin::execute();

    /* Set government mode */
    $smarty= get_smarty();

    $smarty->assign("type",FALSE);
    $smarty->assign("depselectivbb","");
    
    /* Check permissions for export */
    // fill in old vars in the Export form
    if((isset($_POST['single']))&&(isset($_POST['sfrmgetsingle']))){
      $smarty->assign("single",$_POST['single']);
      $smarty->assign("type","single");
      $a=$_POST['single'];
      $b=$_POST['selfull'];

    }
    if((isset($_POST['selfull']))&&(isset($_POST['sfrmgetfull']))){
      $smarty->assign("depselectfull",$_POST['selfull']);
      $smarty->assign("type","full");
      $dn = $_POST['selfull'];
    } else {
      $smarty->assign("depselectfull", "");
    }
    if((isset($_POST['selivbb']))&&(isset($_POST['sfrmgetivbb']))){
      $smarty->assign("depselectivbb",$_POST['selivbb']);
      $smarty->assign("type","ivbb");
      $dn = $_POST['selivbb'];
    } else {
      $smarty->assign("depselectivbb", "");
    }

    // Set values for optionlist in form
    $bases      = array();
    $acl_bases  = $this->ui->get_module_departments("ldapmanager");
    foreach($this->config->idepartments as $base_dn => $name){
      if(in_array_ics($base_dn,$acl_bases)){
        $bases[$base_dn] = $name;
      }
    }
    $smarty->assign("deplist", $bases);
    $smarty->assign("choicelist",array( get_people_ou()             =>"users" ,
                                        "ou=groups,"                =>"groups" ,
                                        "ou=systems,"               =>"computers",
                                        "ou=servers,ou=systems,"    =>"servers",
                                        "dc=addressbook,"           =>"addressbook"));

    // Get the LDAP link, to generate the Export
    $ldap = $this->config->get_ldap_link();

    if ($this->config->current['GOVERNMENTMODE'] == 'true'){
      $smarty->assign ("govmode", "true");
    } else {
      $smarty->assign ("govmode", "false");
    }

    $smarty->assign("LDIFError",FALSE);

    if(isset($dn))
    {
      if(!($ldap->dn_exists($dn) && $dn != "")){
        $smarty->assign("LDIFError",TRUE);
      	print_red (_("Error while exporting the requested entries!"));
      }
    }
   
    /* check alcs for given dn */
    $acls ="";
    if(isset($dn)){
      $acls = $this->ui->get_permissions($dn,"ldapmanager/xlsexport");
    }
 
    if((!preg_match("/r/",$acls)) && isset($dn)){

      /* Show error msg */      
      print_red(sprintf(_("You are not allowed to export the given ldap entry (%s)"),@LDAP::fix($dn)));

      /* Show main page */
      $smarty->assign("LDIFError",TRUE);
      $smarty->assign("para","");
    }else{

      // Export a single LDAP entry
      if(isset($_POST['sfrmgetsingle']))
      {
        $smarty->assign("para","?ivbb=2&amp;d=".base64_encode($_POST['single'])."&n=".base64_encode($_POST['selfull']));	
      }
      elseif(isset($_POST['sfrmgetfull']))
      {
        $smarty->assign("para","?ivbb=3&amp;dn=".base64_encode($_POST['selfull']));
      }
      elseif(isset($_POST['sfrmgetivbb']))
      {
        $smarty->assign("para","?ivbb=4&amp;dn=".base64_encode($_POST['selivbb']));
      }
    }

    /* Show main page */
    return ($smarty->fetch (get_template_path('contentexportxls.tpl', TRUE)));
  }

  /* Return plugin informations for acl handling
     #FIXME You can only read attributes within this report plugin */
  function plInfo()
  {
    return (array(
        "plShortName"   => _("Export XLS"),
        "plDescription" => _("Export XLS"),
        "plSelfModify"  => FALSE,
        "plDepends"     => array(),
        "plPriority"    => 2,
        "plSection"     => array("addon"),
        "plCategory"    => array("ldapmanager"),

        "plProvidedAcls" => array()
        ));
  }
}


// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
