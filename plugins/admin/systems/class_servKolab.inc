<?php

class servkolab extends plugin {
  /* CLI vars */
  var $cli_summary = "Manage server basic objects";
  var $cli_description = "Some longer text\nfor help";
  var $cli_parameters = array("eins"=>"Eins ist toll", "zwei"=>"Zwei ist noch besser");

  var $postfix_mydomain                 = "";
  var $postfix_mydestination            = "";
  var $postfix_mynetworks               = "127.0.0.1/8";
  var $postfix_enable_virus_scan        = "TRUE";
  var $postfix_relayhost                = "";
  var $postfix_mxrelayenabled           =  true;
  var $postfix_allow_unauthenticated     = "FALSE";
  var $cyrus_quotawarn                  = "80";
  var $kolabFreeBusyFuture              = "1";
  var $k                                = "kolab";
  var $cyrus_admins                     = "TRUE";
  var $cyrus_imap                       = "TRUE";
  var $cyrus_pop3                       = "TRUE";
  var $cyrus_imaps                      = "TRUE";
  var $cyrus_pop3s                      = "TRUE";
  var $cyrus_sieve                      = "TRUE";
  var $apache_allow_unauthenticated_fb  = "TRUE";
  var $proftpd_ftp                      = "TRUE";
  var $apache_http                      = "TRUE";
  var $kolabHost                        = array();
  var $orig_cn                          = "";

  var $attributes =  array("postfix_mydomain", "postfix_mydestination", "proftpd_ftp", "k",
      "postfix_mynetworks", "postfix_enable_virus_scan", "postfix_relayhost", "apache_http",
      "postfix_allow_unauthenticated", "cyrus_admins", "cyrus_imap","kolabFreeBusyFuture",
      "cyrus_pop3", "cyrus_imaps", "cyrus_pop3s", "cyrus_sieve", "apache_allow_unauthenticated_fb",
      "cyrus_quotawarn");
  var $objectclasses = array("top", "kolab");

  /* Serverservice vars */
  var $conflicts    = array("goImapServer","goMailServer");
  var $DisplayName  = "Kolab mail service";
  var $StatusFlag   = "";

  function servkolab($config, $dn = NULL) 
  {
    /* Setting the hostname and tell this Plugin that we are the kolab extension*/
    $this->hostname = preg_replace('/^cn=([^,]+),.*$/', '\1', $dn);
    $this->dn       = "k=kolab,".$config->current['BASE'];

    /* Load variables, if given*/
    plugin::plugin($config, $this->dn);

    /* Copy needed attributes */
    foreach($this->attributes as $val) {
      $name = preg_replace('/_/', '-', $val);
      if (isset($this->attrs["$name"][0])) {
        $this->$val = $this->attrs["$name"][0];
      }
    }

    /* Toggle relayhost */
    $this->postfix_mxrelayenabled = preg_match('/^\[/', $this->postfix_relayhost);
    $this->postfix_relayhost      = preg_replace("/[\[\]]/","",$this->postfix_relayhost);

    /* Is this Server a member of the Kolab extension or not ?*/ 
    if(isset($this->attrs['kolabHost'])) {
      $this->kolabHost= $this->attrs['kolabHost'];
      unset($this->kolabHost['count']);
    }
    $this->is_account            = false;
    $this->initially_was_account = false;
    foreach($this->kolabHost as $host){
      if($this->hostname == $host){
        $this->is_account            = true;
        $this->initially_was_account = true;
      }
    } 

  }


