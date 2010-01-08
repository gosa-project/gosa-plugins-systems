<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id$$
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

class systemManagement extends management
{
  var $plHeadline     = "Systems";
  var $plDescription  = "List of systems";
  var $plIcon  = "plugins/systems/images/plugin.png";

  // Tab definition 
  protected $tabClass = "";
  protected $tabType = "";
  protected $aclCategory = "";
  protected $aclPlugin   = "";
  protected $objectName   = "";

  protected $objectInfo = array();

  protected $opsi = NULL;

  function __construct($config,$ui)
  {
    $this->config = $config;
    $this->ui = $ui;

    // Set storage points 
    $tD = $this->getObjectDefinitions(); 
    $sP = array();
    foreach($tD as $entry){
      if(!empty($entry['ou']))
        $sP[] = $entry['ou'];
    }
    $this->storagePoints = array_unique($sP);

#    // Build filter
#    if (session::global_is_set(get_class($this)."_filter")){
#      $filter= session::global_get(get_class($this)."_filter");
#    } else {
  $filter = new filter(get_template_path("system-filter.xml", true));
  $filter->setObjectStorage($this->storagePoints);
#    }
  $this->setFilter($filter);

  // Build headpage
  $headpage = new listing(get_template_path("system-list.xml", true));
  $headpage->setFilter($filter);

  $filter->setConverter('INCOMING', 'systemManagement::incomingFilterConverter');

  // Add copy&paste and snapshot handler.
  if ($this->config->boolValueIsTrue("main", "copyPaste")){
    $this->cpHandler = new CopyPasteHandler($this->config);
  }
  if($this->config->get_cfg_value("enableSnapshots") == "true"){
    $this->snapHandler = new SnapshotHandler($this->config);
  }
  parent::__construct($config, $ui, "systems", $headpage);
  }

  static function incomingFilterConverter($filter)
  {
    $rdn = preg_replace("/^[^=]*=/", "", get_ou('systemIncomingRDN'));
    $rdn = preg_replace("/,.*$/","",$rdn);
    return(preg_replace("/%systemIncomingRDN/", $rdn,$filter));
  }

  function editEntry($action="",$target=array(),$all=array(), $altTabClass ="", $altTabType = "", $altAclCategory="")
  {
    if(count($target) == 1){
      $tInfo = $this->getObjectDefinitions();
      $headpage = $this->getHeadpage();
      $dn = $target[0];
      $type = $tInfo[$headpage->getType($dn)];
      return(management::editEntry($action,$target,$all,$type['tabClass'],$type['tabDesc'],$type['aclCategory']));
    }
  }



  /*! \brief   Overridden render method of class mangement.
   *            this allows us to add a release selection box.
   */
  function renderList()
  {
    $headpage = $this->getHeadpage();
    $headpage->update();

    $tD = $this->getObjectDefinitions();
    $smarty = get_smarty();
    foreach($tD as $name => $obj){
      $smarty->assign("USE_".$name, (empty($obj['TABNAME']) || class_available($obj['TABNAME'])));
    }

    $display = $headpage->render();
    return($this->getHeader().$display);
  }


  public function getObjectDefinitions()
  {
    $tabs = array(
        "FAKE_OC_OpsiHost" => array(
          "ou"          => "",
          "plugClass"   => "opsiGeneric",
          "tabClass"    => "opsi_tabs",
          "tabDesc"     => "OPSITABS",
          "aclClass"    => "opsiGeneric",
          "aclCategory" => "opsi"),

        "goServer" => array(
          "ou"          => get_ou('serverRDN'),
          "plugClass"   => "servgeneric",
          "tabClass"    => "servtabs",
          "tabDesc"     => "SERVTABS",
          "aclClass"    => "servgeneric",
          "aclCategory" => "server"),

        "gotoWorkstation" => array(
          "ou"          => get_ou('workstationRDN'),
          "plugClass"   => "workgeneric",
          "tabClass"    => "worktabs",
          "tabDesc"     => "WORKTABS",
          "aclClass"    => "workstation",
          "aclCategory" => "workgeneric"),

        "gotoTerminal" => array(
            "ou"          => get_ou('terminalRDN'),
            "plugClass"   => "termgeneric",
            "tabClass"    => "termtabs",
            "tabDesc"     => "TERMTABS",
            "aclClass"    => "terminal",
            "aclCategory" => "termgeneric"),

        "gotoPrinter" => array(
            "ou"          => get_ou('printerRDN'),
            "plugClass"   => "printgeneric",
            "tabClass"    => "printtabs",
            "tabDesc"     => "PRINTTABS",
            "aclClass"    => "printer",
            "aclCategory" => "printgeneric"),

        "FAKE_OC_NewDevice" => array(
            "ou"          => get_ou('systemIncomingRDN'),
            "plugClass"   => "termgeneric",
            "tabClass"    => "termtabs",
            "tabDesc"     => "TERMTABS",
            "aclClass"    => "terminal",
            "aclCategory" => "termgeneric"),

        "goFonHardware" => array(
            "ou"          => get_ou('phoneRDN'),
            "plugClass"   => "phoneGeneric",
            "tabClass"    => "phonetabs",
            "tabDesc"     => "PHONETABS",
            "aclClass"    => "phone",
            "aclCategory" => "phoneGeneric"),

        "FAKE_OC_winstation" => array(
            "ou"          => get_winstations_ou(),
            "plugClass"   => "wingeneric",
            "tabClass"    => "wintabs",
            "tabDesc"     => "WINTABS",
            "aclClass"    => "wingeneric",
            "aclCategory" => "winworkstation"),

        "ieee802Device" => array(
            "ou"          => get_ou('componentRDN'),
            "plugClass"   => "componentGeneric",
            "tabClass"    => "componenttabs",
            "tabDesc"     => "COMPONENTTABS",
            "aclClass"    => "componentGeneric",
            "aclCategory" => "component"),
        );

    // Now map some special types
    $tabs['FAKE_OC_NewWorkstation'] = &$tabs['gotoWorkstation'];
    $tabs['FAKE_OC_NewTerminal'] = &$tabs['gotoTerminal'];
    $tabs['FAKE_OC_NewServer'] = &$tabs['gotoWorkstation'];
    $tabs['gotoWorkstation__IS_BUSY'] = &$tabs['gotoWorkstation'];
    $tabs['gotoWorkstation__IS_ERROR'] = &$tabs['gotoWorkstation'];
    $tabs['gotoWorkstation__IS_LOCKED'] = &$tabs['gotoWorkstation'];
    $tabs['gotoTerminal__IS_BUSY'] = &$tabs['gotoTerminal'];
    $tabs['gotoTerminal__IS_ERROR'] = &$tabs['gotoTerminal'];
    $tabs['gotoTerminal__IS_LOCKED'] = &$tabs['gotoTerminal'];
    $tabs['goServer__IS_BUSY'] = &$tabs['goServer'];
    $tabs['goServer__IS_ERROR'] = &$tabs['goServer'];
    $tabs['goServer__ISLOCKED'] = &$tabs['goServer'];

    $tabs['FAKE_OC_NewUnknownDevice'] = &$tabs['FAKE_OC_NewDevice'];

    return($tabs);
  }
} 
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
