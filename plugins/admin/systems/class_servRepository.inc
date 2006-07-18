<?php

class servrepository extends plugin
{
  /* CLI vars */
  var $cli_summary          = "Manage server basic objects";
  var $cli_description      = "Some longer text\nfor help";
  var $cli_parameters       = array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  //  var $ignore_account         = TRUE;
  var $attributes             = array("FAIrepository");
  var $objectclasses          = array("FAIrepositoryServer");
  
  /* Search filter */
  var $regex                  = "*";

  /* Configurationdialog for repositories */
  var $dialog                 = NULL;

  /* Repositories */
  var $repositories          = array();
  var $FAIrepository         = array();


  var $conflicts             = array("FAIrepositoryServer");
  var $DisplayName           = "";
  var $StatusFlag            = "";


  function servrepository ($config, $dn= NULL)
  {
    plugin::plugin ($config, $dn);

    $this->DisplayName = _("Repository service");

    $this->repositories = array();
    if(isset($this->attrs['FAIrepository'])){
      for($i = 0; $i < $this->attrs['FAIrepository']['count']; $i++){
        $tmp = split("\|",$this->attrs['FAIrepository'][$i]);
        $tmp2 = array();  
        $tmp3 = array();   

        if(isset($tmp[1])){
          $tmp2['ParentServer'] = $tmp[1];
          if(empty($tmp[1])){
            $tmp2['ParentServer'] = "none";
          }
        }else{
          $tmp2['ParentServer'] = "none";
        }

        if(isset($tmp[0])){
          $tmp2['Url']          = $tmp[0];
        }else{
          $tmp2['Url']          = "";
        }
  
        if(isset($tmp[2])){
          $tmp2['Release']      = $tmp[2];
        }else{
          $tmp2['Release']      = "";
        }

        if(isset($tmp[3])){
          $tmp3 = split(",",$tmp[3]);
          foreach($tmp3 as $sec){
            $tmp2['Sections'][$sec]=$sec;
          }    
        }else{
          $tmp['Section']=array();
        }

        $this->repositories[$tmp[2]]=$tmp2;      
      }
    }
  }

  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    /* Fill templating stuff */
    $smarty= get_smarty();
    $display= "";

    /* Smarty vars*/
    $smarty->assign("infoimage",    get_template_path('images/info.png'));
    $smarty->assign("search_image", get_template_path('images/search.png'));
    $smarty->assign("launchimage",  get_template_path('images/small_filter.png'));
    $smarty->assign("tree_image",   get_template_path('images/tree.png'));
    $smarty->assign("alphabet",     generate_alphabet());
    $smarty->assign("apply",        apply_filter());
    $smarty->assign("regex",        $this->regex);

    /* Show tab dialog headers */
    /*
       ADD / EDIT Repository
       Dialog Handling
     */
    $once = false;
    if(isset($_POST['servRepository'])){
      foreach($_POST as $name => $value){

        if(preg_match("/AddRepository/",$name)){
          $once = true;
          $this->dialog = new servRepositorySetup($this->config,$this->dn);
          $this->dialog->acl = $this->acl;
        }

        if((preg_match("/^delete_/",$name))&&(!$once)){
          $once = true;
          $value = preg_replace("/delete_/","",$name);
          $value = base64_decode(preg_replace("/_.*$/","",$value));

          $url = $this->repositories[$value]['Url'];
          $release = $this->repositories[$value]['Release'];

          $ldap = $this->config->get_ldap_link();
          $ldap->cd ($this->config->current['BASE']);

          $ldap->search("(&(objectClass=gotoWorkstation)(objectClass=FAIobject)(FAIdebianMirror=".$url."))",array("cn","FAIclass"));

          $found = false;
          $found_in = " ";
          while($attrs = $ldap->fetch()){
            foreach($attrs['FAIclass'] as $class){
              if(preg_match("/".str_replace("/","\/",$release)."$/i",$class)){
                $found = true;  
                $found_in .= $attrs['cn'][0]." ";
              }
            }
          }

          if($found){
            print_red(sprintf(_("You can't delete this release, it is still used by these workstations [%s]. Please solve this dependencies first, to keep data base consistency."),$found_in));
          }else{
            if(isset($this->repositories[$value])){
              unset($this->repositories[$value]);
            }
          }
        }

        if((preg_match("/^edit_/",$name))&&(!$once)){
          $value = preg_replace("/edit_/","",$name);
          $value = base64_decode(preg_replace("/_.$/","",$value));

          if(isset($this->repositories[$value])){

            $ldap = $this->config->get_ldap_link();
            $ldap->cd ($this->config->current['BASE']);

            $url = $this->repositories[$value]['Url'];
            $release = $this->repositories[$value]['Release'];

            $ldap->search("(&(objectClass=gotoWorkstation)(objectClass=FAIobject)(FAIdebianMirror=".$url."))",array("cn","FAIclass"));

            $found = false;
            $found_in = " ";
            while($attrs = $ldap->fetch()){
              foreach($attrs['FAIclass'] as $class){
                if(preg_match("/".str_replace("/","\/",$release)."$/i",$class)){
                  $found = true;
                  $found_in .= $attrs['cn'][0]." ";
                }
              }
            }

            if($found){
              print_red(sprintf(_("Be careful editing this release, it is still used by these workstations [%s]."),$found_in));
            } 

            if(isset($this->repositories[$value])){
              $once = true;
              $obj = $this->repositories[$value];

              /* to be able to detect if this was renamed */
              $obj['initialy_was'] = $obj['Release'];
              $this->dialog = new servRepositorySetup($this->config,$this->dn,$obj);
              $this->dialog->acl = $this->acl;
            }
          }
        }
      }
    }
    if((isset($_GET['act']))&&($_GET['act']=="open")&&(isset($_GET['id']))){
      $obj = $this->repositories[base64_decode($_GET['id'])];
      $obj['initialy_was'] = $obj['Release'];
      $this->dialog = new servRepositorySetup($this->config,$this->dn,$obj);
      $this->dialog->acl = $this->acl;
    }

