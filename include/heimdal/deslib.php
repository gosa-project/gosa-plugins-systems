<?php
/*
 * deslib.php,v 1.0 2006/08/25 21:00:00
 *
 * Copyright 2006 Alejandro Escanero Blanco <aescanero@chaosdimension.org>
 *
 * See the enclosed file COPYING for license information (GPL).  If you
 * did not receive this file, see http://www.fsf.org/copyleft/gpl.html.
 */


define(DEBUG,false);
define(HDB_KU_MKEY,0x484442);

class Des{

	var $in;
	var $out;
	var $key;
	var $schedule1;
	var $schedule2;
	var $schedule3;

	function add1($a,$b,$len){
		$carry=0;
		for ($i=$len-1;$i>=0;$i--){
			$x=$a[$i]+$b[$i]+$carry;
			if($x > 0xff) $carry=1;else $carry=0;
			$a[$i] = $x & 0xff;
		}
		for($i = $len - 1; $carry!=0 && $i!=0; $i--){
			$x = $a[$i] + $carry;
			if($x > 0xff) $carry=1;else $carry=0;
			$a[$i] = $x & 0xff;
		}
		return ($a);
	}
	
	function _krb5_n_fold($str,$size){ //str=passREALMname
		$len=strlen($str);
		if($size>$len) $maxlen=2*$size;
		else $maxlen=2*$len;
		$l=0;
		$key=array($size);
		$tmp=array($maxlen);
		$tmpi=0;
		for ($i=0;$i<$maxlen;$i++) $tmp[$i]=0;
		for ($i=0;$i<$size;$i++) $key[$i]=0;
		$buf=$str;
		do{
			for ($i=0; $i<$len;$i++) $tmp[$tmpi+$l+$i]=ord(substr($buf,$i,1));
			$l+=$len;
			$buf=$this->srr13($buf,strlen($buf)*8);
			while($l>=$size){
				$key=$this->add1($key,$tmp,$size);
				$l-=$size;
				if($l==0) break;
				for ($i=0;$i<$l;$i++) $tmp[$i]=$tmp[$i+$size];
			}
		}while ($l!=0);
/*		$stringh="";
		for($i=0;$i<$size;$i++) $stringh.=sprintf("%02x",$key[$i]);
		print "EKEY: ".$stringh."\n";*/
		$skey="";
		for($i=0;$i<$size;$i++) $skey.=chr($key[$i]);
		return($skey);
	}
	
	function srr13($sbuf,$len){
		
		$buf=array(strlen($sbuf));
		for($i=0;$i<strlen($sbuf);$i++) $buf[$i]=ord(substr($sbuf,$i,1));
		$bytes = floor(($len + 7) / 8);
		$tmp=array($bytes);
		for($i=0;$i<$bytes;$i++) $tmp[$i]=0;
		
		$bits = 13 % $len;
		$lbit = $len % 8;
		for($i=0;$i<$bytes;$i++) $tmp[$i]=$buf[$i];
		if($lbit!=0){
			$tmp[$bytes - 1] &= (0xff << (8 - $lbit));
			for($i = $lbit; $i < 8; $i += $len)
			$tmp[$bytes - 1] |= $buf[0] >> $i;
		}
		for($i = 0; $i < $bytes; $i++) {
		$bb = 8 * $i - $bits;
		while($bb < 0)
			$bb += $len;
		$b1 = floor($bb / 8);
		$s1 = $bb % 8;
		
		if($bb + 8 > $bytes * 8) 
			$s2 = ($len + 8 - $s1) % 8;
		else 
			$s2 = 8 - $s1;
		$b2 = ($b1 + 1) % $bytes;
		$buf[$i] = (($tmp[$b1]) << $s1) | (($tmp[$b2]) >> $s2);
		}
		$slen=strlen($sbuf);
		$sbuf="";
		for($i=0;$i<$slen;$i++) $sbuf.=chr($buf[$i]);
		return($sbuf);
	
	}

	function load($achar,$aint){		//Array=char(8), Array=int32(2)
		$aint[0] =  $achar[0] << 24;
		$aint[0] |= $achar[1] << 16;
		$aint[0] |= $achar[2] << 8;
		$aint[0] |= $achar[3] << 0;
		$aint[1] =  $achar[4] << 24;
		$aint[1] |= $achar[5] << 16;
		$aint[1] |= $achar[6] << 8;
		$aint[1] |= $achar[7] << 0;
	}

	function store($aint,$achar){		//Array=int32(2),Array=char(8)
		$achar[0] = ($aint[0] >> 24) & 0xff;
		$achar[1] = ($aint[0] >> 16) & 0xff;
		$achar[2] = ($aint[0] >>  8) & 0xff;
		$achar[3] = ($aint[0] >>  0) & 0xff;
		$achar[4] = ($aint[1] >> 24) & 0xff;
		$achar[5] = ($aint[1] >> 16) & 0xff;
		$achar[6] = ($aint[1] >>  8) & 0xff;
		$achar[7] = ($aint[1] >>  0) & 0xff;
	}

	function IP($v){		//v=Array int(2)
		$work=0;
		$work = (($v[0] >> 4) ^ $v[1]) & 0x0f0f0f0f;
		$v[1] ^= $work;
		$v[0] ^= ($work << 4);
		$work = (($v[0] >> 16) ^ $v[1]) & 0x0000ffff;
		$v[1] ^= $work;
		$v[0] ^= ($work << 16);
		$work = (($v[1] >> 2) ^ $v[0]) & 0x33333333;
		$v[0] ^= $work;
		$v[1] ^= ($work << 2);
		$work = (($v[1] >> 8) ^ $v[0]) & 0x00ff00ff;
		$v[0] ^= $work;
		$v[1] ^= ($work << 8);
		$v[1] = (($v[1] << 1) | (($v[1] >> 31) & 1)) & 0xffffffff;
		$work = ($v[0] ^ $v[1]) & 0xaaaaaaaa;
		$v[0] ^= $work;
		$v[1] ^= $work;
		$v[0] = (($v[0] << 1) | (($v[0] >> 31) & 1)) & 0xffffffff;
	}

	function FP($v){		//v=Array int(2)

		$work=0;
		
		$v[0] = ($v[0] << 31) | (($v[0] >> 1) & 0x7fffffff);
		$work = ($v[1] ^ $v[0]) & 0xaaaaaaaa;
		$v[1] ^= $work;
		$v[0] ^= $work;
		$v[1] = ($v[1] << 31) | (($v[1] >> 1) & 0x7fffffff);
		$work = (($v[1] >> 8) ^ $v[0]) & 0x00ff00ff;
		$v[0] ^= $work;
		$v[1] ^= ($work << 8);
		$work = (($v[1] >> 2) ^ $v[0]) & 0x33333333;
		$v[0] ^= $work;
		$v[1] ^= ($work << 2);
		$work = (($v[0] >> 16) ^ $v[1]) & 0x0000ffff;
		$v[1] ^= $work;
		$v[0] ^= ($work << 16);
		$work = (($v[0] >> 4) ^ $v[1]) & 0x0f0f0f0f;
		$v[1] ^= $work;
		$v[0] ^= ($work << 4);
	}

