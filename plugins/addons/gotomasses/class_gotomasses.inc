<?php

class gotomasses extends plugin
{
  /* Definitions */
  var $plHeadline     = "Mass machine";
  var $plDescription  = "This does something";

  /* attribute list for save action */
  var $attributes= array();
  var $objectclasses= array();

  /* Source file that contains the csv data */
  var $file_to_read = "Undefined"; #Set in constructor 

  /* Parsed csv content */
  var $contents         = array();

  /* Used to detect changes made on the csv content.
   *  Buttons will be disabled and js warnings will be 
   *  shown if the content wasn't saved or discarded
   */
  var $contents_backup  = array();


  function gotomasses($config, $dn= NULL)
  {
    /* Define source file */
    $this->file_to_read = CONFIG_DIR."/gotomasses_machines";
  
    /* Include config object */
    $this->config= $config;
    $this->load_csv_data();
  }


  function get_object_groups()
  {
    $ret = array();
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    $ldap->search("(&(objectClass=gosaGroupOfNames)(cn=*))",array("cn"));
    while($attrs = $ldap->fetch()){
      $ret [$attrs['cn'][0]] = $attrs['cn'][0];
    }
    return($ret); 
  }


  function execute()
  {
    if(isset($_POST['export_gotomass_csv'])){
        $data = "";
        foreach($this->contents as $val){
          $data .= $val['MAC'].", ".$val['OG']."\n";
        }
        header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
        header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
        header("Cache-Control: no-cache");
        header("Pragma: no-cache");
        header("Cache-Control: post-check=0, pre-check=0");
        header("Content-type: text/plain");
        if (preg_match('/MSIE 5.5/', $_SERVER['HTTP_USER_AGENT']) ||
            preg_match('/MSIE 6.0/', $_SERVER['HTTP_USER_AGENT'])){
          header('Content-Disposition: filename="gotomass.csv"');
        } else {
          header('Content-Disposition: attachment; filename="gotomass.csv";');
        }
        echo $data;
        exit();
    }
   
    /* Import given file */ 
    if(isset($_POST['import_gotomass_csv']) && isset($_FILES['mass_file'])){
      $str = @file_get_contents($_FILES['mass_file']['tmp_name']);
      if(empty($str)){
        print_red(_("The uploaded file seams to be empty, import aborted."));
      }else{
        $this->load_csv_data($str);
      }
    }

    /* Add a new empty entry to the list */ 
    if(isset($_POST['add_new_entry'])){
      $this->contents[] = array("MAC" => "", "OG" => "");
    }

    /* Call parent execute */
    plugin::execute();
    $smarty= get_smarty();
    $smarty->assign("contents_modified",$this->contents_modified());
    $smarty->assign("ogs", $this->get_object_groups());
    $smarty->assign("contents", $this->contents);
    $smarty->assign("launchimage","images/launch.png");
    return ($smarty->fetch (get_template_path('contents.tpl', TRUE)));
  }

  
  /* Check if something is modified */
  function contents_modified($display = FALSE)
  {
    $a = $this->contents;
    $b = $this->contents_backup;
    if(count($a) != count($b)){
      if($display){
        print_a(array_diff_assoc($a,$b));
      }
      return(TRUE);
    }else{
      foreach($a as $a_key => $a_val){
        if(count(array_diff($a_val, $b[$a_key]))){

          if($display){
            print_a(array_diff($a_val, $b[$a_key]));
          }
          return(TRUE);
        }
      }
    }
    return(FALSE); 
  }


  function load_csv_data($data = NULL)
  {
    if($data == NULL){
      if(!file_exists($this->file_to_read) || !is_readable($this->file_to_read)){
        print_red(sprintf(_("Can't locate or read csv storage file '%s'."),$this->file_to_read));
        return(FALSE);
      }

      $fp = @fopen($this->file_to_read,"r");
      if(!$fp){
        print_red(sprintf(_("Can't read csv storage file '%s'."),$this->file_to_read));
        return(FALSE);
      }

      $this->contents =array(); 

      while(!feof($fp)){
        $str = trim(fgets($fp,512));

        /* Get mac address */
        $og = trim(preg_replace("/^[^,;]*(,|;)/","",$str));
        $mac = preg_replace("/(,|;).*$/","",$str);

        if(!empty($og) || !empty($mac)){
          $this->contents[] = array("MAC" => $mac , "OG" => $og);
        }
      }
      fclose($fp);
    }else{
      $this->contents =array(); 
      $rows = split("\n",$data);
      foreach($rows as $str){
        
        /* Get mac address */
        $og = trim(preg_replace("/^[^,;]*(,|;)/","",$str));
        $mac = preg_replace("/(,|;).*$/","",$str);

        if(!empty($og) || !empty($mac)){
          $this->contents[] = array("MAC" => $mac , "OG" => $og);
        }
      }
    }
    $this->contents_backup = $this->contents;
  }


  function save_csv_data()
  {
    if(!file_exists($this->file_to_read) || !is_writeable($this->file_to_read)){
      print_red(sprintf(_("Can't locate or write csv storage file '%s'."),$this->file_to_read));
    }else{
      $fp = @fopen($this->file_to_read,"w");
      if(!$fp){
        print_red(sprintf(_("Can't write csv storage file '%s'."),$this->file_to_read));
      }else{  
        $data = "";
        foreach($this->contents as $val){
          $data .= $val['MAC'].", ".$val['OG']."\n";
        }
        fwrite($fp,$data,strlen($data));
        fclose($fp);
      }
    }
  }


  function save_object()
  {
    if(isset($_POST['gotomasses'])){

      /* Check for input changes */
      $ogs = $this->get_object_groups();
      foreach($this->contents as $id => $data){
        if(isset($_POST['mac_'.$id])){
          $this->contents[$id]['MAC'] = $_POST['mac_'.$id];
        }
        if(isset($_POST['og_'.$id]) && in_array_ics($_POST['og_'.$id],$ogs)){
          $this->contents[$id]['OG'] = $_POST['og_'.$id];
        }
      }

      /* check for remove requests */
      $once = TRUE;
      foreach($_POST as $name => $value){
        if(preg_match("/^remove_[0-9]*_(x|y)$/",$name) && $once){
          $once = FALSE;
          $id = preg_replace("/^remove_/","",$name);
          $id = preg_replace("/_(x|y)$/","",$id);

          if(isset($this->contents[$id])){
            unset($this->contents[$id]);
          }
        }
      }

      /* Write back all changes */
      if(isset($_POST['save_gotomass_changes'])){
        $this->save_csv_data();

        /* Call load again, so we will see if everything is fine. 
         * And load_csv_data causes the contents_backup to be updated 
         */
        $this->load_csv_data();
      }

      /* Reload data from csv file ? */
      if(isset($_POST['reload_gotomass_data'])){
        $this->load_csv_data();
      }
    }
  }


  function plInfo()
  {
    return (array(
        "plShortName"   => _("Mass machine deployment"),
        "plDescription" => _("Mass machine deployment addon"),
        "plSelfModify"  => FALSE,
        "plDepends"     => array(),
        "plPriority"    => 0,
        "plSection"     => array("addon"),
        "plCategory"    => array("gotomasses" => array("objectClass" => "none", "description" => _("Mass machine deployment"))),
        "plProvidedAcls" => array()
        ));
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
