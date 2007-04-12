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

class setup 
{
  var $i_steps  = 9;  // Number of setup steps 
  var $i_current= 1;  // Current step
  var $i_last   = 1;  // Last setup step;
  var $o_steps  = array(); 
  var $captured_values = array();

  function setup()
  {
    
    $this->o_steps[1] = new setup_step_1();
    $this->o_steps[2] = new setup_step_2();
    $this->o_steps[3] = new setup_step_3();
    $this->o_steps[4] = new setup_step_4();
    $this->o_steps[5] = new setup_step_5();
    $this->o_steps[6] = new setup_step_6();
    $this->o_steps[7] = new setup_step_6a();
    $this->o_steps[8] = new setup_step_7();
    $this->o_steps[9] = new setup_step_8();

    /* Ensure that setup is not reachable if gosa.conf (CONFIG_FILE) */
    if(file_exists(CONFIG_DIR."/".CONFIG_FILE)){
      session_destroy();
      header("Location: index.php")    ;
      exit();
    }
    
    foreach($this->o_steps as $key => $step){
      $this->o_steps[$key]->parent = $this;
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

    /* Set parent */
    foreach($this->o_steps as $key => $value){
      $this->o_steps[$key]->parent = $this;
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
    if(isset($_GET['step']) || isset($_POST['next']) || isset($_POST['last']) || isset($_POST['setup_goto_step'])){

      /* check if current setup step is completed now 
          and activate the next step if possible */
      for($i = 1 ; $i <= $this->i_steps ; $i ++ ){
        if($this->o_steps[$i]->is_completed()){
          if(isset($this->o_steps[($i+1)])){
            $this->o_steps[($i+1)]->set_enabled();
          }
        }else{
          $this->disable_steps_from($i+1);
        }
      }
    }
  
    $step = -1;

    if(isset($_POST['setup_goto_step'])){
      $step= $_POST['setup_goto_step'];
    }

    if(isset($_GET['step'])){
      $step = $_GET['step'];
    }elseif(isset($_POST['next'])){
      $step = $this->i_current + 1;
    }elseif(isset($_POST['last'])){
      $step = $this->i_current - 1;
    }
  
    $once = true;
    foreach($_POST as $name => $value){
      if(preg_match("/^step_[0-9]*$/",$name) && $once ){
        $step = preg_replace("/^step_/","",$name);
      }
    }

    if($this->selectable_step($step)){
      $this->i_last    = $this->i_current;
      $this->i_current = $step;
    }
  }


  function disable_steps_from($start)
  {
    $found = false;
    foreach($this->o_steps as $key => $step){
      if($key == $start){
        $found = true;
      }

      if($found){ 
        $this->o_steps[$key]->set_enabled(false);
        $this->o_steps[$key]->set_completed(false);
      }
    }
  }


  /* Create navigation menu */
  function get_navigation_html()
  {
    $str = "";
    foreach($this->o_steps as $key => $step){

      $step -> update_strings();

      $s_title    = $step -> get_title();
      $s_info     = $step -> get_small_info();
      $b_active   = $step -> is_active();
      $b_enabled  = $step -> is_enabled();

      if($_SESSION['js']){

        $str .="<div >";
        if($b_enabled){
          if($b_active){
            $str .= "<div class='navigation_element_active'>";
            $str .= "<div onClick='document.mainform.setup_goto_step.value=\"$key\";document.mainform.submit();'
              class='navigation_title_active'>".$s_title."</div>";
            $str .= "<div onClick='document.mainform.setup_goto_step.value=\"$key\";document.mainform.submit();'
              class='navigation_info'>".$s_info."</div>";
            $str .= "</div>";
          }else{
            $str .= "<div class='navigation_element'>";
            $str .= "<div onClick='document.mainform.setup_goto_step.value=\"$key\";document.mainform.submit();'
              class='navigation_title_inactive'>".$s_title."</div>";
            $str .= "</div>";
          }
        }else{
          $str .= "<div class='navigation_element'>";
          $str .= "<div class='navigation_title_disabled'>".$s_title."</div>";
          $str .= "</div>";
        }
        $str .= "</div>" ;
      }else{
        $str .="<div >";
        if($b_enabled){
          if($b_active){
            $str .= "<div class='navigation_element_active'>";
            $str .= "<input style='text-align: left; color: #00008F; font-weight: bold; width:100%;' 
                        type='submit' value='".$s_title."' name='step_".$key."'>";
            $str .= "</div>";
          }else{
            $str .= "<div class='navigation_element'>";
            $str .= "<input style='text-align: left; color: #00008F; font-weight: bold; width:100%;' 
                        type='submit' value='".$s_title."' name='step_".$key."'>";
            $str .= "</div>";
          }
        }else{
          $str .= "<div class='navigation_element'>";
          $str .= "<div class='navigation_title_disabled'>".$s_title."</div>";
          $str .= "</div>";
        }
        $str .= "</div>" ;
      }
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
#   if(isset($this->o_steps[$this->i_current +1]) && $this->o_steps[$this->i_current +1]->is_enabled()){
      $str.="   <input class='center' type='image' name='next' src='images/setup_step_forward.png'  title='"._("Next step")."'>";
#   }else{
#     $str.="   <img class='center' src='images/setup_step_forward_gray.png'  title='"._("Next step")."'>";
#   }
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
