<?php

/* Snap shot dialog class */
class SnapShotDialog extends plugin 
{
	var $config;
	var $attributes 		= array("CurrentDescription");
	var $CurrentDescription = "";
	var $parent				= NULL;
	var $Restore			= false;		/* Defines the restore mode */
	var $DeletedOnes		= false; 		/* Specifies which objects will be listed */	
	var $dialog				= true;
	
	function SnapShotDialog($config,$dn,$parent)
	{
		plugin::plugin($config,$dn);
		$this->parent = $parent;
	}
	
	
	/* Display snapshot dialog */
	function execute()
	{
		plugin::execute();
		$smarty = get_smarty();

		/* We must restore a snapshot, so get snapshots  */
		if($this->Restore){

			/* Should we only display all snapshots of already deleted objects 
                or the snapshots for the given object dn */
			$res = array();
			if($this->DeletedOnes){
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
		
			/* Walk through all entries and setup the display text */
			foreach($tmp as $key => $entry){
			
				$data = $entry['description'][0];
				$date = date("d.m.Y H.i.s",preg_replace("/\-.*$/","",$entry['gosaSnapshotTimestamp'][0]));
				if($this->DeletedOnes){
					$data.= " - ".$entry['gosaSnapshotDN'][0];
				}
	
				if(strlen($data) > 83){
					$data= substr($data,0,80)." ...";
				}
	
				$res[base64_encode($entry['dn'])] = $date." - ".$data;
			}
			natcasesort($res);
			$smarty->assign("SnapShots",array_reverse($res));
			$smarty->assign("CountSnapShots",count($res));
		}

		$smarty->assign("RestoreMode",$this->Restore);
		$smarty->assign("CurrentDate",date("d.m.Y H:i"));
		$smarty->assign("CurrentDN",$this->dn);
		$smarty->assign("CurrentDescription",$this->CurrentDescription);
		return($smarty->fetch(get_template_path("snapshotdialog.tpl",TRUE,dirname(__FILE__))));
	}


	function check()
	{
		$message = plugin::check();
		if(!$this->Restore){
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
