<div class='default' style='margin:12px; '>
    <div style='float:left; '>
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
            <img  alt='!'  class='center' src='images/info_small.png' title='{t}Please read the FAQ for more informations{/t}'>
        </div>
    </div>
	<div style='float:left;' >
		{if $failed_checks == 0}
			<h2>{t}Schema check was completely successful{/t}</h2>
		{else}
			<h2><img src='images/warning.png' class='center'>
				<font color='red'>{t}The schema check returned the following results{/t}</font></h2>
		{/if}
	<div style='float:left'>
		{foreach from=$checks item=val key=key}
				{if !$checks[$key].STATUS}
				<br>
				<b>{$key}</b>
				{$checks[$key].INFO}
				<br>
					{if $checks[$key].IS_MUST_HAVE}
						<i><font color='red'>{$checks[$key].MSG}</font></i>
					{else}
						<i><font color='orange'>{$checks[$key].MSG}</font></i>
					{/if}
				<br>
				{/if}
		{/foreach}
	</div>
</div>
<input type='hidden' value='1' name='step7_posted'>
