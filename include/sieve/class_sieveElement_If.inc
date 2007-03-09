<?php


class sieve_if 
{
  var $_parsed 	= array();
  var $TYPE 	= "if";
  var $id     = -1;

  var $address_parts    = array();
  var $comparators      = array();
  var $match_types      = array();
  var $operators        = array();

  
  /* Initialize class 
   *  $elements   contains all tokens that belongs to this if/else tag
   *  $object_id  cotains an unique tag id, to be able to create uniqe html post names
   */
  function sieve_if($elements,$object_id)
  {
    /* Possible address parts we can select */
    $this->address_parts = array( 
        ":all"       => _("Complete adress")."&nbsp;("._("Default").")",
        ":domain"    => _("Domian part") ,
        ":localpart" => _("Local part"));

    /* comparator type */
    $this->comparators   = array( 
        "i;ascii-casemap" => _("Case insensitive")."&nbsp;("._("Default").")",
        "i;octet"         => _("Case sensitive"),
        "i;ascii-numeric" => _("Numeric"));

    /* Match types */
    $this->match_types  = array(  
        ":is"         => _("is"),
        ":contains"   => _("contains"),
        ":matches"    => _("matches"),
        ":count"      => _("count"),
        ":value"      => _("value is"));

    /* Operators */
    $this->operators = array(     
        "lt"  => _("less than"),
        "le"  => _("less or equal"),
        "eq"  => _("equals"),
        "ge"  => _("greater or equal"),
        "gt"  => _("greater than"),
        "ne"  => _("not equal"));

    $this->id       = $object_id;
    $this->elements = $elements;
    $this->_parsed  = $this->_parse($elements['ELEMENTS'],1);
  }


  /* Returns the sieve script for this 
   *  if/else tag.
   */
  function get_sieve_script_part()
  {
    $tmp = "if ".$this->get_sieve_script_part_recursive($parsed = NULL,$id = 1,$obj_id=1);
    return($tmp);
  } 