	function desx($aint,$ks,$comp){
		$SP = array(
			array(
				0x01010400, 0x00000000, 0x00010000, 0x01010404,
				0x01010004, 0x00010404, 0x00000004, 0x00010000,
				0x00000400, 0x01010400, 0x01010404, 0x00000400,
				0x01000404, 0x01010004, 0x01000000, 0x00000004,
				0x00000404, 0x01000400, 0x01000400, 0x00010400,
				0x00010400, 0x01010000, 0x01010000, 0x01000404,
				0x00010004, 0x01000004, 0x01000004, 0x00010004,
				0x00000000, 0x00000404, 0x00010404, 0x01000000,
				0x00010000, 0x01010404, 0x00000004, 0x01010000,
				0x01010400, 0x01000000, 0x01000000, 0x00000400,
				0x01010004, 0x00010000, 0x00010400, 0x01000004,
				0x00000400, 0x00000004, 0x01000404, 0x00010404,
				0x01010404, 0x00010004, 0x01010000, 0x01000404,
				0x01000004, 0x00000404, 0x00010404, 0x01010400,
				0x00000404, 0x01000400, 0x01000400, 0x00000000,
				0x00010004, 0x00010400, 0x00000000, 0x01010004
    			),
			array(
				0x80108020, 0x80008000, 0x00008000, 0x00108020,
				0x00100000, 0x00000020, 0x80100020, 0x80008020,
				0x80000020, 0x80108020, 0x80108000, 0x80000000,
				0x80008000, 0x00100000, 0x00000020, 0x80100020,
				0x00108000, 0x00100020, 0x80008020, 0x00000000,
				0x80000000, 0x00008000, 0x00108020, 0x80100000,
				0x00100020, 0x80000020, 0x00000000, 0x00108000,
				0x00008020, 0x80108000, 0x80100000, 0x00008020,
				0x00000000, 0x00108020, 0x80100020, 0x00100000,
				0x80008020, 0x80100000, 0x80108000, 0x00008000,
				0x80100000, 0x80008000, 0x00000020, 0x80108020,
				0x00108020, 0x00000020, 0x00008000, 0x80000000,
				0x00008020, 0x80108000, 0x00100000, 0x80000020,
				0x00100020, 0x80008020, 0x80000020, 0x00100020,
				0x00108000, 0x00000000, 0x80008000, 0x00008020,
				0x80000000, 0x80100020, 0x80108020, 0x00108000
			),
			array(
				0x00000208, 0x08020200, 0x00000000, 0x08020008,
				0x08000200, 0x00000000, 0x00020208, 0x08000200,
				0x00020008, 0x08000008, 0x08000008, 0x00020000,
				0x08020208, 0x00020008, 0x08020000, 0x00000208,
				0x08000000, 0x00000008, 0x08020200, 0x00000200,
				0x00020200, 0x08020000, 0x08020008, 0x00020208,
				0x08000208, 0x00020200, 0x00020000, 0x08000208,
				0x00000008, 0x08020208, 0x00000200, 0x08000000,
				0x08020200, 0x08000000, 0x00020008, 0x00000208,
				0x00020000, 0x08020200, 0x08000200, 0x00000000,
				0x00000200, 0x00020008, 0x08020208, 0x08000200,
				0x08000008, 0x00000200, 0x00000000, 0x08020008,
				0x08000208, 0x00020000, 0x08000000, 0x08020208,
				0x00000008, 0x00020208, 0x00020200, 0x08000008,
				0x08020000, 0x08000208, 0x00000208, 0x08020000,
				0x00020208, 0x00000008, 0x08020008, 0x00020200
			),
			array(
				0x00802001, 0x00002081, 0x00002081, 0x00000080,
				0x00802080, 0x00800081, 0x00800001, 0x00002001,
				0x00000000, 0x00802000, 0x00802000, 0x00802081,
				0x00000081, 0x00000000, 0x00800080, 0x00800001,
				0x00000001, 0x00002000, 0x00800000, 0x00802001,
				0x00000080, 0x00800000, 0x00002001, 0x00002080,
				0x00800081, 0x00000001, 0x00002080, 0x00800080,
				0x00002000, 0x00802080, 0x00802081, 0x00000081,
				0x00800080, 0x00800001, 0x00802000, 0x00802081,
				0x00000081, 0x00000000, 0x00000000, 0x00802000,
				0x00002080, 0x00800080, 0x00800081, 0x00000001,
				0x00802001, 0x00002081, 0x00002081, 0x00000080,
				0x00802081, 0x00000081, 0x00000001, 0x00002000,
				0x00800001, 0x00002001, 0x00802080, 0x00800081,
				0x00002001, 0x00002080, 0x00800000, 0x00802001,
				0x00000080, 0x00800000, 0x00002000, 0x00802080
			),
			array(
				0x00000100, 0x02080100, 0x02080000, 0x42000100,
				0x00080000, 0x00000100, 0x40000000, 0x02080000,
				0x40080100, 0x00080000, 0x02000100, 0x40080100,
				0x42000100, 0x42080000, 0x00080100, 0x40000000,
				0x02000000, 0x40080000, 0x40080000, 0x00000000,
				0x40000100, 0x42080100, 0x42080100, 0x02000100,
				0x42080000, 0x40000100, 0x00000000, 0x42000000,
				0x02080100, 0x02000000, 0x42000000, 0x00080100,
				0x00080000, 0x42000100, 0x00000100, 0x02000000,
				0x40000000, 0x02080000, 0x42000100, 0x40080100,
				0x02000100, 0x40000000, 0x42080000, 0x02080100,
				0x40080100, 0x00000100, 0x02000000, 0x42080000,
				0x42080100, 0x00080100, 0x42000000, 0x42080100,
				0x02080000, 0x00000000, 0x40080000, 0x42000000,
				0x00080100, 0x02000100, 0x40000100, 0x00080000,
				0x00000000, 0x40080000, 0x02080100, 0x40000100
			),
			array(
				0x20000010, 0x20400000, 0x00004000, 0x20404010,
				0x20400000, 0x00000010, 0x20404010, 0x00400000,
				0x20004000, 0x00404010, 0x00400000, 0x20000010,
				0x00400010, 0x20004000, 0x20000000, 0x00004010,
				0x00000000, 0x00400010, 0x20004010, 0x00004000,
				0x00404000, 0x20004010, 0x00000010, 0x20400010,
				0x20400010, 0x00000000, 0x00404010, 0x20404000,
				0x00004010, 0x00404000, 0x20404000, 0x20000000,
				0x20004000, 0x00000010, 0x20400010, 0x00404000,
				0x20404010, 0x00400000, 0x00004010, 0x20000010,
				0x00400000, 0x20004000, 0x20000000, 0x00004010,
				0x20000010, 0x20404010, 0x00404000, 0x20400000,
				0x00404010, 0x20404000, 0x00000000, 0x20400010,
				0x00000010, 0x00004000, 0x20400000, 0x00404010,
				0x00004000, 0x00400010, 0x20004010, 0x00000000,
				0x20404000, 0x20000000, 0x00400010, 0x20004010
			),
			array(
				0x00200000, 0x04200002, 0x04000802, 0x00000000,
				0x00000800, 0x04000802, 0x00200802, 0x04200800,
				0x04200802, 0x00200000, 0x00000000, 0x04000002,
				0x00000002, 0x04000000, 0x04200002, 0x00000802,
				0x04000800, 0x00200802, 0x00200002, 0x04000800,
				0x04000002, 0x04200000, 0x04200800, 0x00200002,
				0x04200000, 0x00000800, 0x00000802, 0x04200802,
				0x00200800, 0x00000002, 0x04000000, 0x00200800,
				0x04000000, 0x00200800, 0x00200000, 0x04000802,
				0x04000802, 0x04200002, 0x04200002, 0x00000002,
				0x00200002, 0x04000000, 0x04000800, 0x00200000,
				0x04200800, 0x00000802, 0x00200802, 0x04200800,
				0x00000802, 0x04000002, 0x04200802, 0x04200000,
				0x00200800, 0x00000000, 0x00000002, 0x04200802,
				0x00000000, 0x00200802, 0x04200000, 0x00000800,
				0x04000002, 0x04000800, 0x00000800, 0x00200002
			),
			array(
				0x10001040, 0x00001000, 0x00040000, 0x10041040,
				0x10000000, 0x10001040, 0x00000040, 0x10000000,
				0x00040040, 0x10040000, 0x10041040, 0x00041000,
				0x10041000, 0x00041040, 0x00001000, 0x00000040,
				0x10040000, 0x10000040, 0x10001000, 0x00001040,
				0x00041000, 0x00040040, 0x10040040, 0x10041000,
				0x00001040, 0x00000000, 0x00000000, 0x10040040,
				0x10000040, 0x10001000, 0x00041040, 0x00040000,
				0x00041040, 0x00040000, 0x10041000, 0x00001000,
				0x00000040, 0x10040040, 0x00001000, 0x00041040,
				0x10001000, 0x00000040, 0x10000040, 0x10040000,
				0x10040040, 0x10000000, 0x00040000, 0x10001040,
				0x00000000, 0x10041040, 0x00040040, 0x10000040,
				0x10040000, 0x10001000, 0x10001040, 0x00000000,
				0x10041040, 0x00041000, 0x00041000, 0x00001040,
				0x00001040, 0x00040040, 0x10000000, 0x10041000
			)
		);

		$left = $aint[0];
		$right = $aint[1];
		$fval=0;
		$work=0;

		if ($comp) {
			$keyix=0;
			$keys = $ks[$keyix];

			for( $i = 0; $i < 8; $i++ ) {
				$work  = ($right << 28) | (($right >> 4) & 0x0fffffff);
				$work ^= $ks[$keyix];$keyix++;

				$fval  = $SP[6][ $work     & 0x3f];
				$fval |= $SP[4][($work >>  8) & 0x3f];
				$fval |= $SP[2][($work >> 16) & 0x3f];
				$fval |= $SP[0][($work >> 24) & 0x3f];

				$work  = $right ^ $ks[$keyix];$keyix++;
				$fval |= $SP[7][ $work     & 0x3f];
				$fval |= $SP[5][($work >>  8) & 0x3f];
				$fval |= $SP[3][($work >> 16) & 0x3f];
				$fval |= $SP[1][($work >> 24) & 0x3f];

				$left ^= $fval;
				$work  = ($left << 28) | (($left >> 4) & 0x0fffffff);
				$work ^= $ks[$keyix];$keyix++;
				$fval  = $SP[6][ $work     & 0x3f];
				$fval |= $SP[4][($work >>  8) & 0x3f];
				$fval |= $SP[2][($work >> 16) & 0x3f];
				$fval |= $SP[0][($work >> 24) & 0x3f];
				$work  = $left ^ $ks[$keyix];$keyix++;
				$fval |= $SP[7][ $work     & 0x3f];
				$fval |= $SP[5][($work >>  8) & 0x3f];
				$fval |= $SP[3][($work >> 16) & 0x3f];
				$fval |= $SP[1][($work >> 24) & 0x3f];
				$right ^= $fval;
			}
		} else {
			$keyix=30;
			$keys = $ks[$keyix];

			for( $i = 0; $i < 8; $i++ ) {
				$work  = ($right << 28) | (($right >> 4) & 0x0fffffff);
				$work ^= $ks[$keyix];$keyix++;
				$fval  = $SP[6][ $work     & 0x3f];
				$fval |= $SP[4][($work >>  8) & 0x3f];
				$fval |= $SP[2][($work >> 16) & 0x3f];
				$fval |= $SP[0][($work >> 24) & 0x3f];
				$work  = $right ^ $ks[$keyix];$keyix++;
				$fval |= $SP[7][ $work     & 0x3f];
				$fval |= $SP[5][($work >>  8) & 0x3f];
				$fval |= $SP[3][($work >> 16) & 0x3f];
				$fval |= $SP[1][($work >> 24) & 0x3f];
				$left ^= $fval;
				$work  = ($left << 28) | (($left >> 4) & 0x0fffffff);
				$keyix -= 4;
				$work ^= $ks[$keyix];$keyix++;
				$fval  = $SP[6][ $work     & 0x3f];
				$fval |= $SP[4][($work >>  8) & 0x3f];
				$fval |= $SP[2][($work >> 16) & 0x3f];
				$fval |= $SP[0][($work >> 24) & 0x3f];
				$work  = $left ^ $ks[$keyix];$keyix++;
				$fval |= $SP[7][ $work     & 0x3f];
				$fval |= $SP[5][($work >>  8) & 0x3f];
				$fval |= $SP[3][($work >> 16) & 0x3f];
				$fval |= $SP[1][($work >> 24) & 0x3f];
				$right ^= $fval;
				$keyix -= 4;
			}
		}
		$aint[0] = $right;
		$aint[1] = $left;
	}


