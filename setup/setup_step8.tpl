<div style='float:left;width:690px;'>

<h2>{t}Configuration File{/t}</h2>
	<p>
	 	{t}GOsa setup has collected all data needed to create an initial configuration file.{/t}
	</p>
	<p>
		{t}If you want the setup routine to write the configuration file, use the 'Save configuration' button below. If you prefer to copy the 'gosa.conf' manually to '/etc/gosa/' you can download the configuration file by using the 'Download configuration' button.{/t}
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
	{$CONFIG_DIR}{$gosa_conf_name}
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
		{t}After placing the file under /etc/gosa, place make sure that the webserver user is able to read gosa.conf, while other users shouldn't. You may want to execute these commands to achieve this requirement:{/t}
	</p>
	<pre>
	# chown root.{$webgroup} /etc/gosa/gosa.conf
	# chmod 640 /etc/gosa/gosa.conf
	</pre>
</p>



</div>
<input type='hidden' value='1' name='step8_posted'>
