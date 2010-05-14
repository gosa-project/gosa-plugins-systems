<?php 
class migrate_systemRDN extends migrateRDN implements propertyMigration
{
	protected $filter = "(migrate_systemRDN(objectClass=gosaAccount)((objectClass=sambaSamAccount))%s)";
}
?>
