<?php
class password
{
  /* Definitions */
  var $plHeadline= "Password";
  var $plDescription= "This does something";

  
  function plInfo()
  {
    return (array(
          "plDescription"     => _("User password"),
          "plSelfModify"      => TRUE,
          "plDepends"         => array("user"),
          "plPriority"        => 10,
          "plSection"         => "personal",
          "plCategory"        => array("users"),
          "plOptions"         => array(),

          "plProvidedAcls"  => array())
            );
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
