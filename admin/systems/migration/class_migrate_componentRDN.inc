<?php 
class migrate_componentRDN extends migrateRDN implements propertyMigration
{
	protected $filter = "(migrate_componentRDN(objectClass=gosaAccount)((objectClass=sambaSamAccount))%s)";
}
?>
