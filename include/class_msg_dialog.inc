<?php

define("INFO_DIALOG"	, 10001);
define("WARNING_DIALOG"	, 10002);
define("ERROR_DIALOG"	, 10003);
define("CONFIRM_DIALOG"	, 10004);

class msg_dialog
{
	private $s_Title 	= "Undefined";
	private $s_Message 	= "Undefined";
	private $i_Type 	= INFO_DIALOG ;
	private $i_ID		= 0;
	private $a_Trace	= array();

	public function __construct($s_title,$s_message,$i_type)
	{
		if(!in_array($i_type,array(INFO_DIALOG,WARNING_DIALOG,ERROR_DIALOG,CONFIRM_DIALOG))){
			trigger_error("Invalid msg_dialog type.");
			$i_type = INFO_DIALOG;
		}

		$this->i_ID 	 = preg_replace("/[^0-9]*/","",microtime()); 
		$this->s_Title	 = $s_title;
		$this->s_Message = $s_message;
		$this->i_Type	 = $i_type;
		$this->a_Trace	 = debug_backtrace();
		if(!isset($_SESSION['msg_dialogs'])){
			$_SESSION['msg_dialogs'] = array();
		}
		$_SESSION['msg_dialogs'][] = $this;
	}

	public static function display($s_title,$s_message,$i_type = INFO_DIALOG)
	{
		new msg_dialog($s_title,$s_message,$i_type);	
	}

	public function get_ID()
	{
		return($this->i_ID);
	}

	public function execute()
	{
		$smarty = get_smarty();
		$smarty->assign("s_Trace",print_a($this->a_Trace,TRUE));
		$smarty->assign("i_Type",$this->i_Type);
		$smarty->assign("s_Message",$this->s_Message);
		$smarty->assign("s_Title",$this->s_Title);
		$smarty->assign("i_ID",$this->i_ID);
		$smarty->assign("frame",false);
		$smarty->assign("JS",$_SESSION['js']);
		$smarty->assign("IE",preg_match("/MSIE/", $_SERVER['HTTP_USER_AGENT']));
		return($smarty->fetch(get_template_path('msg_dialog.tpl')));
	}

	
	public function is_confirmed()
	{
		if(isset($_POST['MSG_OK'.$this->i_ID])){
			return(TRUE);
		}else{
			return(FALSE);
		}
	}
	

	public static function get_dialogs()
	{
		$return  ="";
		$dialog_ids= "";

		$seen = "";
		if(isset($_POST['closed_msg_dialogs'])){
#			$seen = $_POST['closed_msg_dialogs'];
		}

		if(isset($_SESSION['msg_dialogs']) && is_array($_SESSION['msg_dialogs']) && count($_SESSION['msg_dialogs'])){

			/* Get frame one */
			$smarty = get_smarty();
			$smarty->assign("frame", true);
			$smarty->assign("IE",preg_match("/MSIE/", $_SERVER['HTTP_USER_AGENT']));
			$return = $smarty->fetch(get_template_path('msg_dialog.tpl'));

			if(!$_SESSION['js']){
				$dialog = array_pop($_SESSION['msg_dialogs']);
				$return.= $dialog->execute();
			}else{
				foreach($_SESSION['msg_dialogs'] as $key => $dialog){

					if(preg_match("/".$dialog->get_ID()."/",$seen)){
						unset($_SESSION['msg_dialogs'][$key]);
					}else{
						$return.= $dialog->execute();
						$dialog_ids= $dialog->get_ID().",".$dialog_ids;
					}
					unset($_SESSION['msg_dialogs'][$key]);
				}
			}
			$dialog_ids = preg_replace("/,$/","",$dialog_ids);

			$return .= "</div>";
			$return .="<input type='hidden' style='width:400px;' name='pending_msg_dialogs' id='pending_msg_dialogs' value='".$dialog_ids."'>";
			$return .="<input type='hidden' style='width:400px;' name='closed_msg_dialogs' id='closed_msg_dialogs' value=''>";
			$return .="<input type='hidden' style='width:400px;' name='current_msg_dialogs' id='current_msg_dialogs' value=''>";
			$return .="<input type='hidden' style='width:700px;' name='js_debug' id='js_debug'>";
		}
		return($return);
	}
}
?>
