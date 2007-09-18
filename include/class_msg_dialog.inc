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

	public function __construct($s_title,$s_message,$i_type)
	{
		$this->i_ID 	 = preg_replace("/[^0-9]*/","",microtime()); 
		$this->s_Title	 = $s_title;
		$this->s_Message = $s_message;
		$this->i_Type	 = $i_type;
		if(!isset($_SESSION['msg_dialogs'])){
			$_SESSION['msg_dialogs'] = array();
		}
		$_SESSION['msg_dialogs'][] = $this;
	}

	public static function display($s_title,$s_message,$i_type = INFO_DIALOG)
	{
		$box = new msg_dialog($s_title,$s_message,$i_type);	
	}

	public function is_confirmed()
	{
		if($this->i_Type == CONFIRM_DIALOG && isset($_POST['MSG_OK'])){
			return(TRUE);
		}
	}

	public function execute()
	{
		$smarty = get_smarty();
		$smarty->assign("i_Type",$this->i_Type);
		$smarty->assign("s_Message",$this->s_Message);
		$smarty->assign("s_Title",$this->s_Title);
		$smarty->assign("i_ID",$this->i_ID);
		$smarty->assign("IE",preg_match("/MSIE/", $_SERVER['HTTP_USER_AGENT']));
		return($smarty->fetch(get_template_path('msg_dialog.tpl')));
	}
	

	public static function get_dialogs()
	{
		$return  ="";
		if(isset($_SESSION['msg_dialogs']) && is_array($_SESSION['msg_dialogs'])){
			foreach($_SESSION['msg_dialogs'] as $key => $dialog){
				$return.= $dialog->execute();
			}
		}
		unset($_SESSION['msg_dialogs']);
		return($return);
	}
}
?>