  /* Recursivly create a sieve script out of the given 
   *  tags and tokens provided by $parsed.
   *  $id       specifies the depth of the current element.
   *  $obj_id   is the current tag-id handled by this function
   */
  function get_sieve_script_part_recursive($parsed = NULL,$id = 1,$obj_id=1)
  {
    $script ="";
    if($parsed == NULL){
      $parsed = $this->_parsed;
    }

    /* Walk through all elements */
    foreach($parsed as $key => $data){

      /* Create Inverse Tag */
      if(is_array($data) && isset($data['Inverse']) && $data['Inverse']){
        $Inverse = TRUE;
      }else{
        $Inverse = FALSE;
      }

      /* Create elements */
      switch($key)
      {

        /*******************
         * True / False
         *******************/

        case "true" :
        case "false" :
        {
          /* Invert this test if required */
          if($Inverse){
            $script .= "not ";
          }
 
          $script .= $key;
   
          break;
        }


        /*******************
         * Address
         *******************/

        case "address" :   
        {
          /* [not] address 
                        [address-part: tag] 
                        [comparator: tag] 
                        [match-type: tag] 
                        <header-list: string-list> 
                        <key-list: string-list> 
          */

          /* Invert this test if required */
          if($Inverse){
            $script .= "not ";
          }
  
          $script .="address ";
 
          /* Add address part tag */ 
          if(!empty($data['Address_Part']) && $data['Address_Part'] != ":all"){
            $script .= $data['Address_Part']." ";
          }

          /* Add comparator */
          if(!empty($data['Comparator']) && $data['Comparator'] != ""){
            $script .= ":comparator ".$data['Comparator']." ";
          }
    
          /* Add match type */
          $script .= $data['Match_type']." ";

          /* Add special match type for count and value */
          if(in_array($data['Match_type'], array(":value",":count")) && !empty($data['Match_type_value'])) {
            $script .= sieve_create_strings($data['Match_type_value'])." ";
          }

          $script .= sieve_create_strings($data['Key_List']);
          $script .= " ";
          $script .= sieve_create_strings($data['Value_List']);
          break;
        }


        /*******************
         * Header
         *******************/

        case "header" :   
        {
          /* [not] header   
                [comparator: tag] 
                [match-type: tag] 
                <header-names: string-list> 
                <key-list: string-list>
          */

          /* Invert ? */
          if($Inverse){
            $script .= "not ";
          }
  
          $script .="header ";
 
          /* Add address part tag */ 
          if(!empty($data['Address_Part']) && $data['Address_Part'] != ":all"){
            $script .= $data['Address_Part']." ";
          }

          /* Add comparator */
          if(!empty($data['Comparator']) && $data['Comparator'] != ""){
            $script .= ":comparator ".$data['Comparator']." ";
          }
    
          /* Add match type */
          $script .= $data['Match_type']." ";

          /* Add special match type for count and value */
          if(in_array($data['Match_type'], array(":value",":count")) && !empty($data['Match_type_value'])) {
            $script .= sieve_create_strings($data['Match_type_value'])." ";
          }

          $script .= sieve_create_strings($data['Key_List']);
          $script .= " ";
          $script .= sieve_create_strings($data['Value_List']);
          break;
        }


        /*******************
         * Envelope
         *******************/

        case "envelope" :   
        {
          /* [not]  envelope 
                    [address-part: tag] 
                    [comparator: tag] 
                    [match-type: tag] 
                    <envelope-part: string-list> 
                    <key-list: string-list> 
          */

          /* Invert */
          if($Inverse){
            $script .= "not ";
          }
  
          $script .="envelope ";
 
          /* Add address part tag */ 
          if(!empty($data['Address_Part']) && $data['Address_Part'] != ":all"){
            $script .= $data['Address_Part']." ";
          }

          /* Add comparator */
          if(!empty($data['Comparator']) && $data['Comparator'] != ""){
            $script .= ":comparator ".$data['Comparator']." ";
          }
    
          /* Add match type */
          $script .= $data['Match_type']." ";

          /* Add special match type for count and value */
          if(in_array($data['Match_type'], array(":value",":count")) && !empty($data['Match_type_value'])) {
            $script .= sieve_create_strings($data['Match_type_value'])." ";
          }

          $script .= sieve_create_strings($data['Key_List']);
          $script .= " ";
          $script .= sieve_create_strings($data['Value_List']);
          break;
        }


        /*******************
         * Exists
         *******************/
        case "exists" : 
        {
          /* [not] exists 
              <header-names: string-list> 
          */

          /* Invert ? */
          if($Inverse){
            $script .= "not ";
          }

          $script .= "exists ".sieve_create_strings($data['Values']);
          break;
        }


        /*******************
         * Size
         *******************/
        case "size" : 
        {
          /* [not] size 
                <":over" / ":under"> 
                <limit: number> 
          */

          /* Invert ? */
          if($Inverse){
            $script .= "not ";
          }
 
          /* Add size test */ 
          $script .="size ";
          $script .=$data['Match_type']." ";
          foreach($data['Value_List'] as $val){
            $script .= $val." ";
          }
          break;
        }


        /*******************
         * Allof
         *******************/
        case "anyof" :
        case "allof" :
        {
          /* allof <tests: test-list>
             anyof <tests: test-list> */

 
          /* Add spaces, to indent the code.*/ 
          $block = "\n";
          for($i = 0 ; $i < $id ; $i ++){
            $block .= SIEVE_INDENT_TAB;
          }          

          /* Add allof/anyof tag */
          $script.= " ".$key." ( ";

          /* Add each test parameter */
          foreach($data as $key2 => $dat){
            if(($key2 === "Inverse") && ($key2 == "Inverse")){
              continue;
            }
            $script.= $block.$this->get_sieve_script_part_recursive($dat, ($id +1),$key2).", ";
          }
    
          /* Remove last _,_ and close the tag */
          $script = preg_replace("/,$/","",trim($script));
          $script.= $block.")";
          break ;
        }

        default :
        {
          $script .= "THERE IS SOME IMPLEMENTATION MISSING FOR SIEVE SCRIPT CREATION :".$key;
        }
      }
    }
    return($script);
  }


  /* Ensure that all changes made on the ui 
   *  will be saved. 
   */
  function save_object()
  {
    $tmp = $this->save_object_recursive($parsed = NULL,$id = 1,$obj_id=1);
    $this->_parsed = $tmp;
  }


