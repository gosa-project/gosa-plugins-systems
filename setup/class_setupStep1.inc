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


class setup_step_1 extends setup_step
{
  var $attributes = array("lang");
  var $lang       = "en_EN";
  var $languages  = array();

  function setup_step_1()
  {
    $this->lang = get_browser_language();

    $this->is_enabled = TRUE;
    $this->is_active  = TRUE;
    
    $this->s_title      = _("Language selection");
    $this->s_title_long = _("Please choose a language");
    $this->s_info       = _("This step is allows you to define your prefered language.");

    $this->languages = array("de_DE" => _("German"),
                             "fr_FR" => _("French"),
                             "en_EN" => _("English"),
                             "ru_RU" => _("Russian"));
  }
  
  function execute()
  {
    $smarty = get_smarty();
    $smarty->assign("languages", $this->languages);
    $smarty->assign("lang", $this->lang);
    return($smarty->fetch(get_template_path("setup_step1.tpl",TRUE,dirname(__FILE__))));
  }

  function save_object()
  {
    if((isset($_POST['lang'])) && isset($this->languages[$_POST['lang']])) {
      $this->lang = $_POST['lang'];
      $this->is_completed = TRUE;
      $_SESSION['lang'] = $this->lang;
    }
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
