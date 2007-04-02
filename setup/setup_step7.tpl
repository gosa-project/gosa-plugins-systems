
<input {if $display_type == "failed"} checked  {/if} onClick='document.mainform.submit();' 
	type='radio' name='display_type' value='failed'>{t}Only display failed tests{/t}<br>
<input {if $display_type == "checked"} checked {/if} onClick='document.mainform.submit();' 
	type='radio' name='display_type' value='checked'>{t}Display checked object classes{/t}<br>
<input {if $display_type == "all"} checked {/if} onClick='document.mainform.submit();' 
	type='radio' name='display_type' value='all'>{t}Display all objectClasses also not checked but available classes{/t}


{if $failed_checks == 0}
	<h2>{t}Schema check was completely successful{/t}</h2>
{else}
	<h2>{t}The schema check returned the following results{/t}</h2>
{/if}

	<div style='float:left'>
{foreach from=$checks item=val key=key}
	{if (	$display_type == "failed" && !$checks[$key].STATUS) || 
			$display_type == "checked" || 
			$display_type == "all"}

	<br>
		<b>{$key}</b>
		{$checks[$key].INFO}
		<br>
		<i>
		{if !$checks[$key].STATUS}
			{if $checks[$key].IS_MUST_HAVE}
				<font color='red'>{$checks[$key].MSG}</font>
			{else}
				<font color='orange'>{$checks[$key].MSG}</font>
			{/if}
		{else}
			<font color='green'>{$checks[$key].MSG}</font>
		{/if}
		</i>
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

<input type='hidden' value='1' name='step7_posted'>
