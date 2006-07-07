<?php

define("DEBUG_FAI_FUNC",FALSE);

/* TEST PHASE .... */

/* Returns all object for the given release.
   This function resolves the releases  
   from base up to the given dn.
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
      $ldap->search($filter,array("dn","objectClass","FAIstate"));

      /* check the returned objects, and add/replace them in our return variable */
      while($attr = $ldap->fetch()){
        
        $buffer = array();
        $name = str_ireplace($release,"",$attr['dn']);

        if(isset($attr['FAIstate'][0])){
          if(preg_match("/removed$/",$attr['FAIstate'][0])){
            if(isset($res[$name])){
              unset($res[$name]);
            }
            continue;
          }
        }


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
  if(DEBUG_FAI_FUNC) { echo "Not found"; } 
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


/* Return all releases within the given base */
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


/* Add this object to list of objects, that must be checked for release saving */
function prepare_to_save_FAI_object($Current_DN,$objectAttrs,$removed = false)
{
  /* Get ldap object */  
  global $config;
  $addObj['Current_DN'] = $Current_DN;
  $addObj['objectAttrs']= $objectAttrs;
  $addObj['removed']    = $removed;
  $addObj['diff']       = TRUE;

  if(!$removed){
    $ldap = $config->get_ldap_link();
    $ldap->cd($config->current['BASE']);

    /* Get some basic informations */
    $parent_obj   = get_parent_release_object($Current_DN);
    if(!empty($parent_obj)){
      $ldap->cat($parent_obj,array("*"));
      $attrs = prepare_ldap_fetch_to_be_saved($ldap->fetch());

      if(!array_diff_FAI( $attrs,$objectAttrs)){
        $addObj['diff'] = FALSE;
      }
    } 
  }
   
  $_SESSION['FAI_objects_to_save'][$Current_DN] =  $addObj;
}


/* Detect differences in attribute arrays  */
function array_diff_FAI($ar1,$ar2)
{
  foreach($ar1 as $key1 => $val1){

    if((is_array($val1)) && (count($val1)==1)){
      $ar1[$key1] = $val1[0];
    }

    if((is_array($ar2[$key1])) && (count($ar2[$key1])==1)){
      $val1 = $val1[0];
      $ar2[$key1] = $ar2[$key1][0];
    }
  }
  if(count( array_diff($ar1,$ar2))){
    return(true);
  }else{
    return(false);
  }
}


/* check which objects must be saved, and save them */
function save_release_changes_now()
{
  /* Variable init*/
  $to_save = array();
  

  /* check which attributes must realy be saved */
  foreach($_SESSION['FAI_objects_to_save'] as $Current_DN => $object){
    if($object['diff']){
      $sub_name = $Current_DN;
      while(isset($_SESSION['FAI_objects_to_save'][$sub_name])){
       
        
        $to_save[strlen($sub_name)][$sub_name] = $_SESSION['FAI_objects_to_save'][$sub_name]; 
        unset($_SESSION['FAI_objects_to_save'][$sub_name]);
        $sub_name = preg_replace('/^[^,]+,/', '', $sub_name);
      }
    }
  }

  /* Sort list of objects that must be saved, and ensure that 
      container   objects are safed, before their childs are saved */
  ksort($to_save);
  
  $tmp = array();
  foreach($to_save as $SubObjects){
    foreach($SubObjects as $object){
      $tmp[] = $object;
    }
  }

  $to_save = $tmp;

  /* Save objects and manage the correct release behavior*/
  foreach($to_save as $save){

    $Current_DN = $save['Current_DN'];
    $removed    = $save['removed'];
    $objectAttrs= $save['objectAttrs'];

    /* Get ldap object */ 
    global $config;
    $ldap = $config->get_ldap_link();
    $ldap->cd($config->current['BASE']);

    /* Get some basic informations */
    $base_release       = get_release_dn($Current_DN);
    $sub_releases       = get_sub_releases_of_this_release($base_release,true);
    $parent_obj         = get_parent_release_object($Current_DN);
    $following_releases = get_sub_releases_of_this_release($base_release,true);
    
    /* Check if given dn exists or if is a new entry */
    $ldap->cat($Current_DN);
    if(!$ldap->count()){
      $is_new = true;
    }else{
      $is_new = false;
    }
    
    /* if parameter removed is true, we have to add FAIstate to the current attrs 
          FAIstate should end with ...|removed after this operation */  
    if($removed ){
      $ldap->cat($Current_DN);

      /* Get current object, because we must add the FAIstate ...|removed */
      if((!$ldap->count()) && !empty($parent_obj)){
        $ldap->cat($parent_obj);
      }

      /* Check if we have found a suiteable object */ 
      if(!$ldap->count()){
        echo "Error can't remove this object ".$Current_DN;
        return;
      }else{

        /* Set FAIstate to current objectAttrs */
        $objectAttrs = prepare_ldap_fetch_to_be_saved($ldap->fetch());
        if(isset($objectAttrs['FAIstate'][0])){
          if(!preg_match("/removed$/",$objectAttrs['FAIstate'][0])){
            $objectAttrs['FAIstate'][0] .= "|removed";
          }
        }else{
          $objectAttrs['FAIstate'][0] = "|removed";
        }
      }
    }
   
   
    /* Check if this a leaf release or not */ 
    if(count($following_releases) == 0 ){

      /* This is a leaf object. It isn't unherited by any other object */    
      if(DEBUG_FAI_FUNC) { 
        echo "<b>Saving directly, is a leaf object</b><br> ".$Current_DN;
        print_a($objectAttrs);
      }
      save_FAI_object($Current_DN,$objectAttrs);
    }else{

      /* This object is inherited by some sub releases */  

      /* Get all releases, that inherit this object */ 
      $r = get_following_releases_that_inherit_this_object($Current_DN);

      /* Get parent object */
      $ldap->cat($parent_obj);
      $parent_attrs = prepare_ldap_fetch_to_be_saved($ldap->fetch());
        
      /* New objects require special handling */
      if($is_new){

        /* check if there is already an entry named like this,
            in one of our parent releases */
        if(!empty($parent_obj)){
          if(DEBUG_FAI_FUNC) { 
            echo "There is already an entry named like this.</b><br>";

            echo "<b>Saving main object</b>".$Current_DN;
            print_a($objectAttrs);
          }    
          save_FAI_object($Current_DN,$objectAttrs);

          foreach($r as $key){
            if(DEBUG_FAI_FUNC) { 
              echo "<b>Saving parent to following release</b> ".$key;
              print_a($parent_attrs);
            }
            save_FAI_object($key,$parent_attrs);
          }
        }else{

          if(DEBUG_FAI_FUNC) { 
            echo "<b>Saving main object</b>".$Current_DN;
            print_a($objectAttrs);
          }
          save_FAI_object($Current_DN,$objectAttrs);

          if(isset($objectAttrs['FAIstate'])){
            $objectAttrs['FAIstate'] .= "|removed"; 
          }else{
            $objectAttrs['FAIstate'] = "|removed";
          }

          foreach($r as $key ){
            if(DEBUG_FAI_FUNC) { 
              echo "<b>Create an empty placeholder in follwing release</b> ".$key; 
              print_a($objectAttrs);
            }
            save_FAI_object($key,$objectAttrs);
          }
        }
      }else{

        /* check if we must patch the follwing release */
        if(!empty($r)){
          foreach($r as $key ){
            if(DEBUG_FAI_FUNC) { 
              echo "<b>Copy current objects original attributes to next release</b> ".$key;
              print_a($parent_attrs);
            }
            save_FAI_object($key,$parent_attrs);
          }
        }

        if(DEBUG_FAI_FUNC) { 
          echo "<b>Saving current object</b>".$parent_obj;
          print_a($objectAttrs);
        }
        save_FAI_object($parent_obj,$objectAttrs);

        if(($parent_obj != $Current_DN)){
          print_red(sprintf(_("Error, following objects should be equal '%s' and '%s'"),$parent_obj,$Current_DN));
        }
      }
    }
  } 
  $_SESSION['FAI_objects_to_save'] = array();
}


/* this function will remove all unused (deleted) objects,
    that have no parent object */
function clean_up_releases($Current_DN)
{
  global $config;
  $ldap = $config->get_ldap_link();
  $ldap->cd($config->current['BASE']);

  /* Collect some basic informations and initialize some variables */ 
  $base_release       = get_release_dn($Current_DN);
  $previous_releases  = array_reverse(get_previous_releases_of_this_release($base_release,true));
  $Kill = array();
  $Skip = array();

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
      $ldap->search("(objectClass=FAIclass)",array("dn","objectClass","FAIstate"));

      /* check the returned objects, and add/replace them in our return variable */
      while($attr = $ldap->fetch()){
        
        $buffer = array();
        $name = str_ireplace($release,"",$attr['dn']);

        if(isset($attr['FAIstate'][0])&&(preg_match("/removed$/",$attr['FAIstate'][0]))){

          /* Check if this object is required somehow */    
          if(!isset($Skip[$name])){
            $Kill[$attr['dn']] = $attr['dn'];
          }
        }else{
      
          /* This object is required (not removed), so do not 
              delete any following sub releases of this object */
          $Skip[$name] = $attr['dn'];
        }
      }
    }
  }
  return($Kill);
}


