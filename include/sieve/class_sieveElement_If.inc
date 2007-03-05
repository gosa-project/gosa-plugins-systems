<?php


class sieve_if 
{
  var $_parsed 	= array();
  var $TYPE 	= "if";
  var $id     = -1;

  function sieve_if($elements,$object_id)
  {
    $this->id = $object_id;
    $this->elements = $elements;
    $this->_parsed = $this->_parse($elements['ELEMENTS'],1);
  }


  /* Return html element for IF */ 
  function execute()
  {
    /* Create title */
    $name  = "<img src='images/small_filter.png' class='center'>";
    $name .= "<b>"._("Condition")."</b>";
    if($this->TYPE == "if"){
      $name .= "&nbsp;-&nbsp;"._("If");
    }else{
      $name .= "&nbsp;-&nbsp;"._("Else");
    }

    /* Create new html block */
    $str  ="<table cellspacing=0 width='100%'>
              <tr>
                <td style='width:100%;background-color:#DDDDDD; padding:5px; border: solid 2px #AAAAAA;'>".
                  $name;
    $str .= $this->get_as_html();  
    $str .= "   </td>
              </tr>
            </table>";
    return($str);
  }

  
  /* Returns all elements as html */
  function get_as_html($parsed = NULL,$id = 1,$obj_id=1)
  {
    $header_parts = array(":all",":localpart",":domain",":user",":detail");

    $ret ="";
    if($parsed == NULL){
      $parsed = $this->_parsed;
    }

    /* Walk through all elements */
    foreach($parsed as $key => $data){

      /* Create Inverse Tag */
      if(isset($data['Inverse']) && $data['Inverse']){
        $str_inverse = "<font color='red'><b>"._("Not")."</b></font>&nbsp;";
        $Inverse = TRUE;
      }else{
        $str_inverse = "";
        $Inverse = FALSE;
      }

      /* Id used to have unique html names */
      $element_id = $this->id."_".$id."_".$obj_id;

      /* Create elements */
      switch($key)
      {
  
        /*******************
         * TRUE FALSE 
         *******************/

        case "true" :
        case "false" : 
        { 
          /* Inverse element if required */
          if($Inverse){        
            if($key == "true"){
              $key = "false";
            }else{
              $key = "true";
            }           
          }

          /* Get template */
          $smarty = get_smarty();
          $smarty->assign("values"    , array("false" => _("False"), "true" => _("True")));
          $smarty->assign("selected"  , $key); 
          $smarty->assign("ID"  , $element_id); 
          $ret .= $smarty->fetch(get_template_path("templates/element_boolean.tpl",TRUE,dirname(__FILE__)));
          break;
        }


        /*******************
         * Header  
         *******************/

        case "address" : 
        {
          $ret ="";    

          print_a($data);

          $comparators = array("i;octet" => _("Normal"),"i;ascii-casemap"=>_("Case sensitive"),"i;ascii-numeric"=>_("Numeric"));
          $av_match_type= array(":is",":contains",":matches",":over",":count",":value",":under");
          $mtv = array("lt","le","eq","ge","gt","ne");

          $smarty = get_smarty();
          $smarty->assign("c_values"    , $comparators);
          $smarty->assign("c_selected"  , $data['Comparator']);

          $smarty->assign("match_types",$av_match_type);
          $smarty->assign("match_type",$data['Match_type']);

          $smarty->assign("match_type_value",$data['Match_type_value']);
          $smarty->assign("match_type_values",$mtv);
 
          $keys = "";
          foreach($data['Key_List'] as $key){
            $keys .= $key."\n";
          }
   
          $values = "";
          foreach($data['Value_List'] as $key){
            $values .= $key."\n";
          }
          $smarty->assign("keys",$keys);
          $smarty->assign("values",$values);

 
          $smarty->assign("ID"  , $element_id); 
          $ret .= $smarty->fetch(get_template_path("templates/element_address.tpl",TRUE,dirname(__FILE__)));
          break;
        }
      

        /*******************
         * All of   
         *******************/

        case "allof" : 
        {
          $ret = "<table width='100%'  cellspacing=0 cellpadding=0>
                    <tr>
                      <td style='text-align:center; vertical-align: middle; width:45px; 
                                 background-color: #BDBDBD; border: solid 1px #EEEEEE'>".
                        "<b>".$str_inverse._("All of")."</b>".
                        "<img class='center' src='images/select_ogroup.png'>".
                     "</td>";
          $ret.= "    <td style='background-color:#BDBDBD ; border: solid 1px #EEEEEE'>";
          foreach($data as $key => $dat){
            if($key == "Inverse" ){
              continue;
            }
            $ret.=        $this->get_as_html($dat, ($id +1),$key);
          }
          $ret.= "    </td>
                    </tr>
                  </table>";
          break ;
        } 


        /*******************
         * Any of   
         *******************/

        case "anyof" : 
        {
          $ret = "<table width='100%' cellspacing=0 cellpadding=0>
                    <tr>
                      <td style='text-align:center; vertical-align: middle; width:45px; 
                                 background-color: #AAAAAA; border: solid 1px #EEEEEE'>".
                        "<b>".$str_inverse._("Any of")."</b>".
                        "<img class='center' src='images/select_department.png'>".
                     "</td>";
          $ret.= "    <td style='background-color: #AAAAAA ; border: solid 1px #EEEEEE'>";
          foreach($data as $key => $dat){
            if($key == "Inverse" ){
              continue;
            }
            $ret.=        $this->get_as_html($dat, ($id + 1),$key);
          }
          $ret.= "    </td>
                    </tr>
                  </table>";
          break ;
        }
        default : 
          {
            $ret = "<table width='100%'  cellspacing=0 cellpadding=0>
                      <tr>
                        <td style='background-color: #FEDCA9 ; border: solid 1px        #EEEEEE'>";
            $ret.= $key."<br>"; 
            $ret.= "    </td>
                      </tr>
                    </table>";

          }
      }
    }
    return($ret);
  }


