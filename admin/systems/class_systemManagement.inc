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
      if(!empty($entry['RDN']))
      $sP[] = $entry['RDN'];
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

    // Add copy&paste and snapshot handler.
    if ($this->config->boolValueIsTrue("main", "copyPaste")){
      $this->cpHandler = new CopyPasteHandler($this->config);
    }
    if($this->config->get_cfg_value("enableSnapshots") == "true"){
      $this->snapHandler = new SnapshotHandler($this->config);
    }
    parent::__construct($config, $ui, "roles", $headpage);
  }


  
  /*! \brief   Overridden render method of class mangement.
   *            this allows us to add a release selection box.
   */
  function renderList()
  {
    $headpage = $this->getHeadpage();

    $tD = $this->getObjectDefinitions();
    $smarty = get_smarty();
    foreach($tD as $name => $obj){
#      $smarty->assign("USE_".$name, (empty($obj['CLASS']) || class_available($obj['CLASS'])));
    }

    $display = $headpage->render();
    return($this->getHeader().$display);
  }


  public function getObjectDefinitions()
  {
    $tabs = array(
        "incoming"    => array(
          "RDN"       => get_ou('systemIncomingRDN'),
          "CLASS"     => "",
          "TABNAME"   => "",
          "TABCLASS"  => "",
          "ACLC"      => "incoming",
          "ACL"       => "incoming/systems"),

        "ArpNewDevice"=> array(
          "RDN"       => get_ou('systemIncomingRDN'),
          "CLASS"     => "TERMTABS",
          "TABNAME"   => "termgeneric" ,
          "TABCLASS"  => "termtabs",
          "ACLC"      => "incoming",
          "ACL"       => "incoming/systems"),

        "NewDevice"   => array(
          "RDN"       => get_ou('systemIncomingRDN'),
          "CLASS"     => "TERMTABS",
          "TABNAME"   => "termgeneric",
          "TABCLASS"  => "termtabs",
          "ACLC"      => "incoming",
          "ACL"       => "incoming/systems"),

        "terminal"    => array(
            "RDN"       => get_ou('terminalRDN'),
            "CLASS"     => "TERMTABS",
            "TABNAME"   => "termgeneric",
            "TABCLASS"  => "termtabs",
            "ACLC"      => "terminal",
            "ACL"       => "terminal/termgeneric"),

        "workstation" =>  array(
            "RDN"       => get_ou('workstationRDN'),
            "CLASS"   => "WORKTABS",
            "TABNAME" => "workgeneric",
            "TABCLASS"=> "worktabs",
            "ACLC"    => "workstation",
            "ACL"     => "workstation/workgeneric" ),

        "server"      => array(
            "RDN"       => get_ou('serverRDN'),
            "CLASS"   => "SERVTABS",
            "TABNAME" => "servgeneric",
            "TABCLASS"=> "servtabs",
            "ACLC"    => "server",
            "ACL"     => "server/servgeneric"),

        "printer"     => array(
            "RDN"       => get_ou('printerRDN'),
            "CLASS"   => "PRINTTABS",
            "TABNAME" => "printgeneric",
            "TABCLASS"=> "printtabs",
            "ACLC"    => "printer",
            "ACL"     => "printer/printgeneric"),

        "phone"       => array(
            "RDN"       => get_ou('phoneRDN'),
            "CLASS"   => "PHONETABS",
            "TABNAME" => "phoneGeneric",
            "TABCLASS"=> "phonetabs",
            "ACLC"    => "phone",
            "ACL"     => "phone/phoneGeneric"),

        "winstation"  => array(
            "RDN"       => get_winstations_ou(),
            "CLASS"   => "WINTABS",
            "TABNAME" => "wingeneric",
            "TABCLASS"=> "wintabs",
            "ACLC"    => "winworkstation",
            "ACL"     => "winworkstation/wingeneric"),

        "component"   => array(
            "RDN"       => get_ou('componentRDN'),
            "CLASS"   => "COMPONENTTABS",
            "TABNAME" => "componentGeneric",
            "TABCLASS"=> "componenttabs",
            "ACLC"    => "component",
            "ACL"     => "component/componentGeneric"));

    if($this->opsi != NULL){
      $tabs["opsi_client"] = array(
          "RDN"       => "",
          "CLASS"   => "OPSITABS",
          "TABNAME" => "opsiGeneric" ,
          "TABCLASS"=> "opsi_tabs",
          "ACLC"    => "opsi" ,
          "ACL"     => "opsi/opsiGeneric");
    }

    return($tabs);
  }


} 
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
