<?php

/* TEST PHASE .... */

/* Returns all object for the given release.
   This function resolves the release management 
   from base to the given dn.
 */
function get_all_objects_for_given_base($Current_DN,$filter,$detailed = false)
{
  global $config;
  $ldap = $config->get_ldap_link();
  $ldap->cd($config->current['BASE']);

  /* Collect some basic informations and initialize some variables */ 
  $base_release       = get_release_dn($Current_DN);
  $previous_releases  = array_reverse(get_previous_releases_of_this_release($base_release,true));
  $res = array();
  $tmp = array();

  /* We must also include the given release dn */
  $previous_releases[] = $base_release;

  /* Walk through all releases */
  foreach($previous_releases as $release){

    /* Get fai departments */
    $deps_to_search = get_FAI_departments($release); 

    /* For every single department  (ou=hoos,ou ..) */
    foreach($deps_to_search as $fai_base){

      /* Ldap search for fai classes specified in this release */
      $ldap->cd($fai_base);
      $ldap->search($filter,array("dn","objectClass"));

      /* check the returned objects, and add/replace them in our return vareable */
      while($attr = $ldap->fetch()){

        $buffer = array();
        $name = str_ireplace($release,"",$attr['dn']);

        /* In detailed mode are some additonal informations visible */
        if($detailed){

          /* Create list of parents */
          if(isset($res[$name])){
            $buffer = $res[$name];
            $buffer['parents'][] = $res[$name]['dn'];
          }else{
            $buffer['parents'] = array();
          }

          /* Append objectClass to resulsts */
          $buffer['objectClass']  = $attr['objectClass'];
          unset($buffer['objectClass'][0]);
        }

        /* Add this object to our list */
        $buffer['dn']           = $attr['dn'];
        $res[$name] = $buffer;
      }
    }
  }
  return($res);
}



/* Return the object defining ObjectClass e.g. FAIscriptEntry */
function get_FAI_type($attr)
{
  $arr = array( "FAIprofile","FAIpartitionTable", "FAIpartitionDisk","FAIpartitionEntry","FAIhook","FAIhookEntry",
      "FAIscriptEntry","FAIscript","FAIvariable","FAIvariableEntry","FAIpackageList","FAItemplate",
      "FAItemplateEntry","FAIdebconfInfo","FAIrepository","FAIrepositoryServer","FAIbranch","FAIreleaseTag");  
  foreach($arr as $name){    
    if(in_array($name,$attr['objectClass'])){
      preg_match("");
      return($name);
    }
  }
  echo "Not found";
  return("");
} 


/* Return all relevant FAI departments */
function get_FAI_departments($suffix = "")
{
  $arr = array("hooks","scripts","disk","packages","profiles","templates","variables");
  $tmp = array();
  if(preg_match("/^,/",$suffix)){
    $suffix = preg_replace("/^,/","",$suffix);
  }
  foreach($arr as $name){
    if(empty($suffix)){
      $tmp[$name] = "ou=".$name;
    }else{
      $tmp[$name] = "ou=".$name.",".$suffix;
    }
  }
  return($tmp);
}


function get_all_releases_from_base($dn)
{
  global $config;
  $base = "ou=fai,ou=configs,ou=systems,".$dn;
  $res = array();  
  
  $ldap = $config->get_ldap_link();
  $ldap->cd($base);
  $ldap->search("(objectClass=FAIbranch)",array("ou","dn"));
  while($attrs = $ldap->fetch()){
    $res[$attrs['dn']] = $attrs['ou'][0];
  } 
  return($res);
}


/* This function does everything to be able to save the given dn.

    !!!!!!! No functionality just output 
  
 */
function prepare_to_save_FAI_object($Current_DN)
{
  /* Get some basic informations */
  $base_release = get_release_dn($Current_DN);
  $sub_releases = get_sub_releases_of_this_release($base_release,true);
  $parent_obj   = get_parent_release_object($Current_DN);
  
  $previous_releases = get_previous_releases_of_this_release($base_release,true);
  $following_releases= get_sub_releases_of_this_release($base_release,true);
  
  global $config;
  $ldap = $config->get_ldap_link();
  $ldap->cd($config->current['BASE']);
  $ldap->cat($Current_DN);
  if(!$ldap->count()){
    $is_new = true;
  }else{
    $is_new = false;
  }
 
  echo "<h2>".$Current_DN."</h2>";
 
  /* If we are a leaf object */
  if(count($following_releases) == 0 ){
    echo "<b>Saving dircetly, is a leaf object</b><br>";
  }else{
   
    $r = get_following_releases_that_inherit_this_object($Current_DN);

    if($is_new){
      echo "Creating an empty entry for <b>".$r[key($r)]."</b><br>";
      echo "By setting <b>'FAIstate'</b> to <b>'*|removed'</b><br>"; 
    }else{
      echo "<b>This object has sub releases: </b><br>";
      echo "This object must be updated <b>".$r[key($r)]."</b> with <b>".$parent_obj."</b><br>";
    }
    unset($r[key($r)]);
    echo "<br> the result will be inherited by .";
    foreach($r as $release => $value){
      echo "<br>&nbsp; : <i>".$release."</i>";
    }
  } 
  echo "<br>.--------------------------------------------------------------------.<br>";
}


