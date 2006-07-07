<?php
/*
  This code is part of GOsa (https://gosa.gonicus.de)
  Copyright (C) 2005  Cajus Pollmeier

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

if (!$remove_lock){
	/* Create message object on demand */
	if (!isset($_SESSION['aclplug']) || (isset($_GET['reset']) && $_GET['reset'] == 1)){
		$_SESSION['aclplug']= new acl ($config, 'ou=Direktorium,o=Landeshauptstadt München,c=de');
	}
	$aclplug= $_SESSION['aclplug'];

	/* Set permissions */
	$acl= get_permissions ($ui->dn, $ui->subtreeACL);
	$aclplug->acl= get_module_permission($acl, "acl", $ui->dn);

	/* save changes back to object */
	if (isset ($_POST['target'])){
	  $aclplug->save_object ();
	}

	/* Execute formular */
	if ($aclplug->aclPostHandler()){
		echo "Saved... But this is not the way it goes normally!";
	}
	$display= $aclplug->execute();
	$display.= "<input type=\"hidden\" name=\"ignore\">\n";

	/* Page header*/
	$display= print_header(get_template_path('images/acl.png'), _("Access control")).$display;

	/* Store changes  in session */
	$_SESSION['aclplug']= $aclplug;
}
?>