	function _des3_encrypt($aint,$ks1,$ks2,$ks3,$comp){
		$this->IP(&$aint);
		if($comp){
			$this->desx(&$aint,&$ks1,true);
			$this->desx(&$aint,&$ks2,false);
			$this->desx(&$aint,&$ks3,true);
		}else{
			$this->desx(&$aint,&$ks1,false);
			$this->desx(&$aint,&$ks2,true);
			$this->desx(&$aint,&$ks3,false);
		}
		$this->FP(&$aint);
	}

	function DES_ede3_cbc_encrypt($chain,$comp,$ks1,$ks2,$ks3,$ixlen){

		$n_fold_key=$ch_input;
		$size=$ixlen;
		
		$ivec=array(8);
		for ($i=0;$i<8;$i++) $ivec[$i]=chr(0);
		$uiv=array(2);$uiv[0]=0;$uiv[1]=0;
		$this->load($ivec,&$uiv);
//		$stringh="";
//				for($i=0;$i<$ixlen;$i++) $stringh.=sprintf("%02x",$chain[$i]);
//				print "PREINPUT:  $ixlen ".$stringh."\n";

		$ix=0;
		$input=array(8); for($i=0;$i<8;$i++) $input[$i]=$chain[$i+$ix];
		
		$output=&$input;
		$u=array(2);$u[0]=0;$u[1]=0;
		if($comp){
			while($ixlen>=8){
				$this->load($input,&$u);
				$u[0] ^=$uiv[0];
				$u[1] ^=$uiv[1];
				$this->_des3_encrypt(&$u,$ks1,$ks2,$ks3, true);
				$uiv[0]=$u[0];$uiv[1]=$u[1];
				$this->store($u,&$output);

				$ixlen-=8;
				for($i=0;$i<8;$i++) $chain[$i+$ix]=$output[$i];
				$ix+=8;
				for($i=0;$i<8;$i++) $input[$i]=$chain[$i+$ix];
			}
			if($ixlen>0){
				$tmp=array(8);
				for($i=0;$i<$ixlen;$i++) $input[$i]=$chain[$i+$ix];
				for($i=$ixlen;$i<8;$i++) $input[$i]=0;
				$this->load($input,&$u);
				$u[0] ^= $uiv[0]; $u[1] ^= $uiv[1];
				$this->_des3_encrypt(&$u,$ks1,$ks2,$ks3, true);
				$this->store($u, &$output);
				for($i=0;$i<8;$i++) $chain[$i+$ix]=$output[$i];
			}
		}else{
		}
		$this->store($uiv, &$iv);
		$uiv[0] = 0; $u[0] = 0; $uiv[1] = 0; $u[1] = 0;
//		$stringh="";
//		for($i=0;$i<$size;$i++) $stringh.=sprintf("%02x",$chain[$i]);
//		print "CHAIN:  $ixlen ".$stringh."\n";
		 return($chain);
	}

