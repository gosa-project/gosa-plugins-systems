<?php
/*
   This code is part of GOsa (https://gosa.gonicus.de)
   Copyright (C) 2007 Fabian Hickert

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

require_once("class_setupStep.inc");

class setup extends plugin
{

  var $i_steps  = 5;  // Number of setup steps 
  var $i_current= 1;  // Current step
  var $i_last   = 1;  // Last setup step;
  var $o_steps  = array(); 

  var $captured_values = array();

  function setup()
  {
    for($i = 1 ; $i <= $this->i_steps; $i ++ ){
      $class= "setup_step_".$i;
      $this->o_steps[$i] = new $class();
    }
  }

  function execute()
  {
    $smarty = get_smarty();
    $this->o_steps[$this->i_last]->set_active(FALSE);
    $this->o_steps[$this->i_current]->set_active();
    $content = $this->o_steps[$this->i_current]->execute();
    return($content);
  }


  /* Save posted attributes  */
  function save_object()
  {
    /* Call save_object for current setup step */
    $this->o_steps[$this->i_current] -> save_object();

    /* Get attributes from setup step */
    $tmp = $this->o_steps[$this->i_current]->get_attributes();
    foreach($tmp as $name => $value){
      $this->captured_values[$name] = $value;
    }

    /* check if current setup step is completed now 
        and activate the next step if possible */
    for($i = 1 ; $i <= $this->i_steps ; $i ++ ){
      if($this->o_steps[$i]->is_completed()){
        if(isset($this->o_steps[($i+1)])){
          $this->o_steps[($i+1)]->set_enabled();
        }
      }
    }

    /* Check if image button requests next page */
    foreach($_POST as $name => $value){
      if(preg_match("/^next_(x|y)/",$name)){
        $_POST['next'] = TRUE;
      }
      if(preg_match("/^last_(x|y)/",$name)){
        $_POST['last'] = TRUE;
      }
    }

    /* Check if step was selected */
    if(isset($_GET['step']) || isset($_POST['next']) || isset($_POST['last'])){

      if(isset($_GET['step'])){
        $step = $_GET['step'];
      }elseif(isset($_POST['next'])){
        $step = $this->i_current + 1;
      }elseif(isset($_POST['last'])){
        $step = $this->i_current - 1;
      }

      if($this->selectable_step($step)){
        $this->i_last    = $this->i_current;
        $this->i_current = $step;
      }
    }
  }


  /* Create navigation menu */
  function get_navigation_html()
  {
    $str = "";
    foreach($this->o_steps as $key => $step){

      $s_title    = $step -> get_title();
      $s_info     = $step -> get_small_info();
      $b_active   = $step -> is_active();
      $b_enabled  = $step -> is_enabled();

      $str .="<div >";
      if($b_enabled){
        if($b_active){
          $str .= "<a href='?step=".$key."' class='navigation_element_active'>";
          $str .= "<div class='navigation_title_active'>".$s_title."</div>";
          $str .= "<div class='navigation_info'>".$s_info."</div>";
          $str .= "</a><br>\n";
        }else{
          $str .= "<a href='?step=".$key."' class='navigation_element'>";
          $str .= "<div class='navigation_title_inactive'>".$s_title."</div>";
          $str .= "</a><br>\n";
        }
      }else{
        $str .= "<div class='navigation_element'>";
        $str .= "<div class='navigation_title_disabled'>".$s_title."</div>";
        $str .= "</div>";
      }
      $str .= "</div>" ;
    }
    return($str);
  }

  
  /* Create header entry */
  function get_header_html()
  {
    $str ="";
    $str.=" <div >";
    $str.="   <div>";
    $str.="     <font style='font-size:20px;float:top'>";
    $str.=   $this->o_steps[$this->i_current]->get_long_title();
    $str.="     </font>";
    $str.="   </div>";
    $str.="   <div style='text-align:right;float:top;'>";
    if(isset($this->o_steps[$this->i_current -1]) && $this->o_steps[$this->i_current -1]->is_enabled()){
      $str.="   <input class='center' type='image' name='last' src='images/setup_step_back.png'  title='"._("Last step")."'>";
    }else{
      $str.="   <img class='center' src='images/setup_step_back_gray.png' title='"._("Last step")."'>";
    }
    if(isset($this->o_steps[$this->i_current +1]) && $this->o_steps[$this->i_current +1]->is_enabled()){
      $str.="   <input class='center' type='image' name='next' src='images/setup_step_forward.png'  title='"._("Next step")."'>";
    }else{
      $str.="   <img class='center' src='images/setup_step_forward_gray.png'  title='"._("Next step")."'>";
    }
    $str.= "  </div>";
    $str.= "</div>";
    return ($str);
  }


  /* Check if the given step id is valid and selectable */
  function selectable_step($id)
  {
    if(isset($this->o_steps[$id]) && $this->o_steps[$id]->is_enabled()){
      return(true);
    }
    return(false);
  }
}




// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
