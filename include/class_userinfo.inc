<?php
/*
   This code is part of GOsa (https://gosa.gonicus.de)
   Copyright (C) 2003-2005  Cajus Pollmeier

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

class userinfo
{
  var $dn;
  var $ip;
  var $username;
  var $cn;
  var $uid;
  var $gidNumber= -1;
  var $language= "";
  var $config;
  var $gosaUnitTag= "";
  var $subtreeACL= array();
  var $ACL= array();
  var $groups= array();

  /* get acl's an put them into the userinfo object
     attr subtreeACL (userdn:components, userdn:component1#sub1#sub2,component2,...) */
  function userinfo($config, $userdn){
    $this->config= $config;
    $ldap= $this->config->get_ldap_link();
    $ldap->cat($userdn,array('sn', 'givenName', 'uid', 'gidNumber', 'preferredLanguage', 'gosaUnitTag'));
    $attrs= $ldap->fetch();

    if (isset($attrs['givenName'][0]) && isset($attrs['sn'][0])){
      $this->cn= $attrs['givenName'][0]." ".$attrs['sn'][0];
    } else {
      $this->cn= $attrs['uid'][0];
    }
    if (isset($attrs['gidNumber'][0])){
      $this->gidNumber= $attrs['gidNumber'][0];
    }

    /* Assign user language */
    if (isset($attrs['preferredLanguage'][0])){
      $this->language= $attrs['preferredLanguage'][0];
    }

    if (isset($attrs['gosaUnitTag'][0])){
      $this->gosaUnitTag= $attrs['gosaUnitTag'][0];
    }

    $this->dn= $userdn;
    $this->uid= $attrs['uid'][0];
    $this->ip= $_SERVER['REMOTE_ADDR'];
  }


  function loadACL()
  {

#--------------------------------------------------------------------------OLD-BUT-ACTIVE-----------------------------
    $ldap= $this->config->get_ldap_link();

    /* Load ACL's from all groups we're in */
    $this->subtreeACL= array();
    $ldap->cd($this->config->current['BASE']);
    if ($this->gidNumber == -1){
      $ldap->search("(&(objectClass=posixGroup)(objectClass=gosaObject)".
          "(memberUid=$this->username))");
    } else {
      $ldap->search("(&(objectClass=posixGroup)(objectClass=gosaObject)".
          "(|(memberUid=$this->username)(gidNumber=$this->gidNumber)))");
    }

    while($attrs = $ldap->fetch()){
      $base= preg_replace('/^[^,]+,ou=[^,]+,/i', "",$ldap->getDN());
      $base= preg_replace("/[ ]*,[ ]*/", ",", $base);

      for ($i= 0; $i<$attrs["gosaSubtreeACL"]["count"]; $i++){
        $this->subtreeACL[$base][]= $attrs["gosaSubtreeACL"][$i];
      }
    }
#echo "NEW ACL LOADING --------------------------------------------------------------------------------------------<br>";

    $this->ACL= array();    
    $this->groups= array();    
    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);

    /* Get member groups... */
    $ldap->search("(&(objectClass=posixGroup)(memberUid=".$this->uid."))", array('dn'));
    while ($attrs= $ldap->fetch()){
      $this->groups[$attrs['dn']]= $attrs['dn'];
    }

    /* Crawl through ACLs and move relevant to the tree */
    $ldap->search("(objectClass=gosaACL)", array('dn', 'gosaAclEntry'));
    $aclp= array();
    $aclc= array();
    while ($attrs= $ldap->fetch()){

      /* Insert links in ACL array */
      $aclp[$attrs['dn']]= substr_count($attrs['dn'], ',');
      $aclc[$attrs['dn']]= array();
      $ol= array();
      for($i= 0; $i<$attrs['gosaAclEntry']['count']; $i++){
        $ol= array_merge($ol, @acl::explodeAcl($attrs['gosaAclEntry'][$i]));
      }
      $aclc[$attrs['dn']]= $ol;
    }

    /* ACL's read, sort for tree depth */
    asort($aclp);

    /* Sort in tree order */
    foreach ($aclp as $dn => $acl){
      /* Check if we need to keep this ACL */
      foreach($aclc[$dn] as $idx => $type){
        $interresting= FALSE;
        
        /* No members? This is good for all users... */
        if (!count($type['members'])){
          $interresting= TRUE;
        }

        /* Inspect members... */
        foreach ($type['members'] as $grp => $grpdsc){
          /* Some group inside the members that is relevant for us? */
          if (in_array_ics(preg_replace('/^G:/', '', $grp), $this->groups)){
            $interresting= TRUE;
          }

          /* User inside the members? */
          if (preg_replace('/^U:/', '', $grp) == $this->dn){
            $interresting= TRUE;
          }
        }

        if ($interresting){
          if (!isset($this->ACL[$dn])){
            $this->ACL[$dn]= array();
          }
          $this->ACL[$dn][$idx]= $type;
        }
      }

    }
  }


  function get_permissions($dn, $object, $attribute, $skip_write= FALSE)
  {
    $acl= array("r" => "", "w" => "", "c" => "", "d" => "", "m" => "", "a" => "");

    /* Build dn array */
    $path= split(',', $dn);
    $path= array_reverse($path);

    /* Walk along the path to evaluate the acl */
    $cpath= "";
    foreach ($path as $element){

      /* Clean potential ACLs for each level */
      $acl= $this->cleanACL($acl);

      if ($cpath == ""){
        $cpath= $element;
      } else {
        $cpath= $element.','.$cpath;
      }
      if (isset($this->ACL[$cpath])){

        /* Inspect this ACL, place the result into ACL */
        foreach ($this->ACL[$cpath] as $subacl){

          /* Reset? Just clean the ACL and turn over to the next one... */
          if ($subacl['type'] == 'reset'){
            $acl= $this->cleanACL($acl, TRUE);
            continue;
          }

          /* Per attribute ACL? */
          if (isset($subacl['acl'][$object][$attribute])){
            $acl= $this->mergeACL($acl, $subacl['type'], $subacl['acl'][$object][$attribute]);
            continue;
          }

          /* Per object ACL? */
          if (isset($subacl['acl'][$object][0])){
            $acl= $this->mergeACL($acl, $subacl['type'], preg_replace('/[cdm]/', '', $subacl['acl'][$object][0]));
            continue;
          }

          /* Global ACL? */
          if (isset($subacl['acl']['all'])){
            $acl= $this->mergeACL($acl, $subacl['type'], preg_replace('/[cdm]/', '', $subacl['acl']['all'][0]));
            continue;
          }

        }

      }
    }

    /* Assemble string */
    $ret= "";
    foreach ($acl as $key => $value){
      if ($value != ""){
        $ret.= $key;
      }
    }

    /* Remove write if needed */
    if ($skip_write){
      $ret= preg_replace('/w/', '', $ret);
    }

    return ($ret);
  }


  function get_module_departments($module)
  {
    global $plist;

    $objects= array();
    $deps= array();

    /* Extract all relevant objects for this module from plist */
    foreach ($plist->info as $object => $info){
      foreach ($info['plCategory'] as $idx => $data){
        if (preg_match('/^[0-9]+$/', $idx)){
          if ($data == $module){
            $objects[$object]= $object;
          }
        } else {
          if ($idx == $module){
            $objects[$object]= $object;
          }
        }
      }
    }

    /* Get all gosaDepartments */
    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    $ldap->search('objectClass=gosaDepartment', array('dn'));
    while ($attrs= $ldap->fetch()){
      $acl= array("r" => "", "w" => "", "c" => "", "d" => "", "m" => "", "a" => "");

      /* Build dn array */
      $path= split(',', $attrs['dn']);
      $path= array_reverse($path);

      /* Walk along the path to evaluate the acl */
      $cpath= "";
      foreach ($path as $element){

        /* Clean potential ACLs for each level */
        $acl= $this->cleanACL($acl);

        if ($cpath == ""){
          $cpath= $element;
        } else {
          $cpath= $element.','.$cpath;
        }
        if (isset($this->ACL[$cpath])){

          /* Inspect this ACL, place the result into ACL */
          foreach ($this->ACL[$cpath] as $subacl){

            /* Reset? Just clean the ACL and turn over to the next one... */
            if ($subacl['type'] == 'reset'){
              $acl= $this->cleanACL($acl, TRUE);
              continue;
            }

            /* Per object ACL? */
            foreach ($objects as $object){
              if (isset($subacl['acl'][$object])){
                foreach($subacl['acl'][$object] as $attribute => $dcl){
                  if (isset($subacl['acl'][$object][$attribute])){
                    $acl= $this->mergeACL($acl, $subacl['type'], preg_replace('/[cdm]/', '', $subacl['acl'][$object][$attribute]));
                  }
                }
              }
            }

            /* Global ACL? */
            if (isset($subacl['acl'][0])){
              $acl= $this->mergeACL($acl, $subacl['type'], preg_replace('/[cdm]/', '', $subacl['acl'][0]));
              continue;
            }
          }
        }
      }

      /* Add department, if we have (some) permissions for the requred module */
      foreach ($acl as $val){
        if ($val != ""){
          $deps[]= $attrs['dn'];
          break;
        }
      }
    }

    return ($deps);
  }


  function mergeACL($acl, $type, $newACL)
  {
    foreach(str_split($newACL) as $char){

      /* Ignore invalid characters */
      if (!preg_match('/[rwcdm]/', $char)){
        continue;
      }

      /* Skip permanent and subtree entries */
      if (preg_match('/[sp]/', $acl[$char])){
        continue;
      }

      switch ($type){
        case 'psub':
          $acl[$char]= 'p';
          break;

        case 'sub':
          $acl[$char]= 's';
          break;

        case 'one':
          $acl[$char]= 1;
          break;

        case 'base':
          if ($acl[$char] != 1){
            $acl[$char]= 0;
          }
          break;
      }
    }

    return ($acl);
  }


  function cleanACL($acl, $reset= FALSE)
  {
    foreach ($acl as $key => $value){

      /* Reset removes everything but 'p' */
      if ($reset && $value != 'p'){
        $acl[$key]= "";
        continue;
      }

      /* Decrease tree level */
      if (preg_match('/^[0-9]+$/', $value)){
        if ($value > 0){
          $acl[$key]= $value - 1;
        } else {
          $acl[$key]= "";
        }
      }
    }

    return ($acl);
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