	function get(){
		$stringh="";
		for($i=0;$i<strlen($this->out);$i++) $stringh.=sprintf("%02x",ord(substr($this->out,$i,1)));
		return($stringh);
	}

	function des_make_key_sched($key){
	
		$PC1_CL = array(
			0x00000000, 0x00000010, 0x00001000, 0x00001010,
			0x00100000, 0x00100010, 0x00101000, 0x00101010
		);
			
		$PC1_DL = array(
			0x00000000, 0x00100000, 0x00001000, 0x00101000,
			0x00000010, 0x00100010, 0x00001010, 0x00101010,
			0x00000001, 0x00100001, 0x00001001, 0x00101001,
			0x00000011, 0x00100011, 0x00001011, 0x00101011
		);

		$PC1_CR = array(
			0x00000000, 0x00000001, 0x00000100, 0x00000101,
			0x00010000, 0x00010001, 0x00010100, 0x00010101,
			0x01000000, 0x01000001, 0x01000100, 0x01000101,
			0x01010000, 0x01010001, 0x01010100, 0x01010101
		);
		
		$PC1_DR = array(
			0x00000000, 0x01000000, 0x00010000, 0x01010000,
			0x00000100, 0x01000100, 0x00010100, 0x01010100
		);
			
		$PC2_C = array(
			array(
			0x00000000, 0x00004000, 0x00040000, 0x00044000,
			0x00000100, 0x00004100, 0x00040100, 0x00044100,
			0x00020000, 0x00024000, 0x00060000, 0x00064000,
			0x00020100, 0x00024100, 0x00060100, 0x00064100,
			0x00000001, 0x00004001, 0x00040001, 0x00044001,
			0x00000101, 0x00004101, 0x00040101, 0x00044101,
			0x00020001, 0x00024001, 0x00060001, 0x00064001,
			0x00020101, 0x00024101, 0x00060101, 0x00064101,
			0x00080000, 0x00084000, 0x000c0000, 0x000c4000,
			0x00080100, 0x00084100, 0x000c0100, 0x000c4100,
			0x000a0000, 0x000a4000, 0x000e0000, 0x000e4000,
			0x000a0100, 0x000a4100, 0x000e0100, 0x000e4100,
			0x00080001, 0x00084001, 0x000c0001, 0x000c4001,
			0x00080101, 0x00084101, 0x000c0101, 0x000c4101,
			0x000a0001, 0x000a4001, 0x000e0001, 0x000e4001,
			0x000a0101, 0x000a4101, 0x000e0101, 0x000e4101
			),
			array(
			0x00000000, 0x00000002, 0x00000200, 0x00000202, 
			0x00200000, 0x00200002, 0x00200200, 0x00200202,
			0x00001000, 0x00001002, 0x00001200, 0x00001202,
			0x00201000, 0x00201002, 0x00201200, 0x00201202,
			0x00000040, 0x00000042, 0x00000240, 0x00000242,
			0x00200040, 0x00200042, 0x00200240, 0x00200242,
			0x00001040, 0x00001042, 0x00001240, 0x00001242,
			0x00201040, 0x00201042, 0x00201240, 0x00201242,
			0x00000010, 0x00000012, 0x00000210, 0x00000212,
			0x00200010, 0x00200012, 0x00200210, 0x00200212,
			0x00001010, 0x00001012, 0x00001210, 0x00001212,
			0x00201010, 0x00201012, 0x00201210, 0x00201212,
			0x00000050, 0x00000052, 0x00000250, 0x00000252,
			0x00200050, 0x00200052, 0x00200250, 0x00200252,
			0x00001050, 0x00001052, 0x00001250, 0x00001252,
			0x00201050, 0x00201052, 0x00201250, 0x00201252
			),
			array(
			0x00000000, 0x00000004, 0x00000400, 0x00000404, 
			0x00400000, 0x00400004, 0x00400400, 0x00400404,
			0x00000020, 0x00000024, 0x00000420, 0x00000424,
			0x00400020, 0x00400024, 0x00400420, 0x00400424,
			0x00008000, 0x00008004, 0x00008400, 0x00008404,
			0x00408000, 0x00408004, 0x00408400, 0x00408404,
			0x00008020, 0x00008024, 0x00008420, 0x00008424,
			0x00408020, 0x00408024, 0x00408420, 0x00408424,
			0x00800000, 0x00800004, 0x00800400, 0x00800404,
			0x00c00000, 0x00c00004, 0x00c00400, 0x00c00404,
			0x00800020, 0x00800024, 0x00800420, 0x00800424,
			0x00c00020, 0x00c00024, 0x00c00420, 0x00c00424,
			0x00808000, 0x00808004, 0x00808400, 0x00808404,
			0x00c08000, 0x00c08004, 0x00c08400, 0x00c08404,
			0x00808020, 0x00808024, 0x00808420, 0x00808424,
			0x00c08020, 0x00c08024, 0x00c08420, 0x00c08424
			),
			array(
			    0x00000000, 0x00010000, 0x00000008, 0x00010008, 
				0x00000080, 0x00010080, 0x00000088, 0x00010088,
				0x00100000, 0x00110000, 0x00100008, 0x00110008,
				0x00100080, 0x00110080, 0x00100088, 0x00110088,
				0x00000800, 0x00010800, 0x00000808, 0x00010808,
				0x00000880, 0x00010880, 0x00000888, 0x00010888,
				0x00100800, 0x00110800, 0x00100808, 0x00110808,
				0x00100880, 0x00110880, 0x00100888, 0x00110888,
				0x00002000, 0x00012000, 0x00002008, 0x00012008,
				0x00002080, 0x00012080, 0x00002088, 0x00012088,
				0x00102000, 0x00112000, 0x00102008, 0x00112008,
				0x00102080, 0x00112080, 0x00102088, 0x00112088,
				0x00002800, 0x00012800, 0x00002808, 0x00012808,
				0x00002880, 0x00012880, 0x00002888, 0x00012888,
				0x00102800, 0x00112800, 0x00102808, 0x00112808,
				0x00102880, 0x00112880, 0x00102888, 0x00112888
			)
		);
		
		$PC2_D = array(
			array(
				0x00000000, 0x00000080, 0x00002000, 0x00002080,
				0x00000001, 0x00000081, 0x00002001, 0x00002081,
				0x00200000, 0x00200080, 0x00202000, 0x00202080,
				0x00200001, 0x00200081, 0x00202001, 0x00202081,
				0x00020000, 0x00020080, 0x00022000, 0x00022080,
				0x00020001, 0x00020081, 0x00022001, 0x00022081,
				0x00220000, 0x00220080, 0x00222000, 0x00222080,
				0x00220001, 0x00220081, 0x00222001, 0x00222081,
				0x00000002, 0x00000082, 0x00002002, 0x00002082,
				0x00000003, 0x00000083, 0x00002003, 0x00002083,
				0x00200002, 0x00200082, 0x00202002, 0x00202082,
				0x00200003, 0x00200083, 0x00202003, 0x00202083,
				0x00020002, 0x00020082, 0x00022002, 0x00022082,
				0x00020003, 0x00020083, 0x00022003, 0x00022083,
				0x00220002, 0x00220082, 0x00222002, 0x00222082,
				0x00220003, 0x00220083, 0x00222003, 0x00222083
			),
			array(
				0x00000000, 0x00000010, 0x00800000, 0x00800010,
				0x00010000, 0x00010010, 0x00810000, 0x00810010,
				0x00000200, 0x00000210, 0x00800200, 0x00800210,
				0x00010200, 0x00010210, 0x00810200, 0x00810210,
				0x00100000, 0x00100010, 0x00900000, 0x00900010,
				0x00110000, 0x00110010, 0x00910000, 0x00910010,
				0x00100200, 0x00100210, 0x00900200, 0x00900210,
				0x00110200, 0x00110210, 0x00910200, 0x00910210,
				0x00000004, 0x00000014, 0x00800004, 0x00800014,
				0x00010004, 0x00010014, 0x00810004, 0x00810014,
				0x00000204, 0x00000214, 0x00800204, 0x00800214,
				0x00010204, 0x00010214, 0x00810204, 0x00810214,
				0x00100004, 0x00100014, 0x00900004, 0x00900014,
				0x00110004, 0x00110014, 0x00910004, 0x00910014,
				0x00100204, 0x00100214, 0x00900204, 0x00900214,
				0x00110204, 0x00110214, 0x00910204, 0x00910214
			),
			array(
				0x00000000, 0x00000400, 0x00001000, 0x00001400,
				0x00080000, 0x00080400, 0x00081000, 0x00081400,
				0x00000020, 0x00000420, 0x00001020, 0x00001420,
				0x00080020, 0x00080420, 0x00081020, 0x00081420,
				0x00004000, 0x00004400, 0x00005000, 0x00005400,
				0x00084000, 0x00084400, 0x00085000, 0x00085400,
				0x00004020, 0x00004420, 0x00005020, 0x00005420,
				0x00084020, 0x00084420, 0x00085020, 0x00085420,
				0x00000800, 0x00000c00, 0x00001800, 0x00001c00,
				0x00080800, 0x00080c00, 0x00081800, 0x00081c00,
				0x00000820, 0x00000c20, 0x00001820, 0x00001c20,
				0x00080820, 0x00080c20, 0x00081820, 0x00081c20,
				0x00004800, 0x00004c00, 0x00005800, 0x00005c00,
				0x00084800, 0x00084c00, 0x00085800, 0x00085c00,
				0x00004820, 0x00004c20, 0x00005820, 0x00005c20,
				0x00084820, 0x00084c20, 0x00085820, 0x00085c20
			),
			array(
				0x00000000, 0x00000100, 0x00040000, 0x00040100,
				0x00000040, 0x00000140, 0x00040040, 0x00040140,
				0x00400000, 0x00400100, 0x00440000, 0x00440100,
				0x00400040, 0x00400140, 0x00440040, 0x00440140,
				0x00008000, 0x00008100, 0x00048000, 0x00048100,
				0x00008040, 0x00008140, 0x00048040, 0x00048140,
				0x00408000, 0x00408100, 0x00448000, 0x00448100,
				0x00408040, 0x00408140, 0x00448040, 0x00448140,
				0x00000008, 0x00000108, 0x00040008, 0x00040108,
				0x00000048, 0x00000148, 0x00040048, 0x00040148,
				0x00400008, 0x00400108, 0x00440008, 0x00440108,
				0x00400048, 0x00400148, 0x00440048, 0x00440148,
				0x00008008, 0x00008108, 0x00048008, 0x00048108,
				0x00008048, 0x00008148, 0x00048048, 0x00048148,
				0x00408008, 0x00408108, 0x00448008, 0x00448108,
				0x00408048, 0x00408148, 0x00448048, 0x00448148
			)
		);

			$tmp=0;
			$kix=0;

			$tmp = ($key[$kix++]) <<24;
			$tmp |=($key[$kix++]) <<16;
			$tmp |=($key[$kix++]) <<8;
			$tmp |=($key[$kix++]);
			

			$c =  $PC1_CL[($tmp >> 29) & 0x7]
			| ($PC1_CL[($tmp >> 21) & 0x7] << 1)
			| ($PC1_CL[($tmp >> 13) & 0x7] << 2)
			| ($PC1_CL[($tmp >>  5) & 0x7] << 3);
			$d =  $PC1_DL[($tmp >> 25) & 0xf]
			| ($PC1_DL[($tmp >> 17) & 0xf] << 1)
			| ($PC1_DL[($tmp >>  9) & 0xf] << 2)
			| ($PC1_DL[($tmp >>  1) & 0xf] << 3);

	
			$tmp = ($key[$kix++]) <<24;
			$tmp |=($key[$kix++]) <<16;
			$tmp |=($key[$kix++]) <<8;
			$tmp |=($key[$kix++]);

			$c |= $PC1_CR[($tmp >> 28) & 0xf]
			| ($PC1_CR[($tmp >> 20) & 0xf] << 1)
			| ($PC1_CR[($tmp >> 12) & 0xf] << 2)
			| ($PC1_CR[($tmp >>  4) & 0xf] << 3);
			$d |= $PC1_DR[($tmp >> 25) & 0x7]
			| ($PC1_DR[($tmp >> 17) & 0x7] << 1)
			| ($PC1_DR[($tmp >>  9) & 0x7] << 2)
			| ($PC1_DR[($tmp >>  1) & 0x7] << 3);
		
			define(TWO_BIT_SHIFTS,0x7efc);
			$two_bit_shifts = TWO_BIT_SHIFTS;
			$lix=0;
			for ($i = 16; $i > 0; $i--) {
				
				if ($two_bit_shifts & 0x1) {
					$c = (($c << 2) & 0xffffffc) | ($c >> 26);
					$d = (($d << 2) & 0xffffffc) | ($d >> 26);
				} else {
					$c = (($c << 1) & 0xffffffe) | ($c >> 27);
					$d = (($d << 1) & 0xffffffe) | ($d >> 27);
				}
				$two_bit_shifts >>= 1;
				$ltmp = $PC2_C[0][(($c >> 22) & 0x3f)]
					| $PC2_C[1][(($c >> 15) & 0xf) | (($c >> 16) & 0x30)]
					| $PC2_C[2][(($c >>  8) & 0x3) | (($c >>  9) & 0x3c)]
					| $PC2_C[3][(($c >> 2 ) & 0x20) | (($c >> 1) & 0x18) | ($c & 0x7)];
				$rtmp = $PC2_D[0][(($d >> 22) & 0x3f)]
					| $PC2_D[1][(($d >> 14) & 0xf) | (($d >> 15) & 0x30)]
					| $PC2_D[2][(($d >>  7) & 0x3f)]
					| $PC2_D[3][(($d      ) & 0x3) | (($d >>  1) & 0x3c)];
				
				$sch[$lix]  =  ($ltmp & 0x00fc0000) << 6;
				$sch[$lix] |=    ($ltmp & 0x00000fc0) << 10;
				$sch[$lix] |=    ($rtmp & 0x00fc0000) >> 10;
				$sch[$lix] |= ($rtmp & 0x00000fc0) >> 6;
				$lix++;
				
				$sch[$lix]  =    ($ltmp & 0x0003f000) << 12;
				$sch[$lix] |=    ($ltmp & 0x0000003f) << 16;
				$sch[$lix] |=    ($rtmp & 0x0003f000) >> 4;
				$sch[$lix] |= ($rtmp & 0x0000003f);
				$lix++;
			}

		return($sch);
	}

