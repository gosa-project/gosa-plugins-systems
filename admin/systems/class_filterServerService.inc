<?php

class filterServerService {

  static function query($base, $scope, $filter, $attributes, $category, $objectStorage= "")
  {
    $plugins = ServerService::getServiceList();
    $list = array();
    foreach($plugins as $name => $obj){
      if($obj->is_account){
        $data = $plugins[$name]->getListEntry();
        $data['objectClass'] = array('FAKE_OC_ServerService'); 
        $data['name'] = $name;
        foreach($data as $key => $value){
          if(!is_array($data[$key])) $data[$key] = array($data[$key]);
          $list[$name][] = $key;
          $list[$name][$key] = $data[$key];
          $list[$name][$key]['count'] = count($data[$key]);
          $list[$name]['count'] = count($data);
          $list[$name]['dn'] = $key;
        }
      }
    }
    return($list);
  }
}

?>
