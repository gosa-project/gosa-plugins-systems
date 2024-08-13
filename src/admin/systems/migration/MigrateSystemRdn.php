<?php

namespace GosaSystems\admin\systems\migration;

use \migrateRDN as migrateRDN;
use \propertyMigration as propertyMigration;

class MigrateSystemRdn extends migrateRDN implements propertyMigration
{
    protected $filter = "(&(objectClass=nothing)%s)";
}