function prepare_ldap_fetch_to_be_saved($attrs)
{
  foreach($attrs as $key => $value){
    if(is_numeric($key) || ($key == "count") || ($key == "dn")){
      unset($attrs[$key]);
    }
    if(is_array($value) && isset($value['count'])){
      unset($attrs[$key]['count']);
    }
  }
  return($attrs);
}


function save_FAI_object($dn,$attrs)
{
  global $config;
  $ldap = $config->get_ldap_link();
  $ldap->cd($config->current['BASE']);
  echo $dn;
  print_a($attrs);
  $ldap->create_missing_trees(preg_replace('/^[^,]+,/', '', $dn));
  $ldap->cd($dn);
  
  $ldap->cat($dn,array("dn"));
  if($ldap->count()){
    $ldap->modify($attrs);
  }else{
    $ldap->add($attrs);
  }
  show_ldap_error($ldap->get_error(),sprintf(_("Release management failed, can't save '%s'"),$dn));
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
  $sub_releases = get_sub_releases_of_this_release($base_release);

  /* Get dn suffix. Example  "FAIvairableEntry=keksdose,FAIvariable=Keksregal," */
  $dn_suffix = str_ireplace($base_release,"",$dn);

  /* Check if given object also exists whitin one of these releases */
  foreach($sub_releases as $p_release => $name){

    $check_dn = $dn_suffix.$p_release;
  
    $ldap->cat($check_dn,array("dn","objectClass"));
    
    if($ldap->count()){
      //return($ret);
    }else{
      $ret[$check_dn]=$check_dn;
    }
  }
  return($ret);
}


/* Get previous version of the object dn */
function get_parent_release_object($dn,$include_myself=true)
{
  global $config;
  $ldap = $config->get_ldap_link();
  $ldap->cd($config->current['BASE']);
  $previous_releases= array();

  /* Get base release */
  $base_release = get_release_dn($dn);
  if($include_myself){
    $previous_releases[] = $base_release;  
  }

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
