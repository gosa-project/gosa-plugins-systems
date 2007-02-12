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


class setup_step_7 extends setup_step
{
  var $attributes = array();

  function setup_step_7()
  {
    $this->s_title      = _("GOsa settings 1/2");
    $this->s_title_long = _("GOsa generic settings, page 1/2");
    $this->s_info       = _("This dialog allows you to setup GOsa behaviour");
  }

  
  function execute()
  {
    $smarty = get_smarty();
    return($smarty -> fetch (get_template_path("../setup/setup_step7.tpl")));
  }

  function save_object()
  {
    if(isset($_POST['step7_posted'])){

      /* Get attributes */
      foreach($this->attributes as $attr){
        if(isset($_POST[$attr])){
          $this->$attr = validate($_POST[$attr]);
        }
      }
    }
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
