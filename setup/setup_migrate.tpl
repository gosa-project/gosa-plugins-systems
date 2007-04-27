<div>

	<h1 style='color:red'>Style fixes necessary here ;-)</h1>
    <div class='default'>

		{if $method == "default"}

	<p>	
		Create a reload for each entry later 
		<input type='submit' name='reload' value='{t}Reload{/t}'>
	</p>

			{foreach from=$checks item=val key=key}
				<div class='step2_entry_name'><b>{$checks.$key.TITLE}</b></div>
				{if $checks.$key.STATUS}
					<div class='step2_successful'>{$checks.$key.STATUS_MSG}
					{if $checks.$key.ERROR_MSG}
						{$checks.$key.ERROR_MSG}
					{/if}
					</div>
				{else}
					<div class='step2_failed'>{$checks.$key.STATUS_MSG}
					</div>
					<div>
					{if $checks.$key.ERROR_MSG}
						{$checks.$key.ERROR_MSG}
					{/if}
					</div>
				{/if}
				<p>&nbsp;</p>	
			{/foreach}

		{elseif $method == "create_acls"}

		<h2>{t}Acl setup{/t}</h2>
	
		{if $acl_create_selected != "" && $what_will_be_done_now!=""}
			<div>
<pre>
{$what_will_be_done_now}
</pre>
			</div>		
			<input type='submit' name='create_acls_create_confirmed' value='{t}Next{/t}'>
			<input type='submit' name='create_acls_create_abort' value='{t}Abort{/t}'>
		{else}
		<b>{t}Create a new user and a group with adminstrational acls{/t}</b><br>
	
			{t}To automatically add a new administrative user to your ldap database use the formular below.{/t}<br>
			<p style='padding-left:10px;'>
			<table>
				<tr>
					<td>
						{t}Name{/t}:&nbsp;
					</td>
					<td>
						<i>System administrator</i>
					</td>
				<tr>
				<tr>
					<td>
						{t}User ID{/t}:&nbsp;
					</td>
					<td>
						<i>admin</i>
					</td>
				<tr>
				</tr>
					<td>
						{t}Password{/t}:&nbsp;
					</td>
					<td>
						<input type='input' value='{$new_user_password}' name='new_user_password'><br>
					</td>
				</tr>
			</table>
			<input type='submit' name='create_admin_user' value='{t}Create{/t}'>	
			</p>
			
			{if $users_cnt != 0 && $groups_cnt != 0}
			
			<p>&nbsp;</p>
			<b>{t}Append administrational acls to existing an user or a group{/t}</b><br>
			Bla {t}To grant administrative permissions to a user or a group, select an element and use button below.{/t}
			<p style='padding-left:10px;'>
			<select name='acl_create_type' onChange='document.mainform.submit();' >
				{if $type == "user"}
				<option value='group'>{t}Group{/t}</option>
				<option selected value='user'>{t}User{/t}</option>
				{else}
				<option selected value='group'>{t}Group{/t}</option>
				<option value='user'>{t}User{/t}</option>
				{/if}
			</select>
			<select name='create_acls_selected' size="12" style='width:100%;'>
				{if $type == "user"}
					{html_options options=$users selected=$acl_create_selected}
				{else}
					{html_options options=$groups selected=$acl_create_selected}
				{/if}
			</select>

				<input type='submit' name='create_acls_create' value='{t}Add administrational acls to this object{/t}'>
			</p>
			{/if}
		{/if}


			<p class='seperator'>&nbsp;</p>	

			<div style='width:100%; text-align:right; padding:5px;'>
				<input type='submit' name='create_acls_cancel' value='{t}Close{/t}'>
			</div>
		

		{elseif $method == "migrate_deps"}
	
			<h2>Department migration</h2>

			{t}The listed deparmtents below are currenlty invisble in the GOsa user interface. If you want to migrate a set of departments, just select them and use the migrate button below.{/t}<br>
			{t}If you want to know what will be done when migrating the selected entries, just use the 'What will be done here' button and you will see a list of changes.{/t}
					

			<br><br>
			{foreach from=$deps_to_migrate item=val key=key}

				{if $deps_to_migrate.$key.checked}
					<input type='checkbox' name='migrate_{$key}' checked>
					{$deps_to_migrate.$key.dn}
					{if $deps_to_migrate.$key.after != ""}
						<div class="step2_entry_container_info" id="sol_8">

{t}Current{/t}
<div style='padding-left:20px;'>
<pre>
dn: {$deps_to_migrate.$key.dn}
{$deps_to_migrate.$key.before}
</pre>
</div>
{t}After migration{/t}
<div style='padding-left:20px;'>
<pre>
dn: {$deps_to_migrate.$key.dn}
{$deps_to_migrate.$key.after}
</pre>
</div>
						</div>
					{/if}
				{else}
					<input type='checkbox' name='migrate_{$key}'>
					{$deps_to_migrate.$key.dn}
				{/if}

				<br>
				

			{/foreach}
			<br>

			<input type='submit' name='deps_visible_migrate_refresh' value='{t}Reload list{/t}'>
			<input type='submit' name='deps_visible_migrate_migrate' value='{t}Migrate{/t}'>
			<input type='submit' name='deps_visible_migrate_whatsdone' value='{t}What will be done here{/t}'>

			<p class='seperator'>&nbsp;</p>	

			<div style='width:100%; text-align:right; padding:5px;'>
				<input type='submit' name='deps_visible_migrate_close' value='{t}Close{/t}'>
			</div>
		{elseif $method == "migrate_users"}
	
			<h2>User migration</h2>

			{t}The listed deparmtents below are currenlty invisble in the GOsa user interface. If you want to migrate a set of departments, just select them and use the migrate button below.{/t}<br>
			{t}If you want to know what will be done when migrating the selected entries, just use the 'What will be done here' button and you will see a list of changes.{/t}
					

			<br><br>
			{foreach from=$users_to_migrate item=val key=key}

				{if $users_to_migrate.$key.checked}
					<input type='checkbox' name='migrate_{$key}' checked>
					{$users_to_migrate.$key.dn}
					{if $users_to_migrate.$key.after != ""}
						<div class="step2_entry_container_info" id="sol_8">

{t}Current{/t}
<div style='padding-left:20px;'>
<pre>
dn: {$users_to_migrate.$key.dn}
{$users_to_migrate.$key.before}
</pre>
</div>
{t}After migration{/t}
<div style='padding-left:20px;'>
<pre>
dn: {$users_to_migrate.$key.dn}
{$users_to_migrate.$key.after}
</pre>
</div>
						</div>
					{/if}
				{else}
					<input type='checkbox' name='migrate_{$key}'>
					{$users_to_migrate.$key.dn}
				{/if}

				<br>
				

			{/foreach}
			<br>

			<input type='submit' name='users_visible_migrate_refresh' value='{t}Reload list{/t}'>
			<input type='submit' name='users_visible_migrate_migrate' value='{t}Migrate{/t}'>
			<input type='submit' name='users_visible_migrate_whatsdone' value='{t}What will be done here{/t}'>

			<p class='seperator'>&nbsp;</p>	

			<div style='width:100%; text-align:right; padding:5px;'>
				<input type='submit' name='users_visible_migrate_close' value='{t}Close{/t}'>
			</div>
		{else}

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

	
	{/if}
    </div>
</div>