  function _parse($data,$id = 0)
  {
    $av_methods   = array("address","allof","anyof","exists","false","header","not","size","true","envelope");
    $av_match_type= array(":is",":contains",":matches",":over",":count",":value",":under");
    
    $type = $data[$id]['text'];

    $tmp = array();


    /* Is there an identifier named 'not' to inverse this filter ? */
    $Inverse = FALSE;
    if($data[$id]['class'] == "identifier" && $data[$id]['text'] == "not"){
      $Inverse = TRUE;
      $id ++;
      $type = $data[$id]['text'];
    }

    switch($type)
    {
  
      case "envelope" : 
      case "header":
      case "address" : 
      {
        /* Address matches are struckture as follows :
           [not] 
           address 
                  [address-part: tag]           all|localpart|domain|user|detail
                  [comparator: tag]             i;octet i;ascii-casemap i;ascii-numeric
                  [match-type: tag]             is|contains|matches|count|value 
                  <header-list: string-list> 
                  <key-list: string-list>   
          */ 
   
        
        $part     = "(:all|:localpart|:domain)";
        $operator = "(:contains|:is|:matches|:count|:value)";
        $value_op = "(lt|le|eq|ge|gt|ne)";

        $Address_Part     = "";
        $Comparator       = "";        
        $Match_type       = "";    
        $Match_type_value = "";
  
        $Key_List         = array();
        $Value_List       = array();
  
        for($i = 0 ; $i < count($data) ; $i ++){
         
          /* Get next node */ 
          $node = $data[$i];
  
          /* Check address part definition */
          if($node['class'] == "tag" && preg_match("/".$part."/i",$node['text'])){
            $Address_Part = $node['text'];
          }

          /* Check for match type  */
          elseif($node['class'] == "tag" && preg_match("/".$operator."/i",$node['text'])){
            $Match_type = $node['text'];

            /* Get value operator */
            if($Match_type == ":value"){
              $i ++;        
              $node = $data[$i];
              
              if($node['class'] == "quoted-string" && preg_match("/".$value_op."/",$node['text'])){
                $Match_type_value = $node['text'];
              }
            }
          } 
  
          /* Check for a comparator */
          elseif($node['class'] == "tag" && preg_match("/comparator/",$node['text'])){
            $i ++;
            $node = $data[$i];
            $Comparator = $node['text'];
          }
  
          /* Check for Key_List */  
          elseif(count(sieve_get_strings($data,$i))){
            $tmp2 = sieve_get_strings($data,$i);
            $i =  $tmp2['OFFSET'];

            if(!count($Key_List)){
              $Key_List = $tmp2['STRINGS'];
            }else{
              $Value_List = $tmp2['STRINGS']; 
            }
          } 
      
        }
 
         
        /* Add to Tree */ 
        $values = array( "Inverse"         => $Inverse,
                                "Comparator"      => $Comparator,
                                "Match_type"      => $Match_type,
                                "Match_type_value"=> $Match_type_value,
                                "Key_List"        => $Key_List,
                                "Value_List"      => $Value_List) ;
        if($type == "address"){
          $values["Address_Part"]    = $Address_Part;
        }
        $tmp[$type] = $values;
        break;
      }


      case "size":
      {
    
        $ops = "(:over|:under)";

        $Match_type = "";

        for($i = $id ; $i < count($data); $i++){

          /* Get current node */
          $node = $data[$i];

          /* Get tag (under / over) */
          if($node['class'] == "tag" && preg_match("/".$ops."/",$node['text'])){
            $Match_type = $node['text'];
          }
          
          /* Get Value_List, the value that we want to match for */
          elseif(count(sieve_get_strings($data,$i))){
            $tmp2 = sieve_get_strings($data,$i);
            $i =  $tmp2['OFFSET'];
          
            $Value_List = $tmp2['STRINGS'];
          } 
        }        
    
        $tmp[$type]= array( "Inverse"    => $Inverse,
                            "Match_type" => $Match_type,
                            "Value_List" => $Value_List);
        break;
      }
      case "true": 
      {
        $tmp['true'] = "true";
        break;
      }
      case "false":
      {
        $tmp['false'] = "false";
        break;
      }
      case "allof" :
      {
        $id ++;
        $tmp2 = $this->get_parameter($data,$id);
        
        foreach($tmp2 as $parameter){
          $tmp['allof'][] = $this->_parse($parameter);
        }
        $tmp['allof']['Inverse'] = $Inverse;
        break;
      }

      case "anyof" :
      {
        $id ++;
        $tmp2 = $this->get_parameter($data,$id);

        foreach($tmp2 as $parameter){
          $tmp['anyof'][] = $this->_parse($parameter);
        }
        $tmp['anyof']['Inverse'] = $Inverse;
        break;
      }
      default : $tmp[$id] = $type; 
    }
    
    return($tmp); 
  }


  function get_parameter($data,$id)
  {
    $par = array();
    $open_brakets = 0;
    $next = NULL;
    $num = 0;
    for($i = $id ; $i < count($data) ; $i++ ){
      if(in_array($data[$i]['class'],array("left-parant","left-bracket"))){
        $open_brakets ++;
      }
      if($data[$i]['class'] == "comma" && $open_brakets == 1){
        $num ++;
      }
      if(!in_array($data[$i]['class'],array("comma","left-parant","right-parant")) || $open_brakets >1 ){
        $par[$num][] = $data[$i];
      }
      if(in_array($data[$i]['class'],array("right-parant","right-bracket"))){
        $open_brakets --;
      }
    }
    return($par);
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