  function execute() 
  {
    /* Call parent execute */
    plugin::execute();

    /***************  
      Variable initialisation
     ***************/  
    
    $smarty     = get_smarty();
    $display    = "";
    $ldap       = $this->config->get_ldap_link();


    /* Assemble free/busy string */
    $edit       = sprintf('<input name="kolabFreeBusyFuture" value="%s" %s type="text" maxlength="3" size="4">',
                    $this->kolabFreeBusyFuture, 
                    chkacl($this->acl, 'kolabFreeBusyFuture'));
    $fbfuture   = sprintf(_("Include data from %s days in the past when creating free/busy lists"), $edit);

    /* Assemble quota string */
    $edit       = sprintf('<input name="cyrus_quotawarn" value="%s" type="text" maxlength="3" size="4" %s>',
                    $this->cyrus_quotawarn, 
                    chkacl($this->acl, 'cyrus_quotawarn'));
    $quotastr   = sprintf(_("Warn users when using more than %s%% of their mail quota"), $edit);

    /***************
      Assign informations to smarty 
     ***************/

    /* Set relayhost and if we have MX lookup enabled*/
    if($this->postfix_mxrelayenabled) {
      $smarty->assign("RelayMxSupportCheck"," checked ");
    } else {
      $smarty->assign("RelayMxSupportCheck","");
    }

    /* Initialize all attributes, that were submitted */
    foreach($this->attributes as $val) 
    {
      $smarty->assign($val."ACL",chkacl($this->acl,str_replace("_","-",$val)));

      /* Tell smarty which variables we are useing */
      $smarty->assign($val, $this->$val);
      if (($this->$val != "FALSE") && !empty($this->$val)){
        $smarty->assign($val."Check", "checked");
      } else {
        $smarty->assign($val."Check", "");
      }
    }

    $smarty->assign("fbfuture", $fbfuture);
    $smarty->assign("quotastr", $quotastr);

    /* Load Template */
    $display.=$smarty->fetch(get_template_path('servkolab.tpl', TRUE));
    return ($display);
  }

  function remove_from_parent() 
  {

     /* Remove status flag, it is not a memeber of
        this->attributes, so ensure that it is deleted too */
    if(!empty($this->StatusFlag)){
      $this->attrs[$this->StatusFlag] = array();
    }


    /* Only walk through following code, if this host 
       was a member of the kolab hosts, else skip this */
    if(!$this->initially_was_account){
      return;
    }

    /* !!! Don't use "cn" in this function 
       hostname -> the initial name of the host
       cn       -> is the new name of the host, in case that it was renamed.
     */

    $ldap     = $this->config->get_ldap_link();
    $this->dn = "k=kolab,".$this->config->current['BASE'];

    /* We can't simply remove the whole entry, it is possible that there are 
       some other hosts assigned to this object. 
       So, first of all check if we are the last host entry within the host 
       : Remove k=kolab entry 
       if we aren't alone, only remove host name from hosts entry and save */
    /* Are we alone? Remove complete entry... */

    /* Check if we are definitly in kolabHosts */
    if(!in_array_ics($this->hostname,$this->kolabHost)) {
      return;
    }

    /* Integration check, not translatet because they can't pop up at all, only for debug */ 
    if(count($this->kolabHost) == 0){
      print_red("Server - Kolab tab : This is not possible, we can't remove an account which doesn't exists.");
      return;
    }
    if(!isset($this->hostname) || (empty($this->hostname))){
      print_red("The required attribute hostname seams to empty.");
    }

    /* Our hostname is in kolabHosts and there is a only one entry 
       = we are the last host entry, delete k=kolab entry  */
    if (count($this->kolabHost) == 1){

      /* Remove complete entry */
      $ldap->rmdir ($this->dn);
      show_ldap_error($ldap->get_error(), sprintf(_("Removing host entry from system server/kolab with dn '%s' failed."),$this->dn));
      gosa_log("k=kolab entry removed, ".$this->hostname." was the last kolabHost entry.");

    } else {

      /* Only modify kolabHost */
      $hosts= array();
      foreach ($this->kolabHost as $host){
        if($host != $this->hostname){
          $hosts[]= $host;
        }
      }
      $attrs= array('kolabHost' => $hosts);
      $ldap->cd($this->dn);
      $this->cleanup();
      $ldap->modify ($attrs); 
      show_ldap_error($ldap->get_error(), sprintf(_("Removing host entry from system server/kolab with dn '%s' failed."),$this->dn));
      gosa_log("Removing ".$this->hostname." from list of kolabHosts");
    }

    /* Optionally execute a command after we're done */
    $this->handle_post_events("remove");
  }


  function save_object()    
  {
    if (isset($_POST['kolabtab'])){
      plugin::save_object();

      /* Save checkboxes */
      foreach (array( "postfix_enable_virus_scan", "postfix_allow_unauthenticated",
            "cyrus_admins", "cyrus_imap", "cyrus_pop3", "cyrus_imaps",
            "cyrus_pop3s", "cyrus_sieve", "apache_allow_unauthenticated_fb",
            "proftpd_ftp", "apache_http") as $cb){
        if (isset($_POST[$cb])){
          $this->$cb= "TRUE";
        } else {
          $this->$cb= "FALSE";
        }
      }

      /* Toggle relay check */
      $this->postfix_mxrelayenabled= isset($_POST['RelayMxSupport']);
    }

  }


