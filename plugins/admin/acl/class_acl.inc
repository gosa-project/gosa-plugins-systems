<?php

class acl extends plugin
{
  /* Definitions */
  var $plHeadline= "Access control";
  var $plDescription= "This does something";

  /* attribute list for save action */
  var $attributes= array("target", "nmessage");
  var $objectclasses= array();


  function acl ($config, $dn= NULL)
  {
    /* Include config object */
    $this->config= $config;
  }


  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    $smarty= get_smarty();

    /* Show main page */
    return ($smarty->fetch (get_template_path('contents.tpl', TRUE)));
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