	function DES_set_odd_parity($in){

		$odd_parity=array(
			1,  1,  2,  2,  4,  4,  7,  7,  8,  8, 11, 11, 13, 13, 14, 14,
			16, 16, 19, 19, 21, 21, 22, 22, 25, 25, 26, 26, 28, 28, 31, 31,
			32, 32, 35, 35, 37, 37, 38, 38, 41, 41, 42, 42, 44, 44, 47, 47,
			49, 49, 50, 50, 52, 52, 55, 55, 56, 56, 59, 59, 61, 61, 62, 62,
			64, 64, 67, 67, 69, 69, 70, 70, 73, 73, 74, 74, 76, 76, 79, 79,
			81, 81, 82, 82, 84, 84, 87, 87, 88, 88, 91, 91, 93, 93, 94, 94,
			97, 97, 98, 98,100,100,103,103,104,104,107,107,109,109,110,110,
			112,112,115,115,117,117,118,118,121,121,122,122,124,124,127,127,
			128,128,131,131,133,133,134,134,137,137,138,138,140,140,143,143,
			145,145,146,146,148,148,151,151,152,152,155,155,157,157,158,158,
			161,161,162,162,164,164,167,167,168,168,171,171,173,173,174,174,
			176,176,179,179,181,181,182,182,185,185,186,186,188,188,191,191,
			193,193,194,194,196,196,199,199,200,200,203,203,205,205,206,206,
			208,208,211,211,213,213,214,214,217,217,218,218,220,220,223,223,
			224,224,227,227,229,229,230,230,233,233,234,234,236,236,239,239,
			241,241,242,242,244,244,247,247,248,248,251,251,253,253,254,254,
		);

			for($j=0;$j<8;$j++){
				$in[$j]=$odd_parity[$in[$j]];
			}
		return $in;
	}