    /*
    if(isset($_POST['AddRepository'])){
      $this->dialog = new servRepositorySetup($this->config,$this->dn);
      $this->dialog->acl = $this->acl;
    }
    */

    if(isset($_POST['repository_setup_save'])){
      $this->dialog->save_object();
      if(($this->dialog->is_new_name())&&(isset($this->repositories[$this->dialog->GetName()]))){
        print_red(_("This name is already in use."));
      }else

      if(count($this->dialog->check())!=0){
        foreach($this->dialog->check() as $msg){
          print_red($msg);
        }
      }else{
        $obj = $this->dialog->save();
        if($this->dialog->is_new_name()){
          $oldname = $this->dialog->initialy_was;
          $this->repositories[$obj['Release']]=$obj;        
          unset($this->repositories[$oldname]);
        }else{ 
          $this->repositories[$obj['Release']]=$obj;        
        }
        $this->dialog = NULL;
        $this->is_dialog= false;
      }
    }

    if(isset($_POST['repository_setup_cancel'])){
      $this->dialog=NULL;
      $this->is_dialog = false;
    }
   
    if($this->dialog != NULL){
      $this->dialog->save_object();
      $this->is_dialog = true;
      return($this->dialog->execute());
    }

    /*
      Repository setup dialog handling /END
    */

    $divlist = new divList("repositories");
    $divlist->SetEntriesPerPage(0);
    $divlist->setHeight(400);

    $divlist->setHeader(array(array("string"=>_("Release"),"attach"=>"style='width:80px;'"),
                              array("string"=>_("Sections")),
                              array("string"=>_("Options"),"attach"=>"style='border-right:0px;width:55px;'")
                       )     );

    $link   = "<a href='?plug=".$_GET['plug']."&amp;act=open&amp;id=%s'>%s</a>";
    $edit   = "<input type='image' value='%s' name='edit_%s'   src='images/edit.png'>&nbsp;";
    $delete = "<input type='image' value='%s' name='delete_%s' src='images/edittrash.png'>";
  
    foreach($this->repositories as $name => $reps){

      $str = " ";

      if(preg_match("/".str_replace("*",".*",$this->regex)."/",$reps['Release'])){
    

      foreach($reps['Sections'] as $sec){
        $str.=$sec." ";  
      }    
  
      $divlist->AddEntry(array(
                              array("string"=>sprintf($link,base64_encode($name),$name),"attach"=>"style='width:80px;'"),
                              array("string"=>sprintf($link,base64_encode($name),_("Sections")." :".$str)),
                              array("string"=>preg_replace("/%s/",base64_encode($name),$edit.$delete),"attach"=>"style='border-right:0px;width:55px;text-align:right;'")
                              ));
      }
    }
 
    $smarty -> assign("Repositories",$divlist->DrawList());

