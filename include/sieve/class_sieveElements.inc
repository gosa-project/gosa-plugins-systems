<?php


class sieve_element
{

  function execute()
  {
    return("NIY<br>");
  }
}

class sieve_elsif extends sieve_if {}
class sieve_if extends sieve_element
{

  /* Contains all tests with an uniqe id */
  var $test_list = array();
      
  /* Available test types */
  var $TESTS = array("address","allof","anyof","exists","false","header","not","size","true","envelope");
  
  var $elements = array();


  function sieve_if($elements)
  {
    $this->elements = $elements;
    $this->_parse($elements);
  }

  function execute()
  {
    return("-> "._("Condition"));
  }


  function _parse($data,$id = 0)
  {
    /* Objects that could be tested */
    $av_methods   = array("address","allof","anyof","exists","false","header","not","size","true","envelope");

    /* Tests that could be used */
    $av_match_type= array(":is",":contains",":matches",":over",":count",":value");

    /* Variable initialization */
    $test_method    = ""; // The object that we want to test, like 'header'
    $match_type     = ""; // The operator like :contains
    $obj_attribute  = ""; // The attribute we want to check, like 'from'
    $obj_match_value= ""; // The value that we want to match for.
    $comparator     = ""; // The comperator specifies the type of values that should be matched.

    $node = $data['ELEMENTS'][$id];

    /* Skip the if / else identifier */
    if($node ['class'] == "identifier" && in_array($node['text'],array("if","elsif"))){
      $id ++;
      $node = $data['ELEMENTS'][$id];
    }

    /* Get current command like "allof" or "header" */
    if($node ['class'] == "identifier" && in_array($node['text'],$av_methods)){
      $test_method = $node['text'];
    }

    echo $id;

    /* switch different types */
    switch($test_method)
    {
      case "allof"  : $this->_parse($elements,$id);
      case "anyof"  : $this->_parse($elements,$id);

      case "address" : 
      {
        /* header :comparator <string> :optional-operator :operator attribute match-value */
        $tmp = array();
        $tmp['OPTIONAL_OP']     ="";
        $tmp['COMPARATOR']      ="";
        $tmp['OPERATOR']        ="";
        $tmp['ATTRIBUTE']       ="";
        $tmp['MATCH-VALUE']     ="";
        

      }
      default : echo $test_method."<br>" ;
    }
  }
};


class sieve_comment extends sieve_element
{
  var $data = "";
 
    
  function sieve_comment($data)
  {
    foreach($data['ELEMENTS'] as $node){
       $this->data .= $node['text'];
    }
  }

  function execute()
  {
    return(_("Comment")."&nbsp;<input type='text' name='comment_' value='".$this->data."'>");
  }
}


class sieve_require extends sieve_element
{
  var $data = array();

  function sieve_require($data)
  {
    foreach($data['ELEMENTS'] as $node ){
      if(in_array($node['class'],array("quoted-string","text"))){
        $this->data[] = preg_replace("/\"/","",$node['text']);
      }
    }
  }

  function execute()
  {
    $str = _("Sieve includes");
    foreach($this->data as $req){
      $str .= "&nbsp;<i>".$req."</i>";
    }
    return($str);
  }
}

class sieve_discard extends sieve_element
{
  var $data = array();

  function sieve_discard($data)
  {
  }

  function execute()
  {
    $str = _("Discard mail");
    return($str);
  }
}



class sieve_reject extends sieve_element
{
  var $data = array();

  function sieve_reject($data)
  {
    $str = "";
    foreach($data['ELEMENTS'] as $node ){
      if(in_array($node['class'],array("quoted-string","text"))){
        $str .= $node['text'];
      }
    }
    $this->data = preg_replace("/\"/","",$str);
  }

  function execute()
  {
    $str = _("Reject mail");
    $str .= "&nbsp;<textarea name='test' style='width:90%'>".$this->data."</textarea>";
    return($str);
  }
}

class sieve_redirect extends sieve_element
{
  var $data = array();

  function sieve_redirect($data)
  {
    foreach($data['ELEMENTS'] as $node ){
      if(in_array($node['class'],array("quoted-string","text"))){
        $this->data[] = $node['text'];
      }
    }
  }

  function execute()
  {
    $str = _("Redirect to");
    foreach($this->data as $dest){
      $str .= "<input type='text' name='bal' value='".$dest."'><br> ";
    }
    return($str);
  }
}

class sieve_fileinto extends sieve_element
{
  var $data = array();

  function sieve_fileinto($data)
  {
    foreach($data['ELEMENTS'] as $node ){
      if(in_array($node['class'],array("quoted-string","text"))){
        $this->data[] = preg_replace("/\"/","",$node['text']);
      }
    }
  }

  function execute()
  {
    $str = _("File into");

    $str .= "<select name='test'>";
    foreach($this->data as $folder){
      $str .= "<option>".$folder."</option>";
    }
    $str .= "</select>";

    return($str);
  }
}

class sieve_vacation extends sieve_element 
{
  var $days     = FALSE;
  var $subject  = FALSE;
  var $from     = "";
  var $mime     = "";
  var $hanlde   = "";
  var $reason   = "";
  var $addresses= array();

  function sieve_vacation($data)
  {
    /* Usage:   vacation [":days" number] [":subject" string]
       [":from" string] [":addresses" string-list]
       [":mime"] [":handle" string] <reason: string> */

    /* Not all attribute types are supported by the sieve class right now */
    $known_attrs = array(":days",":subject",":from",":mime",":handle");

    /* Walk through elements */
    for($i = 0 ; $i < count($data['ELEMENTS']) ; $i ++){

      /* get current element */
      $node = $data['ELEMENTS'][$i];

      /* Check if tag is in the specified list of attributes */
      if($node['class'] == "tag" && in_array($node['text'],$known_attrs)){

        $var = preg_replace("/\:/","",$node['text']);
        $this->$var = $data['ELEMENTS'][$i+1]['text'];
        $i ++;
      }

      /* Check for addresses */
      if($node['class'] == "tag" && $node['text'] == ":addresses") {
        $this->addresses = array();
        $i ++;

        /* Multiple or single address given */
        if($data['ELEMENTS'][$i]['class'] == "left-bracket"){
          while($data['ELEMENTS'][$i]['class'] != "right-bracket" && ($i < count($data['ELEMENTS']))){
            $i ++;
            if($data['ELEMENTS'][$i]['class'] == "quoted-string"){
              $this->addresses[] = preg_replace("/\"/i","",$data['ELEMENTS'][$i]['text']);
            }
          }
        }else{
          $this->addresses[] = $data['ELEMENTS'][$i]['text'] ;
        }
      }

      /* Add the vacation message */
      if($node['class'] == "quoted-string"){
        $this->reason = $node['text'];
      }
    }
  }


  function execute()
  {
    $str ="";
    $str .="<b>"._("Vacation message")."</b>";
    foreach($this->addresses as $addr){ 
      $str .="<br><input type='text' value='".$addr."' style='width:50%;'>";
    }
    $str .="<br><textarea style='width:100%;height:60px;'>".$this->reason."</textarea>";
    
    return($str);
  }
}

class sieve_block_start extends sieve_element 
{
  function execute()
  {
    return("<table width='100%'> <tr><td style='background-color:#BBBBBB;border: solid 2px #FFFFFF;padding-left:20px;'>");
  }
}

class sieve_block_end extends sieve_element 
{
  function execute()
  {
    return("</td></tr></table>");
  }
}

/* This class handles the keep statement */
class sieve_keep extends sieve_element 
{
  function execute()
  {
    return("->  "._("Keep message"));
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
