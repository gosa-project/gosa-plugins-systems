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


class Step_Feedback  extends setup_step
{
  var $languages      = array();
  var $attributes     = array();
  var $header_image   = "images/welcome.png";

  function Step_Feedback()
  {
    $this->is_enabled     = TRUE;
    $this->is_active      = TRUE;
    $this->update_strings(); 
  }

  function update_strings()
  {
    $this->s_title      = _("Feedback");
    $this->s_info       = _("In order to keep you up to date, you can simply check some of the options below.");
    $this->s_title_long = _("Notification and registration");
  }
  
  function execute()
  {
    $this->is_completed=TRUE;
    $smarty = get_smarty();
    $smarty->assign ("must", "<font class=\"must\">*</font>");
    $smarty->assign('auth_id', session_id());
    return($smarty->fetch(get_template_path("setup_feedback.tpl",TRUE,dirname(__FILE__))));
  }

  function save_object()
  {

  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
