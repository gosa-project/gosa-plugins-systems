<?php

namespace GosaSystems\admin\systems\services\gofax;

use \log as log;
use GosaSystems\admin\systems\services\GoService as GoService;
use \plugin as plugin;
use \msgPool as msgPool;

class GoFaxServer extends GoService
{

    public $cli_summary      = "This pluign is used within the ServerService Pluign \nand indicates that this server supports fax informations.";
    public $cli_description  = "Some longer text\nfor help";
    public $cli_parameters   = array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

    /* This plugin only writes its objectClass */
    public $objectclasses    = array("goFaxServer");
    public $attributes       = array("goFaxAdmin", "goFaxPassword");
    public $StatusFlag       = "goFaxServerStatus";

    /* This class can't be assigned twice so it conflicts with itsself */
    public $conflicts        = array("goFaxServer");

    public $DisplayName      = "";
    public $dn               = NULL;
    public $acl;

    public $goFaxAdmin         = "";
    public $goFaxPassword      = "";
    public $goFaxServerStatus  = "";
    public $cn                 = "";
    public $view_logged  = FALSE;

    function __construct(&$config, $dn)
    {
        parent::__construct($config, $dn);
        $this->DisplayName = _("FAX database");
    }


    function execute()
    {
        $smarty = get_smarty();

        if ($this->is_account && !$this->view_logged) {
            $this->view_logged = TRUE;
            new log("view", "server/" . get_class($this), $this->dn);
        }

        /* Assign acls */
        $tmp = $this->plInfo();
        foreach ($tmp['plProvidedAcls'] as $name => $translation) {
            $smarty->assign($name . "ACL", $this->getacl($name));
        }

        foreach ($this->attributes as $attr) {
            $smarty->assign($attr, set_post($this->$attr));
        }
        return ($smarty->fetch(get_template_path("goFaxServer.tpl", TRUE, dirname(__FILE__))));
    }


    function getListEntry()
    {
        $fields               = parent::getListEntry();
        $fields['Message']    = _("FAX database configuration");
        #$fields['AllowEdit']  = true;
        return ($fields);
    }

    function check()
    {
        $message = plugin::check();
        if (empty($this->goFaxAdmin)) {
            $message[] = msgPool::required(_("User"));
        }
        if (empty($this->goFaxPassword)) {
            $message[] = msgPool::required(_("Password"));
        }
        return ($message);
    }


    function save_object()
    {
        if (isset($_POST['goFaxServerPosted'])) {
            plugin::save_object();
        }
    }

    /* Return plugin informations for acl handling */
    static function plInfo()
    {
        return (array(
            "plShortName"   => _("Fax database"),
            "plDescription" => _("Fax database") . " (" . _("Services") . ")",
            "plSelfModify"  => FALSE,
            "plDepends"     => array(),
            "plPriority"    => 99,
            "plSection"     => array("administration"),
            "plCategory"    => array("server"),
            "plRequirements" => array(
                'ldapSchema' => array('goFaxServer' => '>=2.7'),
                'onFailureDisablePlugin' => array(get_class(), 'faxreport', 'gofaxAccount')
            ),

            "plProvidedAcls" => array(
                "goFaxAdmin"     => _("Login name"),
                "goFaxPassword"  => _("Password"),

                "start"         => _("Start"),
                "stop"          => _("Stop"),
                "restart"       => _("Restart")
            )
        ));
    }
}
