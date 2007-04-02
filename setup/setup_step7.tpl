    <div style='float:left; width:600px;'>
        <br>
        <b>{t}Validate schema when login into GOsa?{/t}</b>
    </div>

	<div class='step4_container'>
        <div class='step4_name'>
            {t}Enable schema validation{/t}
        </div>
        <div class='step4_value'>
			<select name='enable_schema_check'>
				{html_options options=$bool selected=$enable_schema_check}
            </select>
        </div>
        <div class='step4_status'>
            {t}Infos in FAQ{/t}&nbsp;
            <img class='center' src='images/info_small.png' title='{t}Please read the FAQ for more informations{/t}'>
        </div>
    </div>
	<div style='float:left'>

		{if $failed_checks == 0}
			<h2>{t}Schema check was completely successful{/t}</h2>
		{else}
			<h2>{t}The schema check returned the following results{/t}</h2>
		{/if}

		<input {if $display_type == "failed"} checked  {/if} onClick='document.mainform.submit();' 
			type='radio' name='display_type' value='failed'>{t}Only display failed tests{/t}<br>
		<input {if $display_type == "checked"} checked {/if} onClick='document.mainform.submit();' 
			type='radio' name='display_type' value='checked'>{t}Display checked object classes{/t}<br>
		<input {if $display_type == "all"} checked {/if} onClick='document.mainform.submit();' 
			type='radio' name='display_type' value='all'>{t}Display all objectClasses also not checked but available classes{/t}

	<div style='float:left'>
		{foreach from=$checks item=val key=key}
			{if (	$display_type == "failed" && !$checks[$key].STATUS) || 
					$display_type == "checked" || 
					$display_type == "all"}

			<br>
				<b>{$key}</b>
				{$checks[$key].INFO}
				<br>
				{if !$checks[$key].STATUS}
					{if $checks[$key].IS_MUST_HAVE}
						<i><font color='red'>{$checks[$key].MSG}</font></i>
					{else}
						<i><font color='orange'>{$checks[$key].MSG}</font></i>
					{/if}
				{else}
					<i><font color='green'>{$checks[$key].MSG}</font></i>
				{/if}
			{/if}
		{/foreach}
	</div>

	{if $display_type == "all"}
		<div style='float:left'>
			<h2>{t}All available objectClasses{/t}</h2>
			{foreach from=$not_checked item=val key=key}
				<b>{$key}</b> &nbsp; {$val}<br>
			{/foreach}
		</div>
	{/if}
</div>
<input type='hidden' value='1' name='step7_posted'>
