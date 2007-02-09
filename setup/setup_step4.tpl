<h2>{t}ldap connection{/t}</h2>
	<div class='step4_container'>
		<div class='step4_name'>
			{t}Location description{/t}
		</div>
		<div class='step4_value'>
			<input type='text' name='location' maxlength='80' size='25' value='{$location}' style='width:100%;'>
		</div>
		<div class='step4_status'>
			
		</div>
	</div>
	<div class='step4_container'>
		<div class='step4_name'>
			<div style='vertical-align:middle;height:100%;'>{t}Connection url{/t}</div>
		</div>
		<div class='step4_value'>
			<input type='text' name='connection' maxlength='80' size='25' value='{$connection}' style='width:100%;'>
		</div>
		<div class='step4_status'>
			<input type='submit' value='{t}Try connect{/t}' style='width:90%;'>
		</div>
	</div>
	<div style='float:left; width:680px;padding-bottom:10px;'>
		{t}Status{/t}: {$connection_status}
	</div>
	<br>
	<div class='step4_container'>
		<div class='step4_name'> 
			{t}Base{/t}
		</div>
		<div class='step4_value'>

			{if $namingContextsCount >= 1}

				<select name='base' style='width:100%;'>		
					{html_options values=$namingContexts output=$namingContexts selected=$base}
				</select>
			{else}
				<input type='text' name='base' maxlength='80' size='40' value='{$base}' style='width:100%;'>
			{/if}

		</div>
		<div class='step4_status'>
		
		</div>
	</div>

	<div class='step4_container'>
		<div class='step4_name'> 
			{t}Admin DN{/t}
		</div>
		<div class='step4_value'>
			
			<input type='text' name='admin' maxlength='160' size='40' value='{$admin}' style='width:100%;'>
		</div>
		<div class='step4_status'>
			<input type='submit' name='resolve_user' value='{t}Select user{/t}' style='width:90%;' >
		</div>
	</div>

	{if $resolve_user}
	<div style='float:left; width:680px;padding-bottom:10px;border: solid 1px #CCC;'>
        <div class='step4_name' style='background-color:#DDD'>
			<input type='text' value='{$resolve_filter}' name='resolve_filter'>
        </div>
		<div class='step4_value' style='background-color:#CCC'>
			<input type='submit' value='{t}Search{/t}' name='resolve_search'>
        </div>
		<div class='step4_status'>
			<input type='submit' value='{t}Use selected user{/t}' name='use_selected_user' style='width:90%;'>
		</div>
		<select name='admin_to_use' size=6 style="width:680px;background-color:#DDD;">				
			{html_options options=$resolved_users selected=$admin}
		</select>
	</div>		
	{/if}

	<div class='step4_container'>
		<div class='step4_name'>
			{t}Admin password{/t}
		</div>
		<div class='step4_value'>
			<input type='password' name='password' maxlength='20' size='20' value='{$password}' style='width:100%;'>
		</div>
		<div class='step4_status'>
		</div>
	</div>
		

{if 1== 0}

		
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
