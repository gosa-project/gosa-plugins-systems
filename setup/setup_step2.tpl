<!-- Div container with PHP module checks -->
<div style='float:left;'> 

	<!-- Set content styles -->	
	<div class='default' style='margin:10px;'>

		<!-- Beginning PHP module check output -->
		<h2 style='background-color:#DDDDDD;border: solid 1px #CCCCCC'>{t}PHP module and extension checks{/t}</h2>
		{foreach from=$basic item=val key=key}
	
			<!-- Entry {$key} -->		
			{if $basic[$key].SOLUTION != "" && !$basic[$key].RESULT}
		
				<!-- Add ability to display info popup -->	
				<div  style='width:300px; cursor:default;'
					onMouseOver="javascript: display_solution('sol_{$key}');"
					onMouseOut="javascript: hide_solution('sol_{$key}');" >
			{else}

				<!-- Normal entry everything is fine -->
				<div  style='width:300px; cursor:default;'>	
			{/if}
			
			<div class='default' style='float:left'>{$basic[$key].NAME}</div>
			<div class='default' style='none;text-align:right; '>

			{if $basic[$key].RESULT}
				<font color='green'>{t}Successfull{/t}</font>
			{else}
				{if $basic[$key].MUST}
					<font color='red'>{t}Failed{/t}</font>
				{else}
					<font color='blue'>{t}Failed{/t}</font>
				{/if}
				{if $basic[$key].SOLUTION != ""}
	
				<!-- Inforamtion popup -->
				<div class='solution_hidden' id='sol_{$key}'>
					{if $basic[$key].MUST}
						<div style="background-image: url('images/error.png'); background-repeat: no-repeat; padding-left: 35px;">
						<b>{t}GOsa will NOT run without fixing this.{/t}</b>
					{else}
						<div style="background-image: url('images/warning.png'); background-repeat: no-repeat; padding-left: 35px;">
						<i>{t}GOsa will run without fixing this.{/t}</i>
					{/if}
					<br>
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


<!-- Div container with PHP config checks -->
<div style='float:left;'>

	<!-- Set content styles -->	
	<div class='default' style='margin:10px;'>

		<!-- Beginning PHP config check output -->
		<h2 style='background-color:#DDDDDD;border: solid 1px #CCCCCC'>{t}PHP setup configuration{/t}</h2>

		{foreach from=$config item=val key=key}

			<!-- Entry {$key} -->
            {if $config[$key].SOLUTION != "" && !$config[$key].RESULT}

                <!-- Add ability to display info popup -->
                <div  style='width:300px; cursor:default;'
                    onMouseOver="javascript: display_solution('sol_config_{$key}');"
                    onMouseOut="javascript: hide_solution('sol_config_{$key}');" >
            {else}

                <!-- Normal entry everything is fine -->
                <div  style='width:300px; cursor:default;'>
            {/if}


			<div class='default' style='float:left'>{$config[$key].NAME}</div>
			<div class='default' style='text-align:right; '>
			{if $config[$key].RESULT}
				<font color='green'>{t}Successfull{/t}</font>
			{else}

				{if $config[$key].MUST}
					<font color='red'>{t}Failed{/t}</font>
				{else}
					<font color='blue'>{t}Failed{/t}</font>
				{/if}

				{if $config[$key].SOLUTION != ""}
	
				<!-- Inforamtion popup -->
				<div class='solution_hidden' id='sol_config_{$key}'>
					{if $config[$key].MUST}
						<div style="background-image: url('images/error.png'); background-repeat: no-repeat; padding-left: 35px;">
						<b>{t}GOsa will NOT run without fixing this.{/t}</b>
					{else}
						<div style="background-image: url('images/warning.png'); background-repeat: no-repeat; padding-left: 35px;">
						<i>{t}GOsa will run without fixing this.{/t}</i>
					{/if}
					<br>
					{$config[$key].SOLUTION}
					</div>
				</div>
				{/if}
			{/if}
			</div>
			</div>
		{/foreach}


		<!-- Beginning PHP config check output -->
		<h2 style='background-color:#DDDDDD;border: solid 1px #CCCCCC'>{t}Configuration file writeable{/t}</h2>
		
		{foreach from=$is_writeable item=val key=key}

			<!-- Entry {$key} -->
            {if $is_writeable[$key].SOLUTION != "" && !$is_writeable[$key].RESULT}

                <!-- Add ability to display info popup -->
                <div  style='width:300px; cursor:default;'
                    onMouseOver="javascript: display_solution('sol_is_writeable{$key}');"
                    onMouseOut="javascript: hide_solution('sol_is_writeable{$key}');" >
            {else}

                <!-- Normal entry everything is fine -->
                <div  style='width:300px; cursor:default;'>
            {/if}


			<div class='default' style='float:left'>{$is_writeable[$key].NAME}</div>
			<div class='default' style='text-align:right; '>
			{if $is_writeable[$key].RESULT}
				<font color='green'>{t}Successfull{/t}</font>
			{else}

				{if $is_writeable[$key].MUST}
					<font color='red'>{t}Failed{/t}</font>
				{else}
					<font color='blue'>{t}Failed{/t}</font>
				{/if}

				{if $is_writeable[$key].SOLUTION != ""}
	
				<!-- Inforamtion popup -->
				<div class='solution_hidden' id='sol_is_writeable{$key}'>
					{if $is_writeable[$key].MUST}
						<div style="background-image: url('images/error.png'); background-repeat: no-repeat; padding-left: 35px;">
						<b>{t}GOsa will NOT run without fixing this.{/t}</b>
					{else}
						<div style="background-image: url('images/warning.png'); background-repeat: no-repeat; padding-left: 35px;">
						<i>{t}GOsa will run without fixing this.{/t}</i>
					{/if}
					<br>
					{$is_writeable[$key].SOLUTION}
					</div>
				</div>
				{/if}
			{/if}
			</div>
			</div>
		{/foreach}


			
	</div>
</div>
<script type="text/javascript">
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
