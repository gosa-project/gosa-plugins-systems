<?php

/* Snap shot dialog class */
class SnapShotDialog extends plugin 
{
	var $config;
	var $attributes 				= array("CurrentDescription");
	var $CurrentDescription 		= "";
	var $parent						= NULL;
	var $display_restore_dialog		= false;		/* Defines the restore mode */
	var $display_all_removed_objects= false; 		/* Specifies which objects will be listed, all snapshots for a single entry 
														or all snapshots of already deleted objects  */	
	var $dialog						= true;
	var $del_dn						= "";
	var $ui;
	var $acl;
	
	function SnapShotDialog($config,$dn,$parent)
	{
		plugin::plugin($config,$dn);
		$this->parent 	= $parent;
		$this->ui 		= get_userinfo();
	}
	
	
	/* Display snapshot dialog */
	function execute()
	{
		plugin::execute();
		$smarty = get_smarty();
	
		$once = true;
		foreach($_POST as $name => $value){
			if((preg_match("/^RemoveSnapShot_/",$name)) && ($once)){
				$once = false;
	
				$entry = preg_replace("/^RemoveSnapShot_/","",$name);
				$entry = base64_decode(preg_replace("/_[xy]$/","",$entry));

				$acl			= get_permissions ($entry, $this->ui->subtreeACL);
				$this->acl		= get_module_permission($acl, "snapshot", $entry );

				if (chkacl($this->acl, "delete") == ""){
					$this->del_dn 	= $entry;
					$smarty= get_smarty();
					$smarty->assign("intro", sprintf(_("You're about to delete the snapshot '%s'."), @LDAP::fix($this->del_dn)));
					return($smarty->fetch (get_template_path('remove.tpl')));
				} else {

					/* Obviously the user isn't allowed to delete. Show message and
					   clean session. */
					print_red (_("You are not allowed to delete this snap shot!"));
				}
			}
		}

		/* Remove snapshot */
		if(isset($_POST['delete_confirm']) && !empty($this->del_dn)){
			$acl			= get_permissions ($this->del_dn, $this->ui->subtreeACL);
			$this->acl		= get_module_permission($acl, "snapshot", $this->del_dn);
			if (chkacl($this->acl, "delete") == ""){
				$this->remove_snapshot($this->del_dn);
				$this->del_dn = "";
			}	
		}

		/* We must restore a snapshot */
		if($this->display_restore_dialog){

			/* Should we only display all snapshots of already deleted objects 
                or the snapshots for the given object dn */
			$res = array();
			if($this->display_all_removed_objects){
				if(is_array($this->dn)){
					$tmp = array();
					foreach($this->dn as $dn){
						$tmp = array_merge($tmp,$this->getAllDeletedSnapshots($dn,true));
					}
				}else{
					$tmp = $this->getAllDeletedSnapshots($this->dn,true);
				}
			}else{
				$tmp = $this->Available_SnapsShots($this->dn,true);
			}

			$DivListSnapShots = new DivSelectBox("SnapShotRestore");
			$DivListSnapShots->SetHeight(180);

			$list_of_elements = array();			
			
			/* Walk through all entries and setup the display text */
			foreach($tmp as $key => $entry){
				$TimeStamp = $entry['gosaSnapshotTimestamp'][0];
				$list_of_elements[$TimeStamp] = $entry;
			}

			/* Sort generated list */
			krsort($list_of_elements);
	
			/* Add Elements to divlist */	
			foreach($list_of_elements as $entry){

				$actions = "<input type='image' src='images/edittrash.png' name='RemoveSnapShot_%KEY' 
								class='center' title='"._("Remove snapshot")."'>&nbsp;";
				$actions.= "<input type='image' src='images/restore.png' name='RestoreSnapShot_%KEY' 
								class='center' title='"._("Restore snapshot")."'>&nbsp;";

				$time_stamp 	= date("d.m.Y H.i.s",preg_replace("/\-.*$/","",$entry['gosaSnapshotTimestamp'][0]));
				$display_data 	= $entry['description'][0];

				if($this->display_all_removed_objects){
					$display_data.= " - ".$entry['gosaSnapshotDN'][0];
				}

				$field0 = array("string"=> $time_stamp , "attach"=> "style='vertical-align:top;width:120px;'");
				$field1 = array("string"=> htmlentities (utf8_decode($display_data)), "attach"=> "");
				$field2 = array("string"=> preg_replace("/%KEY/",base64_encode($entry['dn']),$actions) , 
								"attach"=> "style='border-right:0px;vertical-align:top;width:40px;text-align:right;'");
				$DivListSnapShots->AddEntry(array($field0,$field1,$field2));
			}		

			$smarty->assign("SnapShotDivlist",$DivListSnapShots->DrawList());	
			$smarty->assign("CountSnapShots",count($list_of_elements));
		}

		$smarty->assign("restore_deleted",$this->display_all_removed_objects);
		$smarty->assign("RestoreMode",$this->display_restore_dialog);
		$smarty->assign("CurrentDate",date("d.m.Y H:i"));
		$smarty->assign("CurrentDN",$this->dn);
		$smarty->assign("CurrentDescription",$this->CurrentDescription);
		return($smarty->fetch(get_template_path("snapshotdialog.tpl")));
	}


	function check()
	{
		$message = plugin::check();
		if(!$this->display_restore_dialog){
			if(empty($this->CurrentDescription)){
				$message[]  = _("Please specify a valid description for this snapshot.");
			}
		}
		return($message);
	}


	function save_object()
	{	
		plugin::save_object();
		foreach($this->attributes as $name){
			if(isset($_POST[$name])){
				$this->$name = stripslashes($_POST[$name]);
			}
		}
	}
}

?>
