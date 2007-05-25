<?php

require_once("tabs_log.inc");
class gosa_logview extends plugin
{
  /* Definitions */
  var $plHeadline= "System logs";
  var $plDescription= "This does something";

  /* attribute list for save action */
  var $attributes     = array();
  var $objectclasses  = array();

  var $server         = "";





  var $start  = 0;
  var $sort   = 2;

  var $sort_direction= "down";
  var $loglevellist= array();
  var $tilist= array();
  var $fields= array("objecttype", "host", "timestamp", "message");
  var $last= array("objecttype", "host", "time", "regex");
  var $range = 25;
  var $view_logged = FALSE;

  function gosa_logview ($config, $dn= NULL)
  {
    /* Include config object */
    $this->config= $config;

    /* Get global filter config */
    if (1==1 || !is_global("gosa_log_filter")){
      $gosa_log_filter= array(
          "time" => "1",
          "objecttype" => "!All",
          "host" => "!All",
          "regex" => "*");

      register_global("gosa_log_filter", $gosa_log_filter);
    }

    $this->ui = get_userinfo();
  }

  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    /* Log view */
    if(!$this->view_logged){
      $this->view_logged = TRUE;
      new log("view","logview/".get_class($this),$this->dn);
    }

    /* Time interval */
    $date_select = array("0" => _("one hour"), "1" => _("6 hours"),
        "2" => _("12 hours"), "3" => _("24 hours"),
        "4" => _("2 days"), "5" => _("one week"),
        "6" => _("2 weeks"), "7" => _("one month"));

    $gosa_log_filter= get_global("gosa_log_filter");
    $smarty= get_smarty();
    $smarty->assign("search_result", "");
    $smarty->assign("plug", "?plug=".validate($_GET['plug']));
    $smarty->assign("search_image", get_template_path('images/search.png'));
    $smarty->assign("time_image", get_template_path('images/time.png'));
    $smarty->assign("server_image", get_template_path('images/server.png'));
    $smarty->assign("log_image", get_template_path('images/log_warning.png'));
    $smarty->assign("ruleset_image", get_template_path('images/edit.png'));
    $smarty->assign("launchimage", get_template_path('images/launch.png'));
    $smarty->assign("loglevellist", $this->loglevellist);
    $smarty->assign("date_select", $date_select);
    $smarty->assign("mode0", "");
    $smarty->assign("mode1", "");
    $smarty->assign("mode2", "");
    $smarty->assign("mode3", "");

    /* Assign select option content */
    foreach( array("host", "objecttype", "time", "regex") as $type){
      $smarty->assign("$type", $gosa_log_filter[$type]);
    }

    /**** 
     * Check if defined servers 
     *  and mysql extension 
     ****/
    if (!isset($this->config->data['SERVERS']['LOGGING']) || !count($this->config->data['SERVERS']['LOGGING'])){
      print_red (_("No GOsa LOG servers defined!"));
      return ($smarty->fetch (get_template_path('gosa_log_contents.tpl', TRUE)));
    }
    
    if(!is_callable("mysql_connect")){
      print_red(_("There is no mysql extension available, please check your php setup."));
      return ($smarty->fetch (get_template_path('gosa_log_contents.tpl', TRUE)));
    }

    /****
     * Get Configurations 
     ****/
    $list_of_servers = $this->config->data['SERVERS']['LOGGING']; 
    $servers         = array();
    foreach($list_of_servers as $servername => $data){
      $servers[$servername] = $servername;
    }
    
    /* Set a default server, if there is currently no valid server selected */
    if(empty($this->server) || !in_array($this->server,$servers)){
      $this->server = key($servers);
    }
   
    /**** 
     * Connect to currently selected server 
     ****/
    $cfg = $list_of_servers[$this->server];
    $link = mysql_pconnect($this->server, $cfg['USER'], $cfg['PWD']);
    if ($link === FALSE){
      print_red(_("Can't connect to log database, no logs can be shown!"));
      new log("debug","logview","",array(),@mysql_error());
      return ($smarty->fetch (get_template_path('gosa_log_contents.tpl', TRUE)));
    }