function get_following_releases_that_inherit_this_object($dn)
{
  global $config;
  $ldap = $config->get_ldap_link();
  $ldap->cd($config->current['BASE']);

  $ret = array();

  /* Get base release */
  $base_release = get_release_dn($dn);

  /* Get previous release dns */
  $sub_releases = get_sub_releases_of_this_release($base_release,true);

  /* Get dn suffix. Example  "FAIvairableEntry=keksdose,FAIvariable=Keksregal," */
  $dn_suffix = str_ireplace($base_release,"",$dn);
    
  /* Check if given object also exists whitin one of these releases */
  foreach($sub_releases as $p_release => $name){

    $check_dn = $dn_suffix.$p_release;
  
    $ldap->cat($check_dn,array("dn","objectClass"));
    
    if($ldap->count()){
      return($ret);
    }else{
      $ret[$check_dn]=$check_dn;
    }
  }
  return($ret);
}


/* Get previous version of the object dn */
function get_parent_release_object($dn)
{
  global $config;
  $ldap = $config->get_ldap_link();
  $ldap->cd($config->current['BASE']);
  $previous_releases= array();

  /* Get base release */
  $base_release = get_release_dn($dn);
  $previous_releases[] = $base_release;  

  /* Get previous release dns */
  $tmp = get_previous_releases_of_this_release($base_release,true);
  foreach($tmp as $release){
    $previous_releases[] = $release;
  }

  /* Get dn suffix. Example  "FAIvairableEntry=keksdose,FAIvariable=Keksregal," */
  $dn_suffix = str_ireplace($base_release,"",$dn);
    
  /* Check if given object also exists whitin one of these releases */
  foreach($previous_releases as $p_release){
    $check_dn = $dn_suffix.$p_release;
    $ldap->cat($check_dn,array("dn","objectClass"));
    
    if($ldap->count()){
      return($check_dn);
    }
  }
  return("");
}


/* return release names of all parent releases */
function get_previous_releases_of_this_release($dn,$flat)
{
  global $config;
  $ldap = $config->get_ldap_link();
  $ldap->cd($config->current['BASE']);
  $ret = array();

  /* Explode dns into pieces, to be able to build parent dns */
  $dns_to_check = ldap_explode_dn(str_ireplace(",".$config->current['BASE'],"",$dn),0);
  if(!is_array($dns_to_check)){
    return;  
  }

  /* Unset first entry which represents the given dn */
  unset($dns_to_check['count']); 
  unset($dns_to_check[key($dns_to_check)]);

  /* Create dns addresses and check if this dn is a release dn */
  $id = 0;
  while(count($dns_to_check)){

    /* build parent dn */
    $new_dn = "";
    foreach($dns_to_check as $part){
      $new_dn .= $part.",";
    }
    $new_dn .= $config->current['BASE'];

    /* check if this dn is a release */
    if(is_release_department($new_dn)){
      if($flat){
        $ret[$id] = $new_dn; 
      }else{
        $ret = array($new_dn=>$ret); 
      }
      $id ++;
    }else{
      return($ret);
    }
    reset($dns_to_check);
    unset($dns_to_check[key($dns_to_check)]);
  }
  return($ret);
} 


/* This function returns all sub release names, recursivly  */
function get_sub_releases_of_this_release($dn,$flat = false)
{
  global $config;
  $res  = array();
  $ldap = $config->get_ldap_link();
  $ldap->cd($config->current['BASE']);
  $ldap->ls("(objectClass=FAIbranch)",$dn,array("objectClass","dn","ou"));
  while($attr = $ldap->fetch()){

    /* Append department name */
    if($flat){
      $res[$attr['dn']] = $attr['ou'][0];
    }else{
      $res[$attr['dn']] = array();
    }

    /* Get sub release departments of this department */
    if(in_array("FAIbranch",$attr['objectClass'])) {
      if($flat){
        $tmp = get_sub_releases_of_this_release($attr['dn'],$flat);
        foreach($tmp as $dn => $value){
          $res[$dn]=$value;
        }
      }else{
        $res[$attr['dn']] = get_sub_releases_of_this_release($attr['dn']);
      }
    }
  }
  return($res);
}


/* Check if the given department is a release department */
function is_release_department($dn)
{
  global $config;
  $ldap = $config->get_ldap_link();
  $ldap->cd($config->current['BASE']);
  $ldap->cat($dn,array("objectClass","ou"));

  /* Check objectClasses and name to check if this is a release department */
  if($ldap->count()){
    $attrs = $ldap->fetch();
			
    $ou = "";
    if(isset($attrs['ou'][0])){
      $ou = $attrs['ou'][0];	
    }
	
    if((in_array("FAIbranch",$attrs['objectClass'])) || ($ou == "fai")){
      return($attrs['dn']);
    }
  }
  return(false);
}


/* This function returns the dn of the object release */
function get_release_dn($Current_DN)
{
  global $config;
  $ldap = $config->get_ldap_link();
  $ldap->cd($config->current['BASE']);

  /* Split dn into pices */ 
  $dns_to_check = ldap_explode_dn(str_ireplace(",".$config->current['BASE'],"",$Current_DN),0);
  if(!is_array($dns_to_check)){
    return;  
  }

  /* Use dn pieces, to create sub dns like 
      ou=test,ou=1,ou=0...
              ou=1,ou=0...
                   ou=0... 
    To check which dn is our release container.
  */
  unset($dns_to_check['count']); 
  while(count($dns_to_check)){

    /* Create dn */
    $new_dn = "";
    foreach($dns_to_check as $part){
      $new_dn .= $part.",";
    }
    $new_dn .= $config->current['BASE'];

    /* Check if this dn is a release dn */
    if(is_release_department($new_dn)){
      return($new_dn);
    }

    /* Remove first element of dn pieces */
    reset($dns_to_check);
    unset($dns_to_check[key($dns_to_check)]);
  }
  return("");
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
