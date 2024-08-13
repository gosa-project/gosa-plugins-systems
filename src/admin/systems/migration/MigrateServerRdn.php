<?php

namespace GosaSystems\admin\systems\migration;

use \migrateRDN as migrateRDN;
use \propertyMigration as propertyMigration;

class MigrateServerRdn extends migrateRDN implements propertyMigration
{
	protected $filter = "(&(objectClass=goServer)%s)";
}