    /* check of log database is available */
    if (! @mysql_select_db($cfg['DB'])){
      print_red(_("Can't select log database for log generation!"));
      new log("debug","logview","",array(),@mysql_error());
      return ($smarty->fetch (get_template_path('gosa_log_contents.tpl', TRUE)));
    }

    $smarty->assign("regex", $gosa_log_filter['regex']);
    $smarty->assign("servers",$servers);
    $smarty->assign("server",$this->server);
    $smarty->assign("date_select", $date_select);

    /****
     * Check Acls 
     ****/
    $tmp_cat_bases = $this->ui->get_module_departments("logview");
    $all_acls = "";
    foreach($tmp_cat_bases as $acl_base){
      $all_acls .= $this->ui->get_permissions($acl_base,"logview/logview");
    }
    if(count($tmp_cat_bases) == 0 || !preg_match("/r/",$all_acls)){
      $res = "<tr>
        <td colspan=4>
        "._("You have insufficient permissions to view syslog entries.")."
        </td>
        </tr>";

      /* Show main page */
      $smarty->assign("range_selector", "");
      $smarty->assign("search_result", $res);
      return ($smarty->fetch (get_template_path('gosa_log_contents.tpl', TRUE)));
    }


    /****
     * Query stuff 
     ****/
    $res= "";
    $cfg    = $list_of_servers[$this->server];
    $link   = @mysql_pconnect($this->server, $cfg['USER'], $cfg['PWD']);

    /* Test connection object && create up query string */
    if ($link === FALSE){
      print_red(_("Can't connect to log database, no logs can be shown!"));
      new log("debug","logview","",array(),@mysql_error());
    } else {

      if (! @mysql_select_db($cfg['DB'])){
        print_red(_("Can't select log database for log generation!"));
        new log("debug","logview","",array(),@mysql_error());
      } else {

        /* Assemble time query */
        switch ($gosa_log_filter['time']){
          case '0':
            $start=  time() - 3600;
            break;
            ;;
          case '1':
            $start= time() - 21600;
            break;
            ;;
          case '2':
            $start= time() - 43200;
            break;
            ;;
          case '3':
            $start= time() - 86400;
            break;
            ;;
          case '4':
            $start= time() - 172800;
            break;
            ;;
          case '5':
            $start= time() - 604800;
            break;
            ;;
          case '6':
            $start= time() - 1209600;
            break;
            ;;
          case '7':
            $start= time() - 2419200;
            break;
            ;;
        }

        /* Assemble log level query */
        if ($gosa_log_filter['object'] == '!All'){
          $ll= "";
        } else {
          $ll= "AND objecttype='".$gosa_log_filter['objecttype']."'";
        }
        if ($gosa_log_filter['host'] == '!All'){
          $hf= "";
        } else {
          $hf= "AND host='".$gosa_log_filter['host']."'";
        }

        /* Order setting */
        if ($this->sort_direction == "down"){
          $desc= "DESC";
          $sort_sign = "\\/";
        } else {
          $desc= "";
          $sort_sign="/\\";
        }
        $end= date ("YmdHis");
        $query_base= "  FROM 
          gosa_log 
          WHERE 
          result like '".preg_replace("/\*/","%",$gosa_log_filter['regex'])."' 
          $ll 
          $hf 
          AND 
          timestamp <= $end AND timestamp >= $start";

        /* Get number of entries */
        $query= "SELECT COUNT(*)".$query_base.";";
        $result = mysql_query($query);
        if(!$result){
          new log("debug","logview","",array(),@mysql_error());
        }
        $line= mysql_fetch_array($result, MYSQL_ASSOC);
        $count= $line['COUNT(*)'];
        if ($count > 25){
          $smarty->assign("range_selector", range_selector($count, $this->start, $this->range,"EntriesPerPage"));
        } else {
          $smarty->assign("range_selector", "");
        }

        /* Query results */
        $query= "SELECT *".$query_base." ORDER BY ".$this->fields[$this->sort]." $desc LIMIT ".$this->start.",".$this->range.";";
        @DEBUG (DEBUG_MYSQL, __LINE__, __FUNCTION__, __FILE__, $query, "Database query");
        $result = @mysql_query($query);

        if(!$result){
          new log("debug","logview","",array(),@mysql_error());
        }
        /* Display results */
        $mod= 0;

        /* Add entries to result str */
        while ($line = mysql_fetch_array($result, MYSQL_ASSOC)){

          /* Toggle background color */
          if ( ($mod++) & 1){
            $col= "background-color: #ECECEC;";
          } else {
            $col= "background-color: #F5F5F5;";
          }

          $res.=" <tr style=\"$col\">\n";
          $res.="   <td style=\"text-align:center\">
            <img alt=\"".$line['objecttype']."\" 
            src=\"".get_template_path('images/log_'.strtolower($line['objecttype'])).".png\" 
            title=\"Log level is '".$line['objecttype']."'\">
            </td>";
          $res.="   <td>".
            $line['host']."
            </td>"; 
            $res.=" <td>".
            $line['timestamp']."
            </td>";
          $res .= "<td width=\"100%\">".
            $line['message']."
            </td>";
          $res.=" </tr>\n";
        }
        mysql_close($link);
        $smarty->assign("mode".$this->sort, $sort_sign);
        $smarty->assign("host", $gosa_log_filter['host']);
        $smarty->assign("objecttype", $gosa_log_filter['objecttype']);

        $smarty->assign("search_result", $res);
      }
    }

