<?php

class acl extends plugin
{

  /* constructor, if 'dn' is set, the node loads the given
     'dn' from LDAP */
  function acl ($config, $dn= NULL)
  {
    $this->config= $config;

    /* Load base attributes */
    plugin::plugin ($config, $dn);

  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
