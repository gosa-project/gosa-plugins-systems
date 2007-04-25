<div>
    <div class='default'>

		{foreach from=$checks item=val key=key}
	
	        <div class='step2_entry_name'><b>{$checks.$key.TITLE}</b></div>


				
			{if $checks.$key.STATUS}
				<div class='step2_successful'>{$checks.$key.STATUS_MSG}</div>
			{else}
				<div class='step2_failed'>{$checks.$key.STATUS_MSG}</div>
				{if $checks.$key.ERROR_MSG}
					{$checks.$key.ERROR_MSG}
				{/if}
			{/if}
				<input type='submit' name='retry_{$key}' value='{t}Retry{/t}'>
			<p>&nbsp;</p>
		{/foreach}


	<p>&nbsp; </p>
	<p>&nbsp; </p>
	<p>&nbsp; </p>
	<p> </p>
	<p></p>
	<p class='seperator'></p>
	<p></p>
	<p class='seperator'></p>




	* Create a test department with some objects to check for correct permissions
	<br>
	* Look for ACL's. If none is there, choose one user to be the super administrator
	<br>
	* Look for organizationalUnits and let add GOsa departments for selected ones
	<br>
	* Look for person / organizationalPerson and add GOsa account
	<br>
	* Allow to move selected users to people ou's - take care for groupOfNames
	<br>
	* Allow to move selected groups to group ou's - take care for groupOfNames
	<br>
	* Allow to move selected winstations to winstation ou's - take care for groupOfNames
	<br>
	* Check for double uidNumbers/gidNumbers
	<br>
	* Check for mail accounts and add gosaMailAccount - optionally create these accounts on the IMAP server
    </div>
</div>
