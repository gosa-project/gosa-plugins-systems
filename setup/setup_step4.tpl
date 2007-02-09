<h2>{t}ldap connection{/t}</h2>
	<div class='step4_container'>
		<div class='step4_name'>
			{t}Location description{/t}
		</div>
		<div class='step4_value'>
			<input type='text' name='location' maxlength='80' size='25' value='{$location}'>
		</div>
		<div class='step4_status'>
		</div>
	</div>
	<div class='step4_container'>
		<div class='step4_name'>
			<div style='vertical-align:middle;height:100%;'>{t}Connection url{/t}</div>
		</div>
		<div class='step4_value'>
			<input type='text' name='connection' maxlength='80' size='25' value='{$connection}'>
		</div>
		<div class='step4_status'>
			{if !$connection_established}
				{t}Connection failed{/t}	
			{/if}
			<input type='submit' name='next' value='{t}Next{/t}'>
		</div>
		<div>
		</div>
	</div>

	<div class='step4_container'>
		<div class='step4_name'> 
			{t}Admin DN{/t}
		</div>
		<div class='step4_value'>
			<input type='text' name='admin' maxlength='80' size='40' value='{$admin}'>
		</div>
		<div class='step4_status'>
			asd
		</div>
	</div>

	<div class='step4_container'>
		<div class='step4_name'>
			{t}Admin password{/t}
		</div>
		<div class='step4_value'>
			<input type='password' name='password' maxlength='20' size='20' value='{$password}'>
		</div>
		<div class='step4_status'>
			asd
		</div>
	</div>
	<div class='step4_container'>
		<div class='step4_name'> 
			{t}Base{/t}
		</div>
		<div class='step4_value'>
			<input type='text' name='base' maxlength='80' size='40' value='{$base}'>
		</div>
		<div class='step4_status'>
			<input type='submit' name='get_base' value="{t}Get base{/t}">
		</div>
	</div>
		

{if $connection_established}

		
	<div class='step4_container'>
		<div class='step4_name'> 
			{t}People storage ou{/t}
		</div>
		<div class='step4_value'>
			<input type='text' name='peopleou' maxlength='40' size='20' value='{$peopleou}'>
		</div>
		<div class='step4_status'>
			asd
		</div>
	</div>
		
	<div class='step4_container'>
		<div class='step4_name'>
			{t}People dn attribute{/t}
		</div>
		<div class='step4_value'>
			<select size="1" name="peopledn">
				{html_options values=$peopledns output=$peopledns selected=$peopledn}
			</select>
		</div>
		<div class='step4_status'>
			asd
		</div>
	</div>
		
	<div class='step4_container'>
		<div class='step4_name'>
			{t}Group storage ou{/t}
		</div>
		<div class='step4_value'>
			<input type='text' name='groupou' maxlength='40' size='20' value='{$groupou}'>
		</div>
		<div class='step4_status'>
			asd
		</div>
	</div>

	<div class='step4_container'>
		<div class='step4_name'>
			{t}ID base for users/groups{/t}
		</div>
		<div class='step4_value'>
			<input type='text' name='uidbase' maxlength='20' size='10' value='{$uidbase}'>
		</div>
		<div class='step4_status'>
			asd
		</div>
	</div>

	<div class='step4_container'>
		<div class='step4_name'>
			{t}Encryption algorithm{/t}
		</div>
		<div class='step4_value'>
			<select name="encryption" size="1" title="">
				{html_options options=$crypt_methods selected=$encryption}
			</select>
		</div>
		<div class='step4_status'>
			asd
		</div>
	</div>

	<div class='step4_container'>
		<div class='step4_name'>
			{t}Mail method{/t}
		</div>
		<div class='step4_value'>
			<select name="mail" size="1" title="">
				<option name="mail" value="disabled">{t}disabled{/t}</option>
				{html_options options=$mail_methods selected=$mail}
			</select>
		</div>
		<div class='step4_status'>
			asd
		</div>
	</div>

	<div class='step4_container'>
		<div class='step4_name'>
			{t}Display PHP errors{/t}
		</div>
		<div class='step4_value'>
			<select name="errorlvl" size="1" title="">
				<option value="true">{t}true{/t}</option>
				<option value="false">{t}false{/t}</option>
			</select>
		</div>
		<div class='step4_status'>
			asd
		</div>
	</div>
{/if}
<!--
		 {t}GOsa supports several encryption types for your passwords. Normally this is adjustable via user templates, but you can specify a default method to be used here, too.{/t}
		 {t}GOsa always acts as admin and manages access rights internally. This is a workaround till OpenLDAP's in directory ACI's are fully implemented. For this to work, we need the admin DN and the corresponding password.{/t}
		 {t}Some basic LDAP parameters are tunable and affect the locations where GOsa saves people and groups, including the way accounts get created. Check the values below if the fit your needs.{/t}
		 {t}GOsa has modular support for several mail methods. These methods provide interfaces to users mailboxes and general handling for quotas. You can choose the dummy plugin to leave all your mail settings untouched.{/t}-->