  /* Recursivly save all ui changes for the 
   *  tags and tokens provided by $parsed.
   *  $id       specifies the depth of the current element.
   *  $obj_id   is the current tag-id handled by this function
   */
  function save_object_recursive($parsed = NULL,$id = 1,$obj_id=1)
  {
    /* Variable initialization */ 
    $ret ="";
    if($parsed == NULL){
      $parsed = $this->_parsed;
    }

    /* Walk through all elements */
    foreach($parsed as $key => $data){

      /* Id used to have unique html names */
      $element_id = $this->id."_".$id."_".$obj_id;

      /* Create elements */
      switch($key)
      {
 
        /*******************
         * Address 
         *******************/

        case "envelope" :
        case "header" : 
        case "address" : 
        {
          /* [not] address 
                        [address-part: tag] 
                        [comparator: tag] 
                        [match-type: tag] 
                        <header-list: string-list> 
                        <key-list: string-list> 
          */

          /* Possible address parts we can select */
          $address_parts = $this->address_parts;
          $comparators   = $this->comparators;
          $match_types   = $this->match_types; 
          $operators     = $this->operators;

          /* Toggle Inverse ? */
          if(isset($_POST['toggle_inverse_'.$element_id])){
            $parsed[$key]['Inverse'] = !$parsed[$key]['Inverse'];
          }

          /* Check if we want to toggle the expert mode */
          if(isset($_POST['Toggle_Expert_'.$element_id])){
            $parsed[$key]['Expert'] = !$parsed[$key]['Expert'];
          }

          /* Get address part */
          if(isset($_POST['address_part_'.$element_id])){
            $ap = $_POST['address_part_'.$element_id];

            if(isset($address_parts[$ap])){
              $parsed[$key]['Address_Part'] = $ap;
            }
          }

          /* Check if match type has changed */
          if(isset($_POST['matchtype_'.$element_id])){
            $mt = $_POST['matchtype_'.$element_id];

            if(isset($match_types[$mt])){
              $parsed[$key]['Match_type'] = $mt;
            }
          }

          /* Get the comparator tag, if posted */
          if(isset($_POST['comparator_'.$element_id])){
            $cp = $_POST['comparator_'.$element_id];

            if(isset($comparators[$cp])){
              $parsed[$key]['Comparator'] = $cp;
            }
          }

          /* In case of :count and :value match types 
           *  we have a special match operator we should save.
           */
          if(in_array($parsed[$key]['Match_type'],array(":value",":count"))){
            if(isset($_POST['operator_'.$element_id])){
              $op = $_POST['operator_'.$element_id];

              if(isset($operators[$op])){
                $parsed[$key]['Match_type_value'] = $op;
              }
            }
          }

          /* Get the address fields we should check, they are seperated by , */
          if(isset($_POST['keys_'.$element_id])){
            $vls = stripslashes($_POST['keys_'.$element_id]);
            $tmp = array();

            $tmp2 = split(",",$vls);
            foreach($tmp2 as $val){
              $tmp[] = "\"".trim(preg_replace("/\"/","",$val))."\"";
            }
            $parsed[$key]['Key_List'] = $tmp;
          }

          /* Get the values should check for, they are seperated by , */
          if(isset($_POST['values_'.$element_id])){
            $vls = stripslashes($_POST['values_'.$element_id]);
            $tmp = array();

            $tmp2 = split(",",$vls);
            foreach($tmp2 as $val){
              $tmp[] = "\"".trim(preg_replace("/\"/","",$val))."\"";
            }
            $parsed[$key]['Value_List'] = $tmp;
          }
          break;
        }
 
        /*******************
         * TRUE FALSE 
         *******************/

        case "true" :
        case "false" : 
        {
          $name = 'boolean_'.$element_id;
          if(isset($_POST[$name])){
            $key2 = $_POST[$name];
            
            if($key != $key2) {
              $parsed = array($key2 => $key2); 
            }
          }
          break;
        }

        /*******************
         * Exists 
         *******************/

        case "exists" :
        {
          /* Toggle Inverse ? */
          if(isset($_POST['toggle_inverse_'.$element_id])){
            $parsed[$key]['Inverse'] = !$parsed[$key]['Inverse'];
          }

          /* get list of match values */
          if(isset($_POST['Values_'.$element_id])){
            $vls = stripslashes($_POST['Values_'.$element_id]);
            $tmp = array();          
  
            $tmp2 = split(",",$vls);
            foreach($tmp2 as $val){
              $tmp[] = "\"".trim(preg_replace("/\"/","",$val))."\"";
            }
            $parsed['exists']['Values'] = $tmp;
          }
          break;
        }

        /*******************
         * Size 
         *******************/

        case "size" :
        {
          $Match_types = array( ":over" => _("greater than") ,
                                ":under" => _("lower than"));

          $Units       = array( "M" => _("Megabyte") ,
                                "K" => _("Kilobyte"));

          /* Reset error */
          $parsed['size']['LastError'] ="";

          /* Get match type */
          if(isset($_POST['Match_type_'.$element_id])){
            $mt = $_POST['Match_type_'.$element_id];
            if(isset($Match_types[$mt])){
              $parsed['size']['Match_type'] = $mt;
            }else{
              $parsed['size']['LastError'] = _("Please select a valid match type in the list box below.");
            }
          }

          /* Get old values */
          $value = preg_replace("/[^0-9]*$/","",$parsed['size']['Value_List'][0]);
          $unit  = preg_replace("/^[0-9]*/","",$parsed['size']['Value_List'][0]);

          /* Get value */
          if(isset($_POST['Value_'.$element_id])){
            $vl = $_POST['Value_'.$element_id];
         
            if(is_numeric($vl) && preg_match("/^[0-9]*$/",$vl)){
              $value = $vl;
            }else{
              $parsed['size']['LastError'] = _("Only numeric values are allowed here.");
            }
          }        

          /* Get unit */
          if(isset($_POST['Value_Unit_'.$element_id])){
            $ut = $_POST['Value_Unit_'.$element_id];
       
            if(isset($Units[$ut])){
              $unit = $ut;
            }else{
              $parsed['size']['LastError'] = _("No valid unit selected");
            }
          }        

          $parsed['size']['Value_List'][0] = $value.$unit;
          break;
        }

        /*******************
         * Allof 
         *******************/
     
        case "allof" : 
        {
          if(isset($_POST['toggle_inverse_'.$element_id])){
            $parsed[$key]['Inverse'] = !$parsed[$key]['Inverse'];
          }
          foreach($data as $key2 => $dat){
            if(($key2 === "Inverse") && ($key2 == "Inverse")){
              continue;
            }
            $parsed[$key][$key2] = $this->save_object_recursive($dat, ($id +1),$key2);
          }
          break ;
        } 

        /*******************
         * Anyof 
         *******************/
     
        case "anyof" : 
        {
          if(isset($_POST['toggle_inverse_'.$element_id])){
            $parsed[$key]['Inverse'] = !$parsed[$key]['Inverse'];
          }
          foreach($data as $key2 => $dat){
            if(($key2 === "Inverse") && ($key2 == "Inverse")){
              continue;
            }
            $parsed[$key][$key2] =  $this->save_object_recursive($dat, ($id + 1),$key2);
          }
          break ;
        } 
      }
    }
    return($parsed);
  }  


