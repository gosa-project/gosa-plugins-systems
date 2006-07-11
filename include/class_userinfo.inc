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

#    echo "NEW ACL LOADING --------------------------------------------------------------------------------------------<br>";
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
      $tree= split(',', $attrs['dn']);
      $tree= array_reverse($tree);

      $interresting= FALSE;
      $t= acl::explodeAcl($attrs['gosaAclEntry'][0]);
#      print_a($t);

      /* We're interested in ACLs apply for *all* users... */

      /* ... for groups we're member in... */

      /* and for our DN as member. */


      $aclp[$attrs['dn']]= substr_count($attrs['dn'], ',');
      $aclc[$attrs['dn']]= $attrs['gosaAclEntry'];

    }

    /* ACL's read, sort for tree depth */
    asort($aclp);

#print_a($aclp);
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