	function DES3_set_odd_parity($in){

		$odd_parity=array(
			1,  1,  2,  2,  4,  4,  7,  7,  8,  8, 11, 11, 13, 13, 14, 14,
			16, 16, 19, 19, 21, 21, 22, 22, 25, 25, 26, 26, 28, 28, 31, 31,
			32, 32, 35, 35, 37, 37, 38, 38, 41, 41, 42, 42, 44, 44, 47, 47,
			49, 49, 50, 50, 52, 52, 55, 55, 56, 56, 59, 59, 61, 61, 62, 62,
			64, 64, 67, 67, 69, 69, 70, 70, 73, 73, 74, 74, 76, 76, 79, 79,
			81, 81, 82, 82, 84, 84, 87, 87, 88, 88, 91, 91, 93, 93, 94, 94,
			97, 97, 98, 98,100,100,103,103,104,104,107,107,109,109,110,110,
			112,112,115,115,117,117,118,118,121,121,122,122,124,124,127,127,
			128,128,131,131,133,133,134,134,137,137,138,138,140,140,143,143,
			145,145,146,146,148,148,151,151,152,152,155,155,157,157,158,158,
			161,161,162,162,164,164,167,167,168,168,171,171,173,173,174,174,
			176,176,179,179,181,181,182,182,185,185,186,186,188,188,191,191,
			193,193,194,194,196,196,199,199,200,200,203,203,205,205,206,206,
			208,208,211,211,213,213,214,214,217,217,218,218,220,220,223,223,
			224,224,227,227,229,229,230,230,233,233,234,234,236,236,239,239,
			241,241,242,242,244,244,247,247,248,248,251,251,253,253,254,254,
		);
		for($i=0;$i<3;$i++){
			for($j=0;$j<8;$j++){
				$in[($i*8)+$j]=$odd_parity[$in[($i*8)+$j]];
			}
		}
		return $in;
	}

	function des_is_weak_key($in){

		$weak_keys=array(
		/* weak keys */
		array(0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01),
		array(0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFE),
		array(0x1F,0x1F,0x1F,0x1F,0x0E,0x0E,0x0E,0x0E),
		array(0xE0,0xE0,0xE0,0xE0,0xF1,0xF1,0xF1,0xF1),
		/* semi-weak keys */
		array(0x01,0xFE,0x01,0xFE,0x01,0xFE,0x01,0xFE),
		array(0xFE,0x01,0xFE,0x01,0xFE,0x01,0xFE,0x01),
		array(0x1F,0xE0,0x1F,0xE0,0x0E,0xF1,0x0E,0xF1),
		array(0xE0,0x1F,0xE0,0x1F,0xF1,0x0E,0xF1,0x0E),
		array(0x01,0xE0,0x01,0xE0,0x01,0xF1,0x01,0xF1),
		array(0xE0,0x01,0xE0,0x01,0xF1,0x01,0xF1,0x01),
		array(0x1F,0xFE,0x1F,0xFE,0x0E,0xFE,0x0E,0xFE),
		array(0xFE,0x1F,0xFE,0x1F,0xFE,0x0E,0xFE,0x0E),
		array(0x01,0x1F,0x01,0x1F,0x01,0x0E,0x01,0x0E),
		array(0x1F,0x01,0x1F,0x01,0x0E,0x01,0x0E,0x01),
		array(0xE0,0xFE,0xE0,0xFE,0xF1,0xFE,0xF1,0xFE),
		array(0xFE,0xE0,0xFE,0xE0,0xFE,0xF1,0xFE,0xF1));

		for ($i=0; $i<16; $i++){
			if(count(array_diff($in,$weak_keys[$i]))==0) return(true);;
		}
		return(false);
	}

