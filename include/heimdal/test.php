<?php
/*
 * testgenkey.php,v 1.0 2006/08/25 21:00:00
 *
 * Copyright 2006 Alejandro Escanero Blanco <aescanero@chaosdimension.org>
 *
 * See the enclosed file COPYING for license information (GPL).  If you
 * did not receive this file, see http://www.fsf.org/copyleft/gpl.html.
 */


include("genkey.php");


define(MKEYFILE,"/var/lib/heimdal-kdc/m-key");
define(NAME,"alex");
define(REALM,"CHAOSDIMENSION.ORG");
define(PASSWORD,"prueba");


$mkey = new mkey(MKEYFILE);
$genkey= new genkey($mkey);
$genkey->generate(DES_CBC_MD5,NAME,REALM,PASSWORD);
$genkey->encode();
$genkey->printKey();

?>