<?php 
class migrate_serverRDN extends migrateRDN implements propertyMigration
{
	protected $filter = "(migrate_serverRDN(objectClass=gosaAccount)((objectClass=sambaSamAccount))%s)";
}
?>