    $display.= $smarty->fetch(get_template_path('servRepository.tpl', TRUE));
    return($display);
  }

  function remove_from_parent()
  {
    plugin::remove_from_parent();    

     /* Remove status flag, it is not a memeber of
        this->attributes, so ensure that it is deleted too */
    if(!empty($this->StatusFlag)){
      $this->attrs[$this->StatusFlag] = array();
    }

    $ldap= $this->config->get_ldap_link();
    $ldap->cd ($this->config->current['BASE']);
    
    $ldap->cat($this->dn, array('dn'));
    
    if($ldap->count()){
      $ldap->cd($this->dn);
      $this->cleanup();
      $ldap->modify ($this->attrs); 

      $this->handle_post_events("modify");
    }
  }


  /* Save data to object */
  function save_object()
  {
    plugin::save_object();
    if(isset($_POST['regex'])){
      $this->regex = $_POST['regex'];
    }
  }


  /* Check supplied data */
  function check()
  {
    /* Call common method to give check the hook */
    $message= plugin::check();
        
    return ($message);
  }


  /* Save to LDAP */
  function save()
  {
    plugin::save();

    $arr = array();
    foreach($this->repositories as $servername => $conf){
      $str = "";
      foreach($conf['Sections'] as $sec){
        $str.=$sec.",";
      }
      $str=preg_replace("/,$/","",$str);
        
      if($conf['ParentServer']=="none"){
        $conf['ParentServer'] ="";
      }    
    
      $arr[]=$conf['Url']."|".$conf['ParentServer']."|".$conf['Release']."|".$str;
    }
    $this->attrs['FAIrepository'] = $arr;

    $ldap= $this->config->get_ldap_link();
    $ldap->cd ($this->config->current['BASE']);
    
    $ldap->cat($this->dn, array('dn'));
    
    if($ldap->count()){
      $ldap->cd($this->dn);
      $this->cleanup();
      $ldap->modify ($this->attrs);       

      $this->handle_post_events("modify");
    }else{
      $ldap->cd ($this->config->current['BASE']);
      $ldap->create_missing_trees($this->dn);
      $ldap->cd($this->dn);
      $ldap->add($this->attrs);
      $this->handle_post_events("add");
    }
  }

  /* Get updates for status flag */
  function updateStatusState()
  {
    if(empty($this->StatusFlag)) return;

    $attrs = array();
    $flag = $this->StatusFlag;
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->cn);
    $ldap->cat($this->dn,array($flag));
    if($ldap->count()){
      $attrs = $ldap->fetch();
    }
    if(isset($attrs[$flag][0])){
      $this->$flag = $attrs[$flag][0];
    }
  }


  function getListEntry()
  {
    $this->updateStatusState();
    $flag = $this->StatusFlag;
    if(empty($flag)){
      $fields['Status']      = "";
    }else{
      $fields['Status']      = $this->$flag;
    }
    $fields['Message']    = _("Repository service");
    $fields['AllowStart'] = true;
    $fields['AllowStop']  = true;
    $fields['AllowRestart'] = true;
    $fields['AllowRemove']= true;
    $fields['AllowEdit']  = true;
    return($fields);
  }


  /* Directly save new status flag */
  function setStatus($value)
  {
    if($value == "none") return;
    if(!$this->initially_was_account) return;
    if(empty($this->StatusFlag)) return;
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->dn);
    $ldap->cat($this->dn,array("objectClass"));
    if($ldap->count()){

      $tmp = $ldap->fetch();
      for($i = 0; $i < $tmp['objectClass']['count']; $i ++){
        $attrs['objectClass'][] = $tmp['objectClass'][$i];
      }
      $flag = $this->StatusFlag;
      $attrs[$flag] = $value;
      $this->$flag = $value;
      $ldap->modify($attrs);
      show_ldap_error($ldap->get_error(), sprintf(_("Set status flag for server services/repository with dn '%s' failed."),$this->dn));
      $this->action_hook();
    }
  }


  /* Return plugin informations for acl handling */
  function plInfo()
  {
    return (array(
          "plShortName"   => _("Repository"),
          "plDescription" => _("Repository service"),
          "plSelfModify"  => FALSE,
          "plDepends"     => array(),
          "plPriority"    => 0,
          "plSection"     => array("administration"),
          "plCategory"    => array("server"),

          "plProvidedAcls"=> array(
              "cn"            => _("Name"),
              "Release"       => _("Releases"),
              "ParentServer"  => _("Parent server"),
              "Url"           => _("Url"))
            ));
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