  /* Return html element for IF */ 
  function execute()
  {
    /* Create title */
    $name  = "<img alt='' src='images/small_filter.png' class='center'>";
    $name .= "<b>"._("Condition")."</b>";
    if($this->TYPE == "if"){
      $name .= "&nbsp;-&nbsp;"._("If");
    }else{
      $name .= "&nbsp;-&nbsp;"._("Else");
    }

    $smarty = get_smarty();
    $smarty->assign("ID", $this->id);

    /* Only display navigation elements if necessary */
    if($this->TYPE == "if"){
      $object_container = $smarty->fetch(get_template_path("templates/object_container.tpl",TRUE,dirname(__FILE__)));
    }else{
      $object_container = $smarty->fetch(get_template_path("templates/object_container_clear.tpl",TRUE,dirname(__FILE__)));
    }

    $smarty->assign("Name", $name);
    $smarty->assign("Contents", $this->get_as_html());
    $object = $smarty->fetch(get_template_path("templates/element_if_else.tpl",TRUE,dirname(__FILE__)));



    $str = preg_replace("/%%OBJECT_CONTENT%%/",$object,$object_container);
   

    return($str);
  }

  
  /* Returns all elements as html */
  function get_as_html($parsed = NULL,$id = 1,$obj_id=1)
  {
    $ret ="";
    if($parsed == NULL){
      $parsed = $this->_parsed;
    }

    /* Walk through all elements */
    foreach($parsed as $key => $data){

      /* Create Inverse Tag */
      if(is_array($data) && isset($data['Inverse']) && $data['Inverse']){
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

        case "header": 
        {
          $address_parts = $this->address_parts;
          $comparators   = $this->comparators;
          $match_types   = $this->match_types; 
          $operators     = $this->operators;

          $smarty = get_smarty();
          $smarty->assign("comparators",$comparators);
          $smarty->assign("match_types",$match_types);
          $smarty->assign("operators",$operators);

          $smarty->assign("match_type", $data['Match_type']);
          $smarty->assign("operator"  , preg_replace("/\"/","",$data['Match_type_value']));
          $smarty->assign("comparator", $data['Comparator']);
        

          $keys = "";
          foreach($data['Key_List'] as $key){
            $keys .= $key.", ";
          }
          $keys = preg_replace("/,$/","",trim($keys));
   
          $values = "";
          foreach($data['Value_List'] as $key){
            $values .= $key.", ";
          }
          $values = preg_replace("/,$/","",trim($values));

          $smarty->assign("keys",$keys);
          $smarty->assign("Inverse",$Inverse);
          $smarty->assign("values",$values);
          $smarty->assign("Expert", $data['Expert']);

 
          $smarty->assign("ID"  , $element_id); 
          $ret .= $smarty->fetch(get_template_path("templates/element_header.tpl",TRUE,dirname(__FILE__)));
          break;
        }


        /*******************
         * Envelope 
         *******************/

        case "envelope":
        {
          $address_parts = $this->address_parts;
          $comparators   = $this->comparators;
          $match_types   = $this->match_types; 
          $operators     = $this->operators;

          $smarty = get_smarty();
          $smarty->assign("Inverse",$Inverse);
          $smarty->assign("comparators",$comparators);
          $smarty->assign("Expert", $data['Expert']);
          $smarty->assign("match_types",$match_types);
          $smarty->assign("operators",$operators);

          $smarty->assign("match_type", $data['Match_type']);
          $smarty->assign("operator"  , preg_replace("/\"/","",$data['Match_type_value']));
          $smarty->assign("comparator", $data['Comparator']);

          $keys = "";
          foreach($data['Key_List'] as $key){
            $keys .= $key.", ";
          }
          $keys = preg_replace("/,$/","",trim($keys));

          $values = "";
          foreach($data['Value_List'] as $key){
            $values .= $key.", ";
          }
          $values = preg_replace("/,$/","",trim($values));
          $smarty->assign("keys",$keys);
          $smarty->assign("values",$values);

          $smarty->assign("ID"  , $element_id); 
          $ret .= $smarty->fetch(get_template_path("templates/element_envelope.tpl",TRUE,dirname(__FILE__)));
          break;
        }


        /*******************
         * Address 
         *******************/

        case "address" : 
        {
          $address_parts = $this->address_parts;
          $comparators   = $this->comparators;
          $match_types   = $this->match_types; 
          $operators     = $this->operators;

          $smarty = get_smarty();
          $smarty->assign("Inverse",$Inverse);
          $smarty->assign("address_parts",$address_parts);
          $smarty->assign("comparators",$comparators);
          $smarty->assign("match_types",$match_types);
          $smarty->assign("operators",$operators);

          $smarty->assign("match_type", $data['Match_type']);
          $smarty->assign("operator"  , preg_replace("/\"/","",$data['Match_type_value']));
          $smarty->assign("comparator", $data['Comparator']);
          $smarty->assign("address_part", $data['Address_Part']);

          $smarty->assign("Expert", $data['Expert']);
        
          $keys = "";
          foreach($data['Key_List'] as $key){
            $keys .= $key.", ";
          }
          $keys = preg_replace("/,$/","",trim($keys));
   
          $values = "";
          foreach($data['Value_List'] as $key){
            $values .= $key.", ";
          }
          $values = preg_replace("/,$/","",trim($values));
          $smarty->assign("keys",$keys);
          $smarty->assign("values",$values);

 
          $smarty->assign("ID"  , $element_id); 
          $ret .= $smarty->fetch(get_template_path("templates/element_address.tpl",TRUE,dirname(__FILE__)));
          break;
        }
      

        /*******************
         * Size 
         *******************/
        
        case "size" : 
        {
          $Match_types = array( ":over" => _("greater than") , 
                                ":under" => _("lower than"));

          $Units       = array( "M" => _("Megabyte") , 
                                "K" => _("Kilobyte")); 

          $Match_type   = $data['Match_type'];
          $Value        = preg_replace("/[^0-9]/","",$data['Value_List'][0]);
          $Value_Unit   = preg_replace("/[0-9]/","",$data['Value_List'][0]);
       
          $LastError = "";
          if(isset($data['LastError'])){
            $LastError = $data['LastError'];
          }
 
          $smarty = get_smarty();
          $smarty->assign("Inverse",$Inverse);
          $smarty->assign("LastError",$LastError);
          $smarty->assign("Match_types",$Match_types);
          $smarty->assign("Units",$Units);
          $smarty->assign("Match_type",$Match_type);
          $smarty->assign("Value",$Value);
          $smarty->assign("Value_Unit",$Value_Unit);
          $smarty->assign("ID"  , $element_id); 
          $ret .= $smarty->fetch(get_template_path("templates/element_size.tpl",TRUE,dirname(__FILE__)));
          break;
        }
        
        /*******************
         * Exists 
         *******************/
       
        
        case "exists" : 
        {
          $LastError = "";
          if(isset($data['LastError'])){
            $LastError = $data['LastError'];
          }
 
          $Values = "";
          foreach($data['Values'] as $val){
            $Values .= $val.", ";
          }
          $Values = preg_replace("/,$/","",trim($Values));

          $smarty = get_smarty();
          $smarty->assign("LastError",$LastError);
          $smarty->assign("Values",$Values);
          $smarty->assign("Inverse",$Inverse);
          $smarty->assign("ID"  , $element_id); 
          $ret .= $smarty->fetch(get_template_path("templates/element_exists.tpl",TRUE,dirname(__FILE__)));
          break;
        }
  

        /*******************
         * All of   
         *******************/

        case "allof" : 
        {
          $Contents = ""; 
          foreach($data as $key => $dat){
            if(($key === "Inverse") && ($key == "Inverse")){
              continue;
            }
            $Contents .=        $this->get_as_html($dat, ($id +1),$key);
          }
          $smarty = get_smarty();
          $smarty->assign("Inverse",$Inverse);
          $smarty->assign("Contents",$Contents);
          $smarty->assign("ID"  , $element_id); 
          $ret .= $smarty->fetch(get_template_path("templates/element_allof.tpl",TRUE,dirname(__FILE__)));
          break ;
        } 


        /*******************
         * Any of   
         *******************/

        case "anyof" : 
        {
          $Contents = ""; 
          foreach($data as $key => $dat){
            if(($key === "Inverse") && ($key == "Inverse")){
              continue;
            }
            $Contents .=        $this->get_as_html($dat, ($id +1),$key);
          }
          $smarty = get_smarty();
          $smarty->assign("Inverse",$Inverse);
          $smarty->assign("Contents",$Contents);
          $smarty->assign("ID"  , $element_id); 
          $ret .= $smarty->fetch(get_template_path("templates/element_anyof.tpl",TRUE,dirname(__FILE__)));
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


  /* Parse given token identified by $data[$id] 
   *  and return the parsed tokens. 
   */
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

      /****************
       * Parse - Envelope / Header / Address
       ****************/ 

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
            if(in_array($Match_type,array(":value",":count"))){
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
                                "Expert"          => FALSE,
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


      /****************
       * Parse - Size
       ****************/ 

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


      /****************
       * Parse - True / False
       ****************/ 

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


      /****************
       * Parse - Exists
       ****************/ 

      case "exists":
      {
        
        /* Skip first values, [if,not,exists] */
        $node = $data[$id];
        while(in_array($node['text'],array("if","not","exists"))){
          $id ++;
          $node = $data[$id];
        }

        /* Get values */
        $tmp2 = sieve_get_strings($data,$id);
  
        
        $tmp['exists'] = array('Inverse' => $Inverse,
                               'Values'  => $tmp2['STRINGS']);
        break;
      }


      /****************
       * Parse - Allof
       ****************/ 

      case "allof" :
      {
        /* Get parameter and recursivly call this method 
         *  for each parameter 
         */
        $id ++;
        $tmp2 = $this->get_parameter($data,$id);
        
        foreach($tmp2 as $parameter){
          $tmp['allof'][] = $this->_parse($parameter);
        }
        $tmp['allof']['Inverse'] = $Inverse;
        break;
      }


      /****************
       * Parse - Anyof
       ****************/ 

      case "anyof" :
      {
        /* Get parameter and recursivly call this method 
         *  for each parameter 
         */
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
