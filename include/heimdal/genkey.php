<?php
/*
 * genkey.php,v 1.0 2006/08/25 21:00:00
 *
 * Copyright 2006 Alejandro Escanero Blanco <aescanero@chaosdimension.org>
 *
 * See the enclosed file COPYING for license information (GPL).  If you
 * did not receive this file, see http://www.fsf.org/copyleft/gpl.html.
 */

define(DES_CBC_CRC,1);
define(DES_CBC_MD4,2);
define(DES_CBC_MD5,3);
define(DES3_CBC_MD5,5);
define(DES3_CBC_SHA1,16);

include("deslib.php");
include("mkey.php");
include("asnencode.php");

class genkey{

	var $masterkey;
	var $desclass;
	var $intg_dkey;
	var $crypt_dkey;
	var $rnd_key;
	var $stringtokey;
	var $ekey;
	var $keytype;
	var $salt;
	var $sha1_hmac;
	var $okey;
	var $keystr;
	var $b64;

	function genkey($mkey){
		if($mkey->getKeyType_Entry(0)!=DES3_CBC_SHA1) die ("only spported DES3_CBC_SHA1 m-key");
		$this->masterkey=$mkey->getKey_Entry_A(0);
		/* printf("mkey: ");
			for ($i=0;$i<count($this->masterkey);$i++) printf("%02x",$this->masterkey[$i]);
			printf("\n");*/
		$this->desclass=new Des();
		$this->intg_dkey=$this->desclass->derive_key_integrity($this->masterkey);
		$this->crypt_dkey=$this->desclass->derive_key_encrypt($this->masterkey);
		$this->rnd_key=$this->desclass->DES_new_random_key(8);
		$this->ekey=$this->rnd_key;
	}

	function generate($keytype,$name,$realm,$pass){
		$this->keytype=$keytype;
		$this->salt=$realm.$name;
		switch($keytype){
			case DES_CBC_CRC:
			case DES_CBC_MD4:
			case DES_CBC_MD5:
				$this->desclass->DesStringToKey($pass,$realm,$name);
			break;
			case DES3_CBC_MD5:
				$this->desclass->Des3StringToKey($pass,$realm,$name);
			break;
			case DES3_CBC_SHA1:
				$this->desclass->Des3StringToKeyDerived($pass,$realm,$name);
			break;
			default:
			die ("keytype not supported, supported keys are: des-cbc-crc,des-cbc-md4,des-cbc-md5,des3-cbc-md5,des3-cbc-sha1");
		}
		$this->stringtokey=$this->desclass->out;
		for ($i=0;$i<count($this->stringtokey);$i++){
			$this->ekey[8+$i]=$this->stringtokey[$i];
		}
		$hash_key="";
		for ($i=0;$i<count($this->rnd_key);$i++){
			$hash_key.=chr($this->rnd_key[$i]);
		}
		for ($i=0;$i<count($this->stringtokey);$i++){
			$hash_key.=chr($this->stringtokey[$i]);
		}
		
		$data="";
		for ($i=0;$i<count($this->intg_dkey);$i++){
			$data.=chr($this->intg_dkey[$i]);
		}
		$sha1_hmac_raw=$this->hmacsha1($data,$hash_key);
		$this->sha1hmac=array();
		for ($i=0;$i<strlen($sha1_hmac_raw);$i++){
			$this->sha1hmac[$i]=ord(substr($sha1_hmac_raw,$i,1));
		}

		//MUST CHANGE for derive_key_encrypt

		$keys=array(3);
		for($i = 0;$i < 3; $i++){
			$keys[$i]=array(8);
			for($j = 0;$j < 8; $j++) $keys[$i][$j]=$this->crypt_dkey[($i*8)+$j];
		}
		$ks1=$this->desclass->des_make_key_sched($keys[0]);
		$ks2=$this->desclass->des_make_key_sched($keys[1]);
		$ks3=$this->desclass->des_make_key_sched($keys[2]);

		$this->okey=$this->desclass->DES_ede3_cbc_encrypt($this->ekey,true,$ks1,$ks2,$ks3,16);
		for ($i=0;$i<count($this->sha1hmac);$i++) $this->okey[]=$this->sha1hmac[$i];
		$this->keystr="";
		for($i=0;$i<count($this->okey);$i++) $this->keystr.=chr($this->okey[$i]);
		return($this->keystr);
/*		$stringh="";
		for($i=0;$i<count($oekey);$i++) $stringh.=sprintf("%02x",$oekey[$i]);
		print "OUT EKEY: ".$stringh."\n";*/
	}

	function encode(){
		$asn_int_1=new asnEncode();
		$asn_int_1->encodeInteger($this->keytype);//tipo codificacion?
		$asn_int_2=new asnEncode();
		$asn_int_2->encodeInteger(3);
		$asn_int_3=new asnEncode();
		$asn_int_3->encodeInteger(3);
		$asn_int_4=new asnEncode();
		$asn_int_4->encodeInteger(1);

		$asn_salt=new asnEncode();
		$asn_salt->encodeOctetString($this->salt);
		$asn_key=new asnEncode();
		$asn_key->encodeOctetString($this->keystr);

		$asn_salt_seq=new asnEncode();
		$asn_salt_seq->encodeSequence(0,$asn_int_2->getStream());
		$asn_salt_seq->encodeSequence(1,$asn_salt->getStream());

		$asn_salt_obj=new asnEncode();
		$asn_salt_obj->encodeObject(0x30,$asn_salt_seq->getStream());

		$asn_seq3=new asnEncode();
		$asn_seq3->encodeSequence(0,$asn_int_1->getStream());
		$asn_seq3->encodeSequence(1,$asn_key->getStream());
		$asn_seq3->encodeSequence(2,$asn_salt_obj->getStream());

		$asn_key_obj=new asnEncode();
		$asn_key_obj->encodeObject(0x30,$asn_seq3->getStream());

		$asn_seq4=new asnEncode();
		$asn_seq4->encodeSequence(0,$asn_int_4->getStream());
		$asn_seq4->encodeSequence(1,$asn_key_obj->getStream());

		$asn_obj=new asnEncode();
		$asn_obj->encodeObject(0x30,$asn_seq4->getStream());
		$this->b64=base64_encode($asn_obj->printString());
	}

	function printKey(){
		printf("key(base64):\n".$this->b64."\n");
	}

	function hmacsha1($key,$data){
		$blocksize=64;
		$hashfunc='sha1';
		if (strlen($key)>$blocksize)
			$key=pack('H*', $hashfunc($key));
		$key=str_pad($key,$blocksize,chr(0x00));
		$ipad=str_repeat(chr(0x36),$blocksize);
		$opad=str_repeat(chr(0x5c),$blocksize);
		$hmac = pack(
			'H*',$hashfunc(
				($key^$opad).pack(
					'H*',$hashfunc(
						($key^$ipad).$data
					)
				)
			)
		);
		return($hmac);
		//return bin2hex($hmac);
	}

};
?>