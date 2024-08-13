<?php

namespace GosaSystems\admin\systems\migration;

use \migrateRDN as migrateRDN;
use \propertyMigration as propertyMigration;

class MigrateComponentRdn extends migrateRDN implements propertyMigration
{
	protected $filter = "(&(objectClass=ieee802Device)%s)";
}