  function check() 
  {
    /* Call common method to give check the hook */
    $message= plugin::check();

    if(($this->kolabFreeBusyFuture==""))    {
      $message[] = _("Future days in Free/Busy settings must be set.");
    }elseif(!is_uid($this->kolabFreeBusyFuture) || $this->kolabFreeBusyFuture < 0){
      $message[] = _("Future days in Free/Busy settings must be a positive value.");
    }

    if(!is_int((int)($this->cyrus_quotawarn))) {
      $message[] = _("The given Quota settings value must be a number.");
    }elseif(!(($this->cyrus_quotawarn<=100)&&($this->cyrus_quotawarn>=0))){
      $message[] = _("Please choose a value between 1 and 100 for Quota settings.");
    }elseif(strcasecmp($this->cyrus_quotawarn,(int)$this->cyrus_quotawarn)){
      $message[] = _("Future days must be a value.");
    }

    if(empty($this->postfix_mynetworks)) { 
      $message[] = _("No SMTP privileged networks set.");
    }

    /*if(empty($this->postfix_relayhost)) {
      $message[] = _("No SMTP smarthost/relayhost set.");
      }
     */

    return ($message);
  }


  /* Save to LDAP */
  function save() 
  {
    /* Set ldap connection */
    $ldap       = $this->config->get_ldap_link();

    /* Open current dn*/
    $this->dn   = "k=kolab,".$this->config->current['BASE'];

    /* Adapt relayhost */
    $this->postfix_relayhost= preg_replace('/[\[\]]/', '', $this->postfix_relayhost);
    if (!$this->postfix_mxrelayenabled && $this->postfix_relayhost != ""){
      $this->postfix_relayhost= "[".$this->postfix_relayhost."]";
    }

    /* Check if this server was renamed, in this case we have to remove old cn first*/
    if($this->hostname != $this->cn){
      $tmp = array();
      if(in_array_ics($this->hostname,$this->kolabHost)){
        foreach($this->kolabHost as $host){
          if($host != $this->hostname){
            $tmp[] = $host;
          }
        }
        $this->kolabHost = $tmp;
      }
    }

    /* Add ourselves to the list of kolabHost's if needed */
    if (!in_array_ics($this->cn,$this->kolabHost)){
      $this->kolabHost[]= $this->cn;
    }

    /* Call parents save to prepare $this->attrs */
    plugin::save();

    /* Save or modify? */
    $ldap->cat($this->dn, array('dn'));
    if (!$ldap->fetch()){
      $mode= "add"; 
    } else {
      $mode= "modify";
    }

    /* Do attribute conversion */
    foreach ($this->attrs as $key => $value){
      if (preg_match('/_/', $key)){
        $old_key= $key;
        $key= preg_replace('/_/', '-', $key);
        $this->attrs[$key]= $value;
        unset($this->attrs[$old_key]);
      }
    }
    /* Add kolab hosts */
    $this->attrs['kolabHost']= $this->kolabHost;

    /* Perform LDAP action */
    $ldap->cd($this->dn);
    $this->cleanup();;
    $ldap->$mode($this->attrs);
    show_ldap_error($ldap->get_error(), sprintf(_("Saving system server/kolab with dn '%s' failed."),$this->dn));

    /* Optionally execute a command after we're done */
    if ($this->initially_was_account == $this->is_account) {
      if ($this->is_modified) {
        $this->handle_post_events("mofify");
      }
    } else {
      $this->handle_post_events("add");
    }
  }


  function getListEntry()
  {
    $this->updateStatusState();
    $flag = $this->StatusFlag;
    if(empty($flag)){
      $fields['Status']       = "";
    }else{
      $fields['Status']       = $this->$flag;
    }
    $fields['Message']      = _("Kolab mail service");
    $fields['AllowStart']   = true;
    $fields['AllowStop']    = true;
    $fields['AllowRestart'] = true;
    $fields['AllowRemove']  = true;
    $fields['AllowEdit']    = true;
    return($fields);
  }

    /* Directly save new status flag */
  function setStatus($value)
  { 
    return;
  
    if($value == "none") return;
    if(!$this->initially_was_account) return;
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
      show_ldap_error($ldap->get_error(), sprintf(_("Set status flag for system server/kolab with dn '%s' failed."),$this->dn)); 
      $this->action_hook();
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
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
