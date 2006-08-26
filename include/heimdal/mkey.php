<?php
/*
 * mkey.php,v 1.0 2006/08/25 21:00:00
 *
 * Copyright 2006 Alejandro Escanero Blanco <aescanero@chaosdimension.org>
 *
 * See the enclosed file COPYING for license information (GPL).  If you
 * did not receive this file, see http://www.fsf.org/copyleft/gpl.html.
 */


class entry{
	var $timestamp;
	var $vno;
	var $keytype;
	var $data;
};

class principal{
	var $len;
	var $realm;
	var $name_type;
	var $entry;
	var $vno;
}


class mkey{

	var $pvno; //version kerberos
	var $tag;	//tag (id->version)

	var $buf;		
	var $mknvo;
	var $realm;
	var $ix_i;
	var $ix;
	var $fsz;
	var $principals;
	var $principals_l;

	//function mkey(){};

	function mkey($file){
		$this->ix=0;
		$this->ix_i=0;
		$this->buf="";
		$this->principals=array();
		$this->principals_l=0;
		$this->fsz=filesize($file);
		$mk=fopen($file,"r");
		if(!$mk){
			echo "no puedo abrir mkey";
			exit(1);
		}
		while(!feof($mk)){
			$this->buf.=fgets($mk,100);
		}

		/*print "DECODE:: \n";
		for ($i=0;$i<strlen($this->buf);$i++){ printf("%d %02x (%d) ",$i,ord(substr($this->buf,$i,1)),ord(substr($this->buf,$i,1)));echo substr($this->buf,$i,1)."\n";}*/


		//Begin procedure

		$this->pvno=$this->getdec(1);//00 version
		$this->tag=$this->getdec(1);//01 tag
		while($this->ix_i<$this->fsz){
			$this->mkeyPrincipal();
		}
	}

	function mkeyPrincipal(){
		$this->principals[]=new principal();
		$this->principals[$this->principals_l]->len=$this->getdec(4); // len
		$tmp=$this->getdec(2);// tmp
		$sz=$this->getdec(2);// size
		$this->principals[$this->principals_l]->realm=$this->getstr($sz);//realm
		$sz=$this->getdec(2);//size
		$data=$this->getstr($sz);		//Must be "K"
		$sz=$this->getdec(2);//size
		$data=$this->getstr($sz);		//Must be "M"
		$this->principals[$this->principals_l]->name_type=$this->getdec(4);//name type
		$this->principals[$this->principals_l]->entry=$this->mkeyEntry();//entry
		$this->principals[$this->principals_l]->vno=$this->getdec(4);//vno
		$this->principals_l++;
	}

	function mkeyEntry(){
		$local_entry=new entry();
		$local_entry->timestamp=$this->getdec(4);
		$local_entry->vno=$this->getdec(1);
		$local_entry->keytype=$this->getdec(2);
		$len=$this->getdec(2);
		$local_entry->data=$this->getstr($len);
		return($local_entry);
	}

	function getKeyType_Entry($pos=0){
		if($pos>($this->principals_l-1)){ echo "mkey error"; exit(-1); }
		return($this->principals[$pos]->entry->keytype);
	}

	function getKey_Entry($pos=0){
		if($pos>($this->principals_l-1)){ echo "mkey error"; exit(-1); }
		return($this->principals[$pos]->entry->data);
	}

	function getKey_Entry_A($pos=0){
		if($pos>($this->principals_l-1)){ echo "mkey error"; exit(-1); }
		$masterkey=array(strlen($mkey1));
		for ($i=0;$i<(strlen($mkey1));$i++){
			$masterkey[$i]=ord(substr($this->principals[$pos]->entry->data,$i,1));
  		}
		return($masterkey);
	}

	function getNumber_Entry(){
		return($this->principals_l);
	}

	function getdec($len){
		$res=0;
		while ($len>0){
			$res*=0xff;
			$res+=ord(substr($this->buf,$this->ix_i,1));
			$this->ix_i++;
			$len--;
		}
		return($res);
	}

	function getstr($len){
		$res="";
		while ($len>0){
			$res.=substr($this->buf,$this->ix_i,1);
			$this->ix_i++;
			$len--;
		}
		return($res);
	}


};

?>