    /* Show main page */
    return ($smarty->fetch (get_template_path('gosa_log_contents.tpl', TRUE)));
  }


  function save_object()
  {
    /* Get submitted range */	
    if(isset($_POST['EntriesPerPage'])){
      if(is_numeric($_POST['EntriesPerPage'])){
        $this->range = $_POST['EntriesPerPage'];
      }
    }

    /* Save data */
    $gosa_log_filter= get_global("gosa_log_filter");

    /* Get actual select boxe values */
    $logfilter_changed = FALSE;
    foreach( array("host", "time", "objecttype", "regex") as $type){
    
      /* Set new value and test if value has changed */
      $last[$type] = $gosa_log_filter[$type];
      if (isset($_POST[$type])){
        $gosa_log_filter[$type]= $_POST[$type];
      }
  
      if ($last[$type] != $gosa_log_filter[$type]){
        $logfilter_changed = TRUE;
      }
    }

    /* Filter regex values */
    if ($gosa_log_filter['regex'] == ""){
      $gosa_log_filter['regex']= '%';
    } else {
      $new = preg_replace('/\*\**/', '*', $gosa_log_filter['regex']);
      $gosa_log_filter['regex']= $new;
    }

    /* Store filter values */
    register_global("gosa_log_filter", $gosa_log_filter);

    /* Set start value */
    if (isset($_GET['start'])){
      $this->start= (int)$_GET['start'];
    }
  
    /* Reset page number if filter has changed */
    if ($logfilter_changed > 0){
      $this->start= 0;
    }

    /* Adapt sorting */
    if (isset($_GET['sort'])){
      if ($this->sort == (int)$_GET['sort']){
        if ($this->sort_direction == "down"){
          $this->sort_direction= "up";
        } else {
          $this->sort_direction= "down";
        }
      }
      $this->sort= (int)$_GET['sort'];
      if ($this->sort < 0 || $this->sort > 3){
        $this->sort= 0;
      }
    }
  }


  /* Return plugin informations for acl handling
     #FIXME You can only read attributes within this report plugin */
  function plInfo()
  {
    return (array(
        "plShortName"   => _("Log view"),
        "plDescription" => _("Log view addon"),
        "plSelfModify"  => FALSE,
        "plDepends"     => array(),
        "plPriority"    => 0,
        "plSection"     => array("addon"),
        "plCategory"    => array("logview" => array("objectClass" => "none", "description" => _("System logs"))),

        "plProvidedAcls" => array()
        ));
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