	function Des3StringToKey($pass,$realm,$username){
		$n_fold_key=$this->_krb5_n_fold($pass.$realm.$username,24);
//		$stringh="";
//		for($i=0;$i<strlen($n_fold_key);$i++) $stringh.=sprintf("%02x",ord(substr($n_fold_key,$i,1)));
//		print "IKEY: ".$stringh."\n";
		$chain=array(24); for($i=0;$i<24;$i++) $chain[$i]=ord(substr($n_fold_key,$i,1));
		$ix=0;
		$key1=array(8); for($i=0;$i<8;$i++) $key1[$i]=$chain[$i];
		$key2=array(8); for($i=0;$i<8;$i++) $key2[$i]=$chain[$i+8];
		$key3=array(8); for($i=0;$i<8;$i++) $key3[$i]=$chain[$i+16];
		$ks1=$this->des_make_key_sched($key1);
		$ks2=$this->des_make_key_sched($key2);
		$ks3=$this->des_make_key_sched($key3);
		
		$this->out=$this->DES_ede3_cbc_encrypt($chain,true,$ks1,$ks2,$ks3,24);
		$keys=array(3);
		$this->out=$this->DES3_set_odd_parity($this->out);
		for($i = 0;$i < 3; $i++){
			$keys[$i]=array(8);
			for($j = 0;$j < 8; $j++) $keys[$i][$j]=$this->out[($i*8)+$j];
			if($this->DES_is_weak_key($keys[$i])){
				$keys[$i][7]^=0xf0;
				$this->out[($i*8)+7]=$keys[$i][7];
			}
		}
//		$stringh="";
//		for($i=0;$i<24;$i++) $stringh.=sprintf("%02x",$this->out[$i]);
//		print "OUT KEY: ".$stringh."\n";
	}

	function des_cbc_cksum($key,$data,$sch){
		$pos=0;
		$u=array(0,0);
		$uiv=array(0,0);
		$this->load($key,&$uiv);
		$len=count($data);
		$input=array(8);for($i=0;$i<8;$i++) $input[$i]=$data[$i];
		$ix=0;
			while ($len >=8) {
				$this->load($input,&$u);
				$u[0] ^= $uiv[0]; $u[1] ^= $uiv[1];
				$this->IP(&$u);
				$this->desx(&$u, $sch, true);
				$this->FP(&$u);
				$uiv[0] = $u[0]; $uiv[1] = $u[1];
				$len -= 8;
				for($i=0;$i<8;$i++) $data[$i+$ix]=$input[$i];
				$ix+=8;$input=array(8);for($i=0;$i<8;$i++) $input[$i]=$data[$i+$ix];
			}
			if($len>0){
				$input=array(8);
				for($i=0;$i<$len;$i++) $input[$i]=$data[$i+$ix];
				for($i=$len;$i<8;$i++) $input[$i]=0;
				$this->load($input,&$u);
				$u[0] ^= $uiv[0]; $u[1] ^= $uiv[1];
				$this->IP(&$u);
				$this->desx(&$u, $sch, true);
				$this->FP(&$u);
			}
		$this->store($u,&$key);
		return($key);
	}

	function DesStringToKey($pass,$realm,$username){
		$data=$pass.$realm.$username;
		$len=strlen($data);
		$reverse = 0;
		$ix=0;
		$key=array(8); for($i=0;$i<8;$i++) $key[$i]=0;
		$tmp=array($len);for($i=0;$i<$len;$i++) $tmp[$i]=ord(substr($data,$i,1));
		$swap=array( 0x0, 0x8, 0x4, 0xc, 0x2, 0xa, 0x6, 0xe,
			     0x1, 0x9, 0x5, 0xd, 0x3, 0xb, 0x7, 0xf );
		for ($i = 0; $i < $len; $i++) {
			if ($reverse==0){
				$key[$ix]^=($tmp[$i] << 1);
				$ix++;
			}else{
				$ix--;
				$key[$ix]^=($swap[$tmp[$i] & 0xf] <<4)| $swap[(($tmp[$i] & 0xf0) >> 4) &0x0f];
			}
			if(($i % 8) == 7){
				if($reverse==0) $reverse=1;else $reverse=0;
			}
		}
		$key=$this->DES_set_odd_parity($key);
		if($this->DES_is_weak_key($key)){
			$key[7]^=0xf0;
		}
		$sch=$this->des_make_key_sched($key);
		$key=$this->des_cbc_cksum($key,$tmp,$sch);
		$key=$this->DES_set_odd_parity($key);
		if($this->DES_is_weak_key($key)){
			$key[7]^=0xf0;
		}
		$this->out=$key;
	}

	function DES3_random_to_key($data){
		$key=array(24); for ($i=0;$i<24;$i++) $key[$i]=0;

		for($i=0;$i<3;++$i){
			for ($j=0;$j<7;++$j){
				$key[(8*$i)+$j]=$data[7*$i+$j];
			}

			$foo = 0;
			for ($j = 6; $j >= 0; --$j) {
	    			$foo |= $data[7 *$i +$j] & 1;
	    			$foo <<= 1;
			}
			$key[(8*$i)+7]=$foo;
		}
		$keys=array(3);
		$out=$key;
		$out=$this->DES3_set_odd_parity($out);
		for($i = 0;$i < 3; $i++){
			$keys[$i]=array(8);
			for($j = 0;$j < 8; $j++) $keys[$i][$j]=$out[($i*8)+$j];
			if($this->DES_is_weak_key($keys[$i])){
				$keys[$i][7]^=0xf0;
				$out[($i*8)+7]=$keys[$i][7];
			}
		}
		return($out);
	}

	function Des3StringToKeyDerived($pass,$realm,$username){
		$n_fold_key=$this->_krb5_n_fold($pass.$realm.$username,21);
//		$stringh="";
//		for($i=0;$i<strlen($n_fold_key);$i++) $stringh.=sprintf("%02x",ord(substr($n_fold_key,$i,1)));
//		print "IKEY: ".$stringh."\n";
		$data=array(21); for($i=0;$i<strlen($n_fold_key);$i++) $data[$i]=ord(substr($n_fold_key,$i,1));
		$this->out=$this->DES3_random_to_key($data);
		$keys=array(3);
		for($i = 0;$i < 3; $i++){
			$keys[$i]=array(8);
			for($j = 0;$j < 8; $j++) $keys[$i][$j]=$this->out[($i*8)+$j];
		}
		
		$ks1=$this->des_make_key_sched($keys[0]);
		$ks2=$this->des_make_key_sched($keys[1]);
		$ks3=$this->des_make_key_sched($keys[2]);

		$derivekey=$this->derive_key("kerberos",$this->out,$ks1,$ks2,$ks3,24);
		$this->out=$this->DES3_random_to_key($derivekey);
		
	}


