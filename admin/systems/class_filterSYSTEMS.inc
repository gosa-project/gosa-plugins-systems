<?php

class filterSYSTEMS {

  static function query($base, $scope, $filter, $attributes, $category, $objectStorage= "")
  {
    $entries = filterLDAP::query($base, $scope, $filter, $attributes, $category, $objectStorage);

    foreach($entries as $key => $entry){
      if(preg_match("/".preg_quote(get_ou('systemIncomingRDN'),'/')."/i", $entry['dn'])){

        // Detect incoming types
        if(in_array('gotoWorkstation', $entry['objectClass'])){
          $entries[$key]['objectClass'][] = 'FAKE_OC_NewWorkstation';
          $entries[$key]['cn'][0].= " <i>("._("New workstation").")</i>";
        }elseif(in_array('gotoTerminal', $entry['objectClass'])){
          $entries[$key]['objectClass'][] = 'FAKE_OC_NewTerminal';
          $entries[$key]['cn'][0].= " <i>("._("New terminal").")</i>";
        }elseif(in_array('GOhard', $entry['objectClass']) && !isset($entry['gotoMode'][0])){ 
          $entries[$key]['objectClass'][] = 'FAKE_OC_ArpNewDevice';
          $entries[$key]['cn'][0].= " <i>("._("New unknown device").")</i>";
        }elseif(in_array('GOhard', $entry['objectClass'])){
          $entries[$key]['objectClass'][] = 'FAKE_OC_NewDevice';
          $entries[$key]['cn'][0].= " <i>("._("New device").")</i>";
        }

      // Detect SAMBA machines
      }elseif(in_array('sambaSamAccount', $entry['objectClass']) && preg_match("/\$$/", $entry['cn'][0])){
        $entries[$key]['objectClass'][] = 'FAKE_OC_winstation';

      // Detect Workstation templates
      }elseif(in_array('gotoWorkstation', $entry['objectClass'])){
        if(!isset($entry['macAddress'][0]) || $entry['macAddress'][0] == "-"){
          $entries[$key]['objectClass'][] = 'FAKE_OC_WorkstationTemplate';
        }

      // Detect Terminal templates
      }elseif(in_array('gotoTerminal', $entry['objectClass'])){
        if(!isset($entry['macAddress'][0]) || $entry['macAddress'][0] == "-"){
          $entries[$key]['objectClass'][] = 'FAKE_OC_TerminalTemplate';
        }

      // Detect Server templates
      }elseif(in_array('goServer', $entry['objectClass'])){
        if(!isset($entry['macAddress'][0]) || $entry['macAddress'][0] == "-"){
          $entries[$key]['objectClass'][] = 'FAKE_OC_ServerTemplate';
        }
      }

      // Detect object states
      foreach(array('gotoWorkstation','gotoTerminal','goServer') as $type){
        if(in_array($type, $entry['objectClass'])){

          if(isset($entry['gotoMode'][0]) && preg_match("/locked/i", $entry['gotoMode'][0])){
            $entries[$key]['objectClass'][] = $type."__IS_LOCKED";
          }elseif(isset($entry['FAIstate'][0])){
            switch (preg_replace('/:.*$/', '', $entry['FAIstate'][0])) {
              case 'error':
                $state= '__IS_ERROR';
                break;
              case 'installing':
              case 'install':
              case 'sysinfo':
              case 'softupdate':
              case 'scheduledupdate':
                $state= '__IS_BUSY';
                break;
              default: $state ="";
            }
            if(!empty($state)) $entries[$key]['objectClass'][] = $type.$state;
          }
        }
      }
    }

    // Append opsi stuff ...
    global $config;
    $ui = get_userinfo();
    if(preg_match("/FAIstate=__NETBOOT__/", $filter) && 
        class_available('opsi') &&
        $base == $config->current['BASE']){

      // Check ACLs and opsi handle
      $opsi_acl = $ui->get_permissions($base,"opsi/opsiProperties");
      $opsi = new opsi($config);
      $objects = array();
      if($opsi instanceof opsi && $opsi->enabled() && preg_match("/r/",$opsi_acl)){

        // Get list of opsi hosts 
        $opsi_clients = $opsi->get_hosts_for_system_management();
        if($opsi->is_error()){
          msg_dialog::display(_("Error"),msgPool::siError($opsi->get_error()),ERROR_DIALOG);
        }else{
          foreach($opsi_clients as $entry){
            $entry['objectClass'] = array("FAKE_OC_OpsiHost");
            $item = array('count' => count($entry));
            foreach($entry as  $name => $value){
              $item[] = $name;
              $item[$name] = $value;
            }
            $entries[] = $item;
          }
        }
      }
    }
    return($entries);
  }
}

?>
