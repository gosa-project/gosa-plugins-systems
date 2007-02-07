<div style='float:left;'> 
	<div class='default' style='margin:10px;'>
		<h2>{t}PHP module and extension checks{/t}</h2>
		{foreach from=$basic item=val key=key}
			<div class='default' style='float:left'>
				{$basic[$key].NAME}
			</div>
			<div class='default' style='text-align:right; '>
			{if $basic[$key].RESULT}
				<font color='green'>{t}Success{/t}</font>
			{else}
				<font color='red'>{t}Failed{/t}</font>
			{/if}
			</div>
		{/foreach}
	</div>
</div>
<div>
	<div class='default' style='margin:10px; float:left;'>
		<h2>{t}PHP setup configuration{/t}</h2>
		{foreach from=$config item=val key=key}
			<div class='default' style='float:left'>
				{$config[$key].NAME}
			</div>
			<div class='default' style='text-align:right; '>
			{if $config[$key].RESULT}
				<font color='green'>{t}Success{/t}</font>
			{else}
				<font color='red'>{t}Failed{/t}</font>
			{/if}
			</div>
		{/foreach}
	</div>
</div>
