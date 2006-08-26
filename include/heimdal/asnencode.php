<?php
/*
 * asnencode.php,v 1.0 2006/08/25 21:00:00
 *
 * Copyright 2006 Alejandro Escanero Blanco <aescanero@chaosdimension.org>
 *
 * See the enclosed file COPYING for license information (GPL).  If you
 * did not receive this file, see http://www.fsf.org/copyleft/gpl.html.
 */


class stream{

	var $stream;
	var $len;

	function stream(){
		$this->stream=array();
		$len=0;
	}

	function writeVal($val){
		$val=($val & 0xff); //Only one byte
		array_push($this->stream,$val);
		$this->len++;
	}
	
	function writeArray($arr){
		foreach ($arr as $val)
			array_push($this->stream,$val);
		
		$this->len+=count($arr);
	}

	function writeStream($stream){
		$str=$stream->getStream();
//		print_r($this->stream);
		foreach ($str as $val)
			array_push($this->stream,$val);
		$this->len+=count($str);
	}

	function writeOctetString($os){
		$len=strlen($os);
		$arr=array($len);
		for($i=0;$i<$len;$i++) $arr[$i]=ord(substr($os,$i,1));
		$this->writeArray($arr);
	}

	function getStream(){
		return($this->stream);
	}

	function getLen(){
		return($this->len);
	}

	function printOut($debug=false,$msg=""){
		if($debug) printf("%s len: %d\n",$msg,$this->len);
//		print_r($this->stream);
		$stringh="";
		for ($i=0;$i<$this->len;$i++) $stringh.=sprintf(" %d %02x\n",$i,$this->stream[$i]);
		printf("%s ASN: %s\n",$msg,$stringh);
	}

	function printString($debug=false,$msg=""){
		$stringh="";
		for ($i=0;$i<$this->len;$i++) $stringh.=chr($this->stream[$i]);
		return($stringh);
	}
};

	define('UNIVERSAL',0x00);
	
	define('INTEGER',0x02);
	//define('INTEGER',0x80);
	define('OCTET_STRING',0x04);
	//define('OCTET_STRING',0x81);
	define('NULL',0x05);
	define('OBJECT_IDENTIFIER ',0x06);
	define('SEQUENCE',0x10);
	define('SEQUENCE_OF',0x10);
	define('SET',0x11);
	define('SET_OF',0x11);



class asnEncode{

	var $stream;

	function asnEncode(){
		$this->stream=new stream();
	}

	function encodeOctetString($os){
		$len=strlen($os);
		$this->encodeTag(UNIVERSAL,OCTET_STRING,false,&$this->stream);
		$this->encodeLen($len,&$this->stream);
		$this->stream->writeOctetString($os);
	}

	function encodeTag($clazz,$value,$constructed,$stream){
		if ($constructed)
			$clazz |= 0x20;
		if ($value < 0x1F){
			$stream->writeVal($clazz | $value);
		} else {
			$stream->writeVal($clazz | 0x1F);
			$clazz = $value;
			while ($clazz > 63) {
				$stream->writeVal(63);
				$clazz -= 63;
			}
			$stream->writeVal($clazz);
		}
		return($stream);
	}

	function encodeInteger($val){

		$dh=dechex($val);
		$ldh=strlen($dh);
		if($ldh%2 !=0){
			$ldh="0".$ldh;
			$ldh++;
		}
		$cero=0;
		if($i==0)
				if(hexdec(substr($dh,0,2))>127) $cero=1;
		$this->encodeTag(UNIVERSAL,INTEGER,false,&$this->stream);
		$this->encodeLen(($ldh/2)+$cero,&$this->stream);
		for($i=0;$i<($ldh/2);$i++){
			if($i==0 && $cero==1) $this->stream->writeVal(0);
			$this->stream->writeVal(hexdec(substr($dh,$i*2,2)));
		}
	}


	function encodeLen($val,$stream){
		if ($val < 128) { // short definite form
			$stream->writeVal($val);
			return;
		}
		if ($val < 256) { // long definite form
			//$this->stream->writeVal(-127);
			$stream->writeVal(0x81);
			$stream->writeVal($val);
			return;
		}
		if ($val < 65536) {
			//$this->stream->writeVal(-126);
			$stream->writeVal(0x80);
			$stream->writeVal($val >> 8);
			$stream->writeVal($val);
			return;
		}
		if ($val < 16777216) {
			//$this->stream->writeVal(-125);
			$stream->writeVal(0x79);
			$stream->writeVal($val >> 16);
			$stream->writeVal($val >>  8);
			$stream->writeVal($val);
			return;
		}
		//$this->stream->writeVal(-124);
		$stream->writeVal(0x78);
		$stream->writeVal($val >> 24);
		$stream->writeVal($val >> 16);
		$stream->writeVal($val >>  8);
		$stream->writeVal($val);
		return($stream);
	}

	function printOut(){
		$this->stream->printOut();
	}

	function printString(){
		return($this->stream->printString());
	}

	function getStream(){
		return($this->stream);
	}

	function encodeSequence($ix,$stream){
		$hix=0xa0+$ix;
		$len=$stream->getLen();
		$local=new stream();
		$local->writeVal($hix);
		$this->encodeLen($len,&$local);
		$local->writeStream($stream);
		$this->stream->writeStream($local);
		return($local);
	}

	function encodeObject($tag,$stream){
		$len=$stream->getLen();
		//$this->encodeTag(UNIVERSAL,$tag,false,&$this->stream);
		$this->stream->writeVal($tag);
		$this->encodeLen($len,&$this->stream);
		$this->stream->writeStream($stream);
	}

};

?>
