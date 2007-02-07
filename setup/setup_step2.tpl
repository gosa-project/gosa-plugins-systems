<div style='float:left;'> 
	<div class='default' style='margin:10px;'>
		&nbsp;
		<h2>{t}PHP module and extension checks{/t}</h2>
		{foreach from=$basic item=val key=key}

		<div  style='width:300px;'>

				{if $basic[$key].SOLUTION != "" && !$basic[$key].RESULT}
					<div class='default' style='float:left'
						onMouseOver="javascript: display_solution('sol_{$key}');"
						onMouseOut="javascript: hide_solution('sol_{$key}');" >
				{else}
					<div class='default' style='float:left'>
				{/if}

	
				{$basic[$key].NAME}
			</div>
				{if $basic[$key].SOLUTION != "" && !$basic[$key].RESULT}
					<div style='width:60px; float:left;'
						onMouseOver="javascript: display_solution('sol_{$key}');"
						onMouseOut="javascript: hide_solution('sol_{$key}');"></div>
				{else}
					<div style='width:60px; float:left;'>&nbsp;</div>
				{/if}
				{if $basic[$key].SOLUTION != "" && !$basic[$key].RESULT}
					<div class='default' style='float:none;text-align:right; '
						onMouseOver="javascript: display_solution('sol_{$key}');"
						onMouseOut="javascript: hide_solution('sol_{$key}');" >
				{else}
					<div class='default' style='none;text-align:right; '>
				{/if}
			{if $basic[$key].RESULT}
				<font color='green'>{t}Success{/t}</font>
			{else}
				<font color='red'>{t}Failed{/t}</font>
				{if $basic[$key].SOLUTION != ""}
					<div class='solution_hidden' id='sol_{$key}'>
						<div style="background-image: url('images/error.png'); background-repeat: no-repeat; padding-left: 35px;">
						{$basic[$key].SOLUTION}
						</div>
					</div>
				{/if}

			{/if}
			</div>
		</div>
		{/foreach}

	</div>
</div>
<div class='default' style='margin:10px; float:left;'>
	&nbsp;
	<h2>{t}PHP setup configuration{/t}</h2>
	{foreach from=$config item=val key=key}
		<div class='default' style='float:left'>
			{$config[$key].NAME}
		</div>
		<div style='width:60px; float:left;'>&nbsp;</div>
		<div class='default' style='text-align:right; '>
		{if $config[$key].RESULT}
			<font color='green'>{t}Success{/t}</font>
		{else}
			<font color='red'>{t}Failed{/t}</font>
		{/if}
		</div>
	{/foreach}
</div>
<script language='javascript'>
{literal}
	function display_solution(str){
		obj = document.getElementById(str);
		obj.className='solution_visible';
	}
	function hide_solution(str){
		obj = document.getElementById(str);
		obj.className='solution_hidden';
	}

{/literal}
</script>
