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


class Step_Migrate extends setup_step
{
  var $languages      = array();
  var $attributes     = array();
  var $header_image   = "images/locale.png";

  function Step_Migrate()
  {
    $this->update_strings(); 
  }

  function update_strings()
  {
    $this->s_title      = _("Ldap migration assistent");
    $this->s_title_long = _("Ldap migration assistent");
    $this->s_info       = _("This step allows you to migrate your ldap database");
  }
  
  function execute()
  {
    $smarty = get_smarty();
    return($smarty->fetch(get_template_path("setup_migrate.tpl",TRUE,dirname(__FILE__))));
  }

  function save_object()
  {
    $this->is_completed = TRUE;
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
