<div class='default' style='margin:12px;' >
<div style='float:left;'>

<h2>{t}Configuration File{/t}</h2>
	<p>
	 	{t}GOsa setup has collected all data needed to create an initial configuration file.{/t}
	</p>
	<p>
		{$msg1}
	</p>
	<p>
		<b>{t}Automatically write configuration{/t}</b>
	</p>
	<p>	
		{if $writeable}
			{t}Write configuration file{/t}&nbsp;
			<input type='submit' name='saveconf' value='{t}Save configuration{/t}'>

			{if $exists}
			<br>
			<input type='hidden' name='create_backup_visible' value='1'>
			<input {if $create_backup} checked {/if} type='checkbox' value='1' name='create_backup'>&nbsp;
			{t}Create a backup from existing configuration file{/t}
			{/if}

			{if $err_msg != ""}
				<div style='color:red ; font-weight:bold '>{$err_msg}</div>
			{else}
				{if $save_requested}		
					<div style='color:green ; font-weight:bold '>{t}Configuration succesfully written.{/t}</div>
				{/if}
			{/if}

			{if $last_backup_name}
				<br>
				<div style='padding-left:20px;'>{t}Last created backup{/t}: &nbsp;<i>{$last_backup_name}</i></div> 
				<br>
			{/if}


			
			{if $exists}
				<br>
				<br>
				<b>{t}Use GOsa with current configuration file{/t}</b>
				<br>
				<input type='submit' name='use_gosa' value='{t}Use GOsa now{/t}'> 
			{/if}
		{else}		
			<div style='font-weight: bold; color: red;'>{t}Can't write configuration file, please check permissions.{/t}</div>
			<pre>
{$msg_permissions}
	{$CONFIG_DIR} 
	{$CONFIG_DIR}/{$CONFIG_FILE}
			</pre>
		{/if}
	</p>
	<br>
	<p>
		<b>{t}Manually create configuration file{/t}</b>
	</p>
	<p>	
		{t}Download the configuration file{/t}&nbsp;
		<input type='submit' name='getconf' value='{t}Download configuration{/t}'>
	</p>
	<p>	
		{$msg2}
	</p>
	<pre>
	# chown root.{$webgroup} {$CONFIG_DIR}/{$CONFIG_FILE}
	# chmod 640 {$CONFIG_DIR}/{$CONFIG_FILE}
	</pre>



</div>
</div>
<input type='hidden' value='1' name='step8_posted'>
