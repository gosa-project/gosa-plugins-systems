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


class Step_Language extends setup_step
{
  var $lang_selected  = "";       /* Language selected from select box  */
  var $lang           = "en_EN";  /* Detected language */
  var $languages      = array();
  var $attributes     = array("lang","lang_selected","timezone");
  var $header_image   = "images/locale.png";
  var $timezone       = "Europe/Berlin";
  var $timezones      = array();

  function Step_Language()
  {
    $this->lang           = get_browser_language();
    $this->lang_selected  = "";
    $this->update_strings(); 
    $this->timezones = get_timezones_array();
  }

  function update_strings()
  {
    $this->s_title      = _("Language setup");
    $this->s_title_long = _("Language setup");
    $this->s_info       = _("This step allows you to select your preferred language.");
  }
  
  function execute()
  {
    $this->languages = array(""      => _("Automatic"),
                             "de_DE" => _("German"),
                             "fr_FR" => _("French"),
                             "en_EN" => _("English"),
                             "zh_CN" => _("Chinese"),
                             "ru_RU" => _("Russian"));
    asort($this->languages);
    $smarty = get_smarty();
    $smarty->assign("languages",    $this->languages);
    $smarty->assign("lang",         $this->lang);
    $smarty->assign("lang_selected",$this->lang_selected);
    $smarty->assign("timezone"     ,$this->timezone);
    $smarty->assign("timezones"    ,$this->timezones);
    return($smarty->fetch(get_template_path("setup_language.tpl",TRUE,dirname(__FILE__))));
  }

  function save_object()
  {
    if((isset($_POST['lang_selected'])) && isset($this->languages[$_POST['lang_selected']])) {
      $this->lang_selected = $_POST['lang_selected'];
      $this->is_completed = TRUE;

      if($this->lang_selected != ""){
        $_SESSION['lang'] = $this->lang_selected;
      }else{
        $_SESSION['lang'] = $this->lang;
      }

      if(isset($_POST['timezone']) && isset($this->timezones[$_POST['timezone']])){
        $this->timezone = $_POST['timezone'];
      }
    }
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