	function derive_key($constant, $data,$ks1,$ks2,$ks3,$ixlen){
		$len=strlen($constant);
		$n_fold_key=$this->_krb5_n_fold($constant,$len);
		$lk=array(8); for($i=0;$i<8;$i++) $lk[$i]=ord(substr($n_fold_key,$i,1));
		$keys=array(3);
		$keys[0]=$lk;

		$keys[0]=$this->DES_ede3_cbc_encrypt($keys[0],true,$ks1,$ks2,$ks3,8);
		$keys[1]=$this->DES_ede3_cbc_encrypt($keys[0],true,$ks1,$ks2,$ks3,8);
		$keys[2]=$this->DES_ede3_cbc_encrypt($keys[1],true,$ks1,$ks2,$ks3,8);

		$out=array(24);
		for ($i=0;$i<3;$i++){
			for ($j=0;$j<8;$j++)
				$out[($i*8)+$j]=$keys[$i][$j];
		}
		return($out);
	}

	function derive_key_integrity($masterkey){
		$keys=array(3);
		for($i = 0;$i < 3; $i++){
			$keys[$i]=array(8);
			for($j = 0;$j < 8; $j++) $keys[$i][$j]=$masterkey[($i*8)+$j];
		}
		
		$ks1=$this->des_make_key_sched($keys[0]);
		$ks2=$this->des_make_key_sched($keys[1]);
		$ks3=$this->des_make_key_sched($keys[2]);

		$usage=$this->getIntegrityUsage();

		$n_fold_key=$this->_krb5_n_fold($usage,8);
		$lk=array(8); for($i=0;$i<8;$i++) $lk[$i]=ord(substr($n_fold_key,$i,1));
		
		$keys=array(3);
		$keys[0]=$lk;
		
		$keys[0]=$this->DES_ede3_cbc_encrypt($keys[0],true,$ks1,$ks2,$ks3,8);
		$keys[1]=$this->DES_ede3_cbc_encrypt($keys[0],true,$ks1,$ks2,$ks3,8);
		$keys[2]=$this->DES_ede3_cbc_encrypt($keys[1],true,$ks1,$ks2,$ks3,8);

		$out=array(24);
		for ($i=0;$i<3;$i++){
			for ($j=0;$j<8;$j++)
				$out[($i*8)+$j]=$keys[$i][$j];
		}

		$out=$this->DES3_random_to_key($out);
/*		$stringh="";
		for($i=0;$i<24;$i++) $stringh.=sprintf("%02x",$out[$i]);
		print "DKEY: ".$stringh."\n";*/
		return($out);
	}

	function derive_key_encrypt($masterkey){
		$keys=array(3);
		for($i = 0;$i < 3; $i++){
			$keys[$i]=array(8);
			for($j = 0;$j < 8; $j++) $keys[$i][$j]=$masterkey[($i*8)+$j];
		}
		
		$ks1=$this->des_make_key_sched($keys[0]);
		$ks2=$this->des_make_key_sched($keys[1]);
		$ks3=$this->des_make_key_sched($keys[2]);

		$usage=$this->getEncryptionUsage();

		$n_fold_key=$this->_krb5_n_fold($usage,8);
		$lk=array(8); for($i=0;$i<8;$i++) $lk[$i]=ord(substr($n_fold_key,$i,1));
		
		$keys=array(3);
		$keys[0]=$lk;
		
		$keys[0]=$this->DES_ede3_cbc_encrypt($keys[0],true,$ks1,$ks2,$ks3,8);
		$keys[1]=$this->DES_ede3_cbc_encrypt($keys[0],true,$ks1,$ks2,$ks3,8);
		$keys[2]=$this->DES_ede3_cbc_encrypt($keys[1],true,$ks1,$ks2,$ks3,8);

		$out=array(24);
		for ($i=0;$i<3;$i++){
			for ($j=0;$j<8;$j++)
				$out[($i*8)+$j]=$keys[$i][$j];
		}

		$out=$this->DES3_random_to_key($out);
/*		$stringh="";
		for($i=0;$i<24;$i++) $stringh.=sprintf("%02x",$out[$i]);
		print "DKEY: ".$stringh."\n";*/
		return($out);
	}

	function krb5_generate_random_block($len=8){
		$key=$this->DES_new_random_key();
		$sched=$this->des_make_key_sched($key);
		$counter=$this->DES_new_random_key();
		$out=$counter;
		$this->DES_ecb_encrypt(&$out,$sched);
	}

	function DES_new_random_key($len=8){
		$seed=array($len);
		//inicializacion!!!!
		do{
			$seed=$this->DES_generate_random_block($len);
			$this->DES_set_odd_parity(&$seed);
		}while ($this->DES_is_weak_key($seed));
		$sequence_seed=$this->des_make_key_sched($seed);
		//DES_ecb_encrypt!!!!
		do{
			$this->DES_ecb_encrypt(&$seed,$sequence_seed);
			$this->DES_set_odd_parity(&$seed);
		}while ($this->DES_is_weak_key($seed));
		return($seed);
	}

	function DES_generate_random_block($len=8){
		$res=array($len);
		for ($i=0;$i<$len;$i++){
			$res[$i]=mt_rand(0,255);
		}
		return($res);
	}

	function DES_ecb_encrypt($key,$sch){
		$u=array(0,0);
		$this->load($key,&$u);
		//DES_encrypt
		$this->IP(&$u);
		$this->desx(&$u, $sch, true);
		$this->FP(&$u);
		$this->store($u,&$key);
		return($key);
	}

	function ENCRYPTION_USAGE($u){
		return(($u << 8) | 0xAA);
	}

	function INTEGRITY_USAGE($u){
		return(($u << 8) | 0x55);
	}

	function getIntegrityUsage(){
		$usage=$this->INTEGRITY_USAGE(HDB_KU_MKEY);
		$ausage=array(5);
		$ausage[4] = $usage & 0xff;
		$usage= $usage >> 8;
		$ausage[3] = $usage & 0xff;
		$usage= $usage >> 8;
		$ausage[2] = $usage & 0xff;
		$usage= $usage >> 8;
		$ausage[1] = $usage & 0xff;
		$usage= $usage >> 8;
		$ausage[0] = $usage & 0xff;
		$str="";
		for ($i=0;$i<5;$i++){
			$str.=chr($ausage[$i]);
		}
		return($str);
	}
		
	function getEncryptionUsage(){
		$usage=$this->ENCRYPTION_USAGE(HDB_KU_MKEY);
		$ausage=array(5);
		$ausage[4] = $usage & 0xff;
		$usage= $usage >> 8;
		$ausage[3] = $usage & 0xff;
		$usage= $usage >> 8;
		$ausage[2] = $usage & 0xff;
		$usage= $usage >> 8;
		$ausage[1] = $usage & 0xff;
		$usage= $usage >> 8;
		$ausage[0] = $usage & 0xff;
		$str="";
		for ($i=0;$i<5;$i++){
			$str.=chr($ausage[$i]);
		}
		return($str);
	}


};



?>