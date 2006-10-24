<?php
/*
   This code is part of GOsa (https://gosa.gonicus.de)
   Copyright (C) 2003  Cajus Pollmeier

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

require "tabsPartition.inc";
require "tabsVariable.inc";
require "tabsHook.inc";
require "tabsTemplate.inc";
require "tabsScript.inc";
require "tabsProfile.inc";
require "tabsPackage.inc";


class faiManagement extends plugin
{
	/* Definitions */
	var $plHeadline			= "FAI";
	var $plDescription		= "Fully Automatic Installation - management";

	/* CLI vars */
	var $cli_summary		  = "Handling of FAI entries";
	var $cli_description	= "This plugin represents a management tool\n
		which allows us to manage all needed attributes for fully automatic installations (FAI)";
	var $cli_parameters		= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

	/* Headpage attributes */
  var $lock_type    = "";    // should be branch/freeze
  var $lock_name    = "";
  var $lock_dn      = "";  

	/* attribute list for save action */
	var $attributes		= array("lock_type","lock_name","lock_dn"); 	// 	Attributes Managed by this plugin 
	var $objectclasses= array();	// 	ObjectClasses which the attributes are related to
	var $dialog				= array();	// 	This object contains every dialog we have currently opened

	var $objects			= array();	// 	This array contains all available objects shown in divlist
	var $is_dialog		= false;

  var $dispNewBranch= false;
  var $dispNewFreeze= false;

  var $DivListFai;

	/* construction/reconstruction 
	 */
	function faiManagement ($config, $ui)
	{
		/* Set defaults */
		$this->dn			= "";
		$this->config	= $config;
		$this->ui			= $ui;	
    
    /* Creat dialog object */
    $this->DivListFai = new divListFai($this->config,$this);
	}

	function execute()
	{
    /* Call parent execute */
    plugin::execute();

    /* Initialise vars and smarty */
		$smarty 	= get_smarty();
		$smarty->assign("BranchName","");
    
		$display 	= "";
    $s_action	= "";
		$s_entry 	= "";
    
    /* If an entry was locked, these vars will be stored in a session to allow direct edit */
    $_SESSION['LOCK_VARS_TO_USE'] = array("/^edit_entry$/","/^id$/","/^entry_edit_/","/^entry_delete_/");


    /****************
      Handle posts 
     ****************/

		/* Check ImageButton posts
		 * Create new tab ich new_xx is posted
		 */
    $posts = array( "/remove_branch/"=>"remove_branch",    "/branch_branch/"=>"branch_branch",
                    "/freeze_branch/"=>"freeze_branch",    "/create_partition/i"=>"new_partition",
                    "/create_script/i"=>"new_script",      "/create_hook/i"=>"new_hook",
                    "/create_variable/i"=>"new_variable",  "/create_template/i"=>"new_template",
                    "/create_package/i"=>"new_package",    "/create_profile/i"=>"new_profile",
                    "/edit_continue/"=>"select_class_name_finished");

		foreach($_POST as $name => $value){
      foreach($posts as $reg => $act ){
        if(preg_match($reg,$name)){
          $s_action = $act;
        }
      }
			if(preg_match("/^entry_edit_.*/",$name)){
				$s_entry = preg_replace("/^entry_edit_/","",$name);
				$s_entry = preg_replace("/_.*$/","",$s_entry);
				$s_action = "edit";
			}elseif(preg_match("/^entry_delete_.*/",$name)){
				$s_entry = preg_replace("/^entry_delete_/","",$name);
				$s_entry = preg_replace("/_.*$/","",$s_entry);
        $s_action = "delete";
      }
    }

		if(isset($_GET['edit_entry'])){
			$s_entry = $_GET['edit_entry'];
			$s_action = "edit";
		}
    
    if((isset($_POST['CancelBranchName'])) || (isset($_POST['CloseIFrame']))){
      $this->dispNewBranch = false;
      $this->dispNewFreeze = false;
    }


    $type_acl_mapping = array(
        "FAIpartitionTable"  => "faiPartitionTable", 
        "FAIpackageList"     => "faiPackage",
        "FAIscript"          => "faiScript",
        "FAIvariable"        => "faiVariable",
        "FAIhook"            => "faiHook",
        "FAIprofile"         => "faiProfile",
        "FAItemplate"        => "faiTemplate");

    /****************
      Delete confirme dialog 
     ****************/

		if ($s_action=="delete"){

      /* Get 'dn' from posted termlinst */
      $this->dn= $this->objects[$s_entry]['dn'];

			/* Load permissions for selected 'dn' and check if
			   we're allowed to remove this 'dn' */
      $acl  = $this->ui->get_permissions($this->dn,"fai/".$type_acl_mapping[$this->objects[$s_entry]['type']]);
      if(preg_match("/d/",$acl)){

				/* Check locking, save current plugin in 'back_plugin', so the dialog knows where to return. */
				if (($user= get_lock($this->dn)) != ""){
					return(gen_locked_message ($user, $this->dn));
				}

				/* Lock the current entry, so nobody will edit it during deletion */
				add_lock ($this->dn, $this->ui->dn);
				$smarty->assign("warning", sprintf(_("You're about to delete all information about the FAI class at '%s'."), @LDAP::fix($this->dn)));
				return($smarty->fetch(get_template_path('remove.tpl', TRUE)));
			} else {

				/* Obviously the user isn't allowed to delete. Show message and
				   clean session. */
				print_red (_("You are not allowed to delete this component!"));
			}
		}


    /****************
      Delete aborted  
     ****************/

		/* Delete canceled? */
		if (isset($_POST['delete_cancel'])){
			del_lock ($this->dn);
		}


    /****************
      Delete confirmed 
     ****************/

		/* Deltetion was confirmed, so delete this entry
		 */
		if (isset($_POST['delete_terminal_confirm'])){

			/* Some nice guy may send this as POST, so we've to check
			   for the permissions again. */

      /* Find out more about the object type */
      $ldap	  = $this->config->get_ldap_link();
      $ldap->cat($this->dn, array('objectClass'));
      $attrs	= $ldap->fetch();
      $type	  = $this->get_type($attrs);			

      $acl  = $this->ui->get_permissions($this->dn,"fai/".$type[1]);
      if(preg_match("/d/",$acl)){

        $this->dialog = new $type[0]($this->config,	$this->config->data['TABS'][$type[2]], $this->dn,"fai");
        $this->dialog->set_acl_base($this->dn);
        $this->dialog->by_object[$type[1]]->remove_from_parent ();
        unset ($this->dialog);
        gosa_log ("FAI class '".$this->dn."' has been tagged as removed");
        $this->dialog= NULL;
        $to_del = clean_up_releases($this->dn);
        save_release_changes_now();

        foreach($to_del as $dn){
          $ldap->rmdir_recursive($dn);
          gosa_log(sprintf(_("Release cleanup : Removing object (tagged as remvoed) that is no longer in use '%s'."),$dn));
        }

      } else {

        /* Normally this shouldn't be reached, send some extra
           logs to notify the administrator */
        print_red (_("You are not allowed to delete this component!"));
        gosa_log ("Warning: '".$this->ui->uid."' tried to trick system ".
            "deletion.");
      }

      /* Remove lock file after successfull deletion */
      del_lock ($this->dn);
    }


    /****************
      Edit entry 
     ****************/

		if(($s_action == "edit") && (!isset($this->dialog->config))){
			$entry    = $this->objects[$s_entry];
			$a_setup  = $this->get_type($entry);
			$this->dn = $entry['dn'];

			/* Check locking, save current plugin in 'back_plugin', so the dialog knows where to return. */
			if (($user= get_lock($this->dn)) != ""){
				return(gen_locked_message ($user, $this->dn));
			}
			add_lock ($this->dn, $this->ui->dn);

			$this->dialog     = new $a_setup[0]($this->config,$this->config->data['TABS'][$a_setup[2]],$this->dn,"fai");
      $this->dialog->set_acl_base($this->dn);
			$this->is_dialog  = true;

      if($entry['FAIstate'] == "freeze"){
        #$this->dialog->set_acl(array("*none*"))  ;    
        echo "Dawn it is freezed.";
      }
			$_SESSION['objectinfo'] = $this->dn;
		}


    /*  Branch handling 
        09.01.2006
    */

    /****************
      Remove branch
     ****************/

    /* Remove branch 
     */
    if($s_action == "remove_branch"){
      $base= $this->DivListFai->selectedBranch;

      /* Load permissions for selected 'dn' and check if
         we're allowed to remove this 'dn' */
      $this->set_acl_base($base);
      if($this->acl_is_removeable()){

        $smarty->assign("info", sprintf(_("You're about to delete a fai branch / freeze  '%s'."), $this->DivListFai->selectedBranch));
        return($smarty->fetch(get_template_path('remove_branch.tpl',TRUE)));
      } else {
        print_red (_("You are not allowed to delete this release!"));
      }
    }

    
    /****************
      Remove branch confirmed
     ****************/

    if(isset($_POST['delete_branch_confirm'])){
      $bb =  $this->DivListFai->selectedBranch;
      if(!isset($ldap)){
        $ldap = $this->config->get_ldap_link();
      }

      $br = $this->getBranches();

      if(isset($br[$bb]) && $this->acl_is_removeable()){
        $name = $br[$bb];
        $ldap->cd($bb);
        $ldap->recursive_remove();
        $ldap->cd(preg_replace('/,ou=fai,ou=configs,ou=systems,/', ',ou=apps,', $bb));
        $ldap->recursive_remove();
        $this->DivListFai->selectedBranch = "main";

        /* Post remove */
        $this->lock_name   = $name;
        $this->lock_dn     = $bb;
        $this->postremove();
      }
    }


    /****************
      Create a new branch "insert Name"
     ****************/

    if((isset($_POST['UseBranchName']))&&(($this->dispNewBranch)||($this->dispNewFreeze))){
      $_SESSION['LASTPOST'] = $_POST;

      if($this->dispNewBranch){
        $type = "branch";
      }else{
        $type = "freeze";
      }

      /* Check branch name */
      $name = $_POST['BranchName'];
      $is_ok = true;
      $smarty->assign("BranchName",$name);
      $base= "ou=fai,ou=configs,ou=systems,".$this->DivListFai->selectedBase;

      /* Check used characters */
      if(preg_match("/[^0-9a-z \ö\ä\ü\.-_:,]/i",$name)){
        if($type == "branch"){
          print_red(_("Specified branch name is invalid."));
        }else{
          print_red(_("Specified freeze name is invalid."));
        }
        $is_ok = false;
      }

      /* Check if this name is already in use */
      if(!$this->CheckNewBranchName($_POST['BranchName'],$this->DivListFai->selectedBranch)){
        print_red(_("This name is already in use."));
        $is_ok = false;
      }

      if($is_ok){
        $_SESSION['LASTPOST']['base'] = $base;
        $_SESSION['LASTPOST']['type'] = $type;
        $smarty->assign("iframe", true);
        $smarty->assign("plugID", $_GET['plug']);
        $display 	= $smarty->fetch(get_template_path('faiNewBranch.tpl', TRUE, dirname(__FILE__)));
        return($display);
      }
    }


    /****************
      Create a new branch 
     ****************/

    if(isset($_GET['PerformBranch'])){

      if(!$this->acl_is_createable()){
        print_red(_("You are not allowed to create a new branch."));
      }else{

        /* Create it know */
        $this->dispNewBranch = false;
        $this->dispNewFreeze = false;
        $base = $_SESSION['LASTPOST']['base'];
        $_POST = $_SESSION['LASTPOST'];      
        $name = $_POST['BranchName'];

        $type = $_SESSION['LASTPOST']['type'];
        $ldap = $this->config->get_ldap_link();

        $baseToUse = $base;
        if($this->DivListFai->selectedBranch != "main" ){
          $baseToUse = $this->DivListFai->selectedBranch;
        }

        /* Create new Release name to be able to set faidebianRelease for FAIpackageList */

        $CurrentReleases  = $this->getBranches();
        $NewReleaseName   = $name;
        if(isset($CurrentReleases[$this->DivListFai->selectedBranch])) {
          if($this->DivListFai->selectedBranch != "main"){
            $NewReleaseName = $CurrentReleases[$this->DivListFai->selectedBranch]."/".$name;
            $NewReleaseName = preg_replace("#\/#","/",$NewReleaseName); 
          }else{
            $NewReleaseName   = $name;
          }
        }

        $appsrc = preg_replace("/ou=fai,ou=configs,ou=systems,/","ou=apps,",$baseToUse); 
        $appdst = preg_replace("/ou=fai,ou=configs,ou=systems,/","ou=apps,","ou=".$name.",".$baseToUse) ; 

        $mimesrc = preg_replace("/ou=fai,ou=configs,ou=systems,/","ou=mime,",$baseToUse); 
        $mimedst = preg_replace("/ou=fai,ou=configs,ou=systems,/","ou=mime,","ou=".$name.",".$baseToUse) ; 

        /* Print header to have styles included */
        echo "<!-- headers.tpl-->".$smarty->fetch(get_template_path('headers.tpl'));
        echo "<body style='background-image:none;margin:3px;color:black'>";

        /* Duplicate applications 
         */
        $ldap->cat($appsrc,array("dn")) ;
        if($ldap->count()){
          $ldap->cd ($appdst);
          $ldap->recursive_remove();
          $ldap->cd ($this->config->current['BASE']);
          $ldap->copy_FAI_resource_recursive($appsrc,$appdst,$NewReleaseName,$type,true);
        }

        /* Duplicate mime types 
         */
        $ldap->cat($mimesrc,array("dn")) ;
        if($ldap->count()){
          $ldap->cd ($appdst);
          $ldap->recursive_remove();
          $ldap->cd ($this->config->current['BASE']);
          $ldap->copy_FAI_resource_recursive($mimesrc,$mimedst,$NewReleaseName,$type,true);
        }

        $attr = array();
        $attr['objectClass'] = array("organizationalUnit","FAIbranch");
        $attr['ou']       = $name;
        $attr['FAIstate'] = $type;
        $ldap->cd($this->config->current['BASE']);
        $ldap->cd("ou=".$name.",".$baseToUse);
        $ldap->cat("ou=".$name.",".$baseToUse);
        if($ldap->count()){
          $ldap->modify($attr);
        }else{
          $ldap->add($attr);
        }

        /* Duplicate fai objects 
         */
        //      $ldap->cd ("ou=".$name.",".$baseToUse);
        //      $ldap->recursive_remove();
        //      $ldap->cd ($this->config->current['BASE']);
        //      $ldap->copy_FAI_resource_recursive($baseToUse,"ou=".$name.",".$baseToUse,$NewReleaseName,$type,true);

        echo "<div style='width:100%;text-align:right;'><form name='form' method='post' action='?plug=".$_GET['plug']."' target='_parent'>
          <br><input type='submit' name='CloseIFrame' value='"._("Continue")."'>
          </form></div>";

        /* Print footer to have valid html */
        echo "</body></html>";

        $this->dispNewFreeze = false; 

        /* Postcreate */ 

        /* Assign possible attributes */
        $this->lock_type  = $type; 
        $this->lock_name  = $name; 
        $this->lock_dn    = $baseToUse;
        $this->postcreate();
        exit();
      }
    }

    /****************
      Display dialog to enter new Branch name
     ****************/

    if(($s_action == "branch_branch")||($this->dispNewBranch)){
      $this->dispNewBranch=true;
      $smarty->assign("iframe",false);
		  $display 	.= $smarty->fetch(get_template_path('faiNewBranch.tpl', TRUE, dirname(__FILE__)));
      return($display);
    }
   
 
    /****************
      Display dialog to enter new Freeze name
     ****************/

    if(($s_action == "freeze_branch")||($this->dispNewFreeze)){
      $this->dispNewFreeze = true;
      $smarty->assign("iframe",false);
		  $display 	.= $smarty->fetch(get_template_path('faiNewBranch.tpl', TRUE, dirname(__FILE__)));
      return($display);
    }


    /****************
      Create a new object 
     ****************/

    $types = array( "new_partition"     =>  "FAIpartitionTable",
                    "new_script"        =>  "FAIscript",
                    "new_hook"          =>  "FAIhook",
                    "new_variable"      =>  "FAIvariable",
                    "new_template"      =>  "FAItemplate",
                    "new_package"       =>  "FAIpackageList");

    if(isset($types[$s_action])){
      $acl = $this->ui->get_permissions($this->DivListFai->selectedBase,"fai/".$type_acl_mapping[$types[$s_action]]);
      if(preg_match("/c/",$acl)){
        $this->dialog = new askClassName($this->config,$this->dn,$this->ui,$types[$s_action]);
      }else{
        print_red(sprintf(_("You are not allowed to create a new '%s' object."),$types[$s_action]));
      }
    }

    /* New Profile */
    if($s_action == "new_profile"){
      $this->dn = "new" ;

      $acl = $this->ui->get_permissions($this->DivListFai->selectedBase,"fai/faiProfile");
      if(preg_match("/c/",$acl)){
        $a_setup= $this->get_type(array("objectClass"=>array("FAIprofile")));
        $this->dialog = new $a_setup[0]($this->config,$this->config->data['TABS'][$a_setup[2]],$this->dn,"faiProfile");
        $this->dialog->set_acl_base($_SESSION['CurrentMainBase']);

        $this->is_dialog = false;
      }else{
        print_red(sprintf(_("You are not allowed to create a new '%s' object."),"FAIprofile"));
      }
    }


    /****************
      Get from ask class name dialog 
     ****************/

    if($s_action == "select_class_name_finished"){
      $this->dialog->save_object();
      if(count($this->dialog->check())!=0){
        foreach($this->dialog->check() as $msg){
          print_red($msg);
        }		
      }else{
        $this->dn = "new" ;
        $a_setup= $this->get_type(array("objectClass"=>array($this->dialog->objectClass)));
        $name = $this->dialog->save();

        $this->dialog = new $a_setup[0]($this->config,$this->config->data['TABS'][$a_setup[2]],$this->dn,"fai");
        $this->dialog->set_acl_base($_SESSION['CurrentMainBase']);
        $this->dialog->by_object[$a_setup[1]]->cn = $name;
        $this->is_dialog = true;
      }		
    }	


    /****************
     Cancel dialogs 
     ****************/

		if(isset($_POST['edit_cancel'])){
			unset($this->dialog);
			$this->dialog=NULL;
			$this->is_dialog = false;
			unset($_SESSION['objectinfo']);
			del_lock ($this->dn);
		}


    /****************
      Save sub dialogs 
     ****************/

		/* This check if the given tab could be saved 
		 * If it was possible to save it, remove dialog object. 
		 * If it wasn't possible, show errors and keep dialog.
		 */
		if((isset($_POST['edit_finish']) || isset($_POST['edit_apply'])) && (isset($this->dialog->config))){
			$this->dialog->save_object();
      $msgs= $this->dialog->check();
			if(count($msgs)!=0){
				foreach($msgs as $msg){
					print_red($msg);
				}
			}else{
				$this->dialog->save();
        save_release_changes_now();
        if (!isset($_POST['edit_apply'])){
          del_lock ($this->dn);
          unset($this->dialog);
          $this->dialog=NULL;
          $this->is_dialog=false;
          unset($_SESSION['objectinfo']);
        }
			}
		}


    /****************
      Display currently open dialog 
     ****************/

		/* If dialog is set, but $this->is_dialog==false, then 
		 *  only the "abort" button is shown, this are dialogs that must not be saved.  
		 * If is_dialog == true, we are currently editing tab objects.
		 *  Here we need both, save and cancel
		 */ 

		if(($this->dialog != NULL) && (isset($this->dialog->config))){
			$display .= $this->dialog->execute();
			/* Don't show buttons if tab dialog requests this */

      if(isset($this->dialog->current)){

        $obj = $this->dialog->by_object[$this->dialog->current];

        if((isset($obj->is_dialog) && (!$obj->is_dialog)) || (isset($obj->dialog) && (!$obj->dialog))){

          $display.= "<p style=\"text-align:right\">\n";
          $display.= "<input type=\"submit\" name=\"edit_finish\" style=\"width:80px\" value=\""._("Ok")."\">\n";
          $display.= "&nbsp;\n";
          if ($this->dn != "new"){
            $display.= "<input type=submit name=\"edit_apply\" value=\""._("Apply")."\">\n";
            $display.= "&nbsp;\n";
          }
          $display.= "<input type=\"submit\" name=\"edit_cancel\" value=\""._("Cancel")."\">\n";
          $display.= "</p>";
        }elseif(!isset($this->dialog->current)){
          $display.= "<p style=\"text-align:right\">\n";
          $display.= "<input type=\"submit\" name=\"edit_continue\" value=\""._("Continue")."\">&nbsp;";
          $display.= "<input type=\"submit\" name=\"edit_cancel\" value=\""._("Cancel")."\">\n";
          $display.= "</p>";
        }
      }else{
        $display.= "<p style=\"text-align:right\">\n";
        $display.= "<input type=\"submit\" name=\"edit_continue\" value=\""._("Continue")."\">&nbsp;";
        $display.= "<input type=\"submit\" name=\"edit_cancel\" value=\""._("Cancel")."\">\n";
        $display.= "</p>";

      }
      return($display);
		}
		

    /****************
      Dialog display
     ****************/

    /* Check if there is a snapshot dialog open */
    $base = $this->DivListFai->selectedBase;
    if($str = $this->showSnapshotDialog($base,$this->get_used_snapshot_bases())){
      return($str);
    }

    /* Display dialog with system list */
    $this->DivListFai->parent = $this;
    $this->DivListFai->execute();
    $this->DivListFai->AddDepartments($this->DivListFai->selectedBase,4);
    $this->reload();
    $this->DivListFai->setEntries($this->objects);
    return($this->DivListFai->Draw());
	}


  /* Return departments, that will be included within snapshot detection */
  function get_used_snapshot_bases()
  {
    $tmp = array();
    $types = array("hooks","scripts","disk","packages","profiles","templates","variables");
    foreach($types as $type){
      if($this->DivListFai->selectedBranch == "main"){
        $tmp[] = "ou=".$type.",ou=fai,ou=configs,ou=systems,".$this->DivListFai->selectedBase;
      }else{
        $tmp[] = "ou=".$type.",".$this->DivListFai->selectedBranch;
      }
    }
    return($tmp);
  }


  /* Get available branches for current base */
  function getBranches($base = false,$prefix = "")
  {
    $ret = array("main"=>"/");
    $ldap = $this->config->get_ldap_link();
    if(!$base){
      $base= $_SESSION['CurrentMainBase'];
    }
    $tmp = get_all_releases_from_base($base,true);
    foreach($tmp as $dn => $name){
      $ret[$dn]=$name;
    }
    return ($ret);
  }

  

  /* reload list of objects */
  function reload()
  {
    /* Variable initialisation */
    $str            = "";
    $Regex          = $this->DivListFai->Regex;
    $this->objects  = array();

    /* Get base */
    $base = "ou=fai,ou=configs,ou=systems,".$this->DivListFai->selectedBase;
    if($this->DivListFai->selectedBranch != "main"){
      $br = $this->getBranches();
      if(isset($br[$this->DivListFai->selectedBranch])){
        $base = $this->DivListFai->selectedBranch;
      }else{
        $base = "ou=fai,ou=configs,ou=systems,".$this->DivListFai->selectedBase;
      }
    }

    $this->lock_type = get_release_tag(get_release_dn($base));

    /* Create a new list of FAI object 
     * Generate List of Partitions,Hooks,Scripts,Templates,Profiles ... 
     */
    $ObjectTypes = array(
        "FAIpartitionTable"  => array("OU"=>"ou=disk,"        , "CHKBOX"=>"ShowPartitions"  ,"ACL" => "faiPartitionTable"),
        "FAIpackageList"     => array("OU"=>"ou=packages,"    , "CHKBOX"=>"ShowPackages"    ,"ACL" => "faiPackage"),
        "FAIscript"          => array("OU"=>"ou=scripts,"     , "CHKBOX"=>"ShowScripts"     ,"ACL" => "faiScript"),
        "FAIvariable"        => array("OU"=>"ou=variables,"   , "CHKBOX"=>"ShowVariables"   ,"ACL" => "faiVariable"),
        "FAIhook"            => array("OU"=>"ou=hooks,"       , "CHKBOX"=>"ShowHooks"       ,"ACL" => "faiHook"),
        "FAIprofile"         => array("OU"=>"ou=profiles,"    , "CHKBOX"=>"ShowProfiles"    ,"ACL" => "faiProfile"),
        "FAItemplate"        => array("OU"=>"ou=templates,"   , "CHKBOX"=>"ShowTemplates"   ,"ACL" => "faiTemplate"));

    $filter = "";
    foreach($ObjectTypes as $key => $data){
      if($this->DivListFai->$data['CHKBOX']){
        $filter.= "(objectClass=".$key.")";
      }
    }
    $filter = "(&(|".$filter.")(cn=$Regex))";
    
    /* Get resolved release dependencies */
    $tmp = get_all_objects_for_given_base($base,$filter);

    /* Ge listed ldap objects */
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    foreach($tmp as $entry){

      /* Get some more informations about the object */ 
      $ldap->cat($entry['dn'], array("cn","description","objectClass","FAIclass","FAIstate","objectClass"));
      $object  = $ldap->fetch();

      /* Walk through possible types */
      foreach($ObjectTypes as $type => $rest){  

        $acl = $this->ui->get_permissions($object['dn'],"fai/".$rest ['ACL']);

        if(in_array($type,$object['objectClass']) && preg_match("/r/",$acl)){

          /* Prepare object */
          unset($object['objectClass']['count']);
          if(!isset($object['description'][0])){
            $object['description'][0]="";
          }

          /* Clean up object informations */
          $obj                  = array();
          $obj['cn'] 			      = $object['cn'][0];
          $obj['dn'] 			      = $object['dn'];
          $obj['acl'] 			    = $acl;
          $obj['description']   = $object['description'][0];
          $obj['objectClass']   = $object['objectClass'];

          /* Append type to this string, to be able to check if the selected 
           * entry is of type 'freeze' or 'branch'
           */
          if(!isset($object['FAIstate'])){
            $obj['FAIstate'] = $this->lock_type;
          }else{
            $obj['FAIstate'] = $object['FAIstate'][0]; 
          }

          $this->objects[strtolower($obj['cn']).$obj['cn'].$type] = $obj;
          $this->objects[strtolower($obj['cn']).$obj['cn'].$type]['type']=$type;
        }
			}
		}

		ksort($this->objects);
		reset ($this->objects);
	
		/* use numeric index, thats a bit more secure */	
		$tmp0 = array();
		foreach($this->objects as $obj){
			$tmp0[]= $obj;
		}
		$this->objects = array();
		$this->objects = $tmp0;
	}

	function remove_lock()
	{
		if (isset($this->dn)){
			del_lock ($this->dn);
		}
	}

	function get_type($array){
		if(in_array("FAIpartitionTable",$array['objectClass'])){
			return(array("tabsPartition","faiPartitionTable","FAIPARTITIONTABS"));
		}
		if(in_array("FAIscript",$array['objectClass'])){
			return(array("tabsScript","faiScript","FAISCRIPTTABS"));
		}
		if(in_array("FAItemplate",$array['objectClass'])){
			return(array("tabsTemplate","faiTemplate","FAITEMPLATETABS"));
		}
		if(in_array("FAIhook",$array['objectClass'])){
			return(array("tabsHook","faiHook","FAIHOOKTABS"));
		}
		if(in_array("FAIvariable",$array['objectClass'])){
			return(array("tabsVariable","faiVariable","FAIVARIABLETABS"));
		}
		if(in_array("FAIprofile",$array['objectClass'])){
			return(array("tabsProfile","faiProfile","FAIPROFILETABS"));
		}
		
		if(in_array("FAIpackageList",$array['objectClass'])){
			return(array("tabsPackage","faiPackage","FAIPACKAGETABS"));
		}
	}

  function CheckNewBranchName($name,$base)
  {
    $f = $this->DivListFai->selectedBranch;
    if($name == ""){
      return(false);
    }elseif(in_array($name,$this->getBranches($f))) {
      return(false);
    }elseif(is_department_name_reserved($name,$base)){
      return(false);
    }
    return(true);
  }

  function save_object()
  {
    $this->DivListFai->save_object();
  }


  /* Return plugin informations for acl handling */ 
  function plInfo()
  {
    return (array( 
          "plShortName"   => _("FAI releases"),
          "plDescription" => _("FAI release management"),
          "plSelfModify"  => FALSE,
          "plDepends"     => array(),
          "plPriority"    => 0,
          "plSection"     => array("administration"),           
          "plCategory"    => array("fai"=> array("description" => _("FAI"),
                                                 "objectClass" => "FAIclass")),
          "plProvidedAcls"=> array()));
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
