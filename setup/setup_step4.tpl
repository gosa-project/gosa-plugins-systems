
<h2>{t}ldap connection{/t}</h2>

<div style='width:100%;'>
{t}Please enter the server URI to allow the GOsa setup to connect your LDAP server (Example: ldap://your.server:389).{/t}
<br>
{t}Use the 'Test' button to try to connect with the given values. The status will be shown on the bottom of the page.{/t}
<br>

</div>
<div class='default' style='margin:12px; '>
<div style=';float:left;width:100%;'>

	{if $resolve_user}

	<div style='float:left; width:100%;margin-bottom:20px;border: solid 1px #CCC;'>
		<input type='text' value='{$resolve_filter}' name='resolve_filter'>
		<input type='submit' value='{t}Search{/t}' name='resolve_search'>
		<input type='submit' value='{t}Use selected user{/t}' name='use_selected_user'>
		<input type='submit' value='{t}Cancel{/t}' name='resolve_user'>
		<select name='admin_to_use' size=6 style="width:100%;background-color:#DDD;">				
			{html_options options=$resolved_users selected=$admin}
		</select>
	</div>		

	{else}

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
			<input type='submit' value='{t}Test{/t}' style='width:100%;'>
		</div>
	</div>
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
			{if $resolve_user}
				<input type='submit' name='resolve_user' value='{t}Cancel{/t}' style='width:100%;' >
			{else}
				<input type='submit' name='resolve_user' value='{t}Select user{/t}' style='width:100%;' >
			{/if}
		</div>
	</div>


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
	<div class='step4_container'>
		<div class='step4_name'>
		</div>
		<div class='step4_value'>
		</div>
		<div class='step4_status'>
			<input type='submit' value='{t}Try connect{/t}' style='width:100%;'>
		</div>
	</div>
    <div class='step4_container'>
        <div class='step4_name'>
            {t}Use tls connection{/t}
        </div>
        <div class='step4_value'>
            <select name="tls" size="1" title="">
			{html_options options=$bool selected=$tls}
            </select>
        </div>
        <div class='step4_status'>
            {t}Infos in FAQ{/t}&nbsp;
            <img class='center' alt='!' src='images/info_small.png' title='{t}Please read the FAQ for more informations{/t}'>
        </div>
    </div>
	{/if}
    <div class='step4_container' style='height:45px;'>
        <div class='step4_name' style='height:45px;'>
            {t}Status{/t}
        </div>
        <div class='step4_value' style='height:45px;'>
			{$connection_status}
        </div>
        <div class='step4_status' style='height:45px;'>
        </div>
    </div>
</div>
</div>
