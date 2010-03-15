<?php

class filterOPSIHOSTS {

  static function query($base, $scope, $filter, $attributes, $category, $objectStorage= "")
  {
    // Append opsi stuff ...
    global $config;
    $ui = get_userinfo();

    echo $filter;

    if(preg_match("/NETBOOT/", $filter) && 
        class_available('opsi') &&
        $base == $config->current['BASE']){

      // Get filter regex ...
      $regex = "";
      if(preg_match("/\(cn=[^\)\$]*\)/", $filter)){
        $regex = trim(preg_replace("/^.*\(cn=([^\)\$]*)\).*$/","\\1", $filter),"* ");
      }

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
            if(empty($regex) || preg_match('/'.preg_quote($regex,'/').'/i',$entry['cn'][0])){
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
    }
    return($entries);
  }
}

?>
