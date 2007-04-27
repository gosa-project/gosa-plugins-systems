<!-- 
	Div container with PHP module checks 
-->
<div style='float:left;width:50%;'> 

	<!-- Set content styles -->	
	<div class='default' style='margin:10px;'>
		<a href='?info' target='_blank'>{t}Show PHP information{/t}</a>

		<!-- Beginning PHP module check output -->
		<h2 class='step2_container_title'>{t}PHP module and extension checks{/t}</h2>
		{foreach from=$basic item=val key=key}
	
			<!-- Entry {$key} -->		
			{if $basic[$key].SOLUTION != "" && !$basic[$key].RESULT}
		
				<!-- Add ability to display info popup -->	
				<div class='step2_entry_container_info'
					onMouseOver="javascript: display_solution('sol_{$key}');"
					onMouseOut="javascript: hide_solution('sol_{$key}');" >
			{else}

				<!-- Normal entry everything is fine -->
				<div class='step2_entry_container'>	
			{/if}
			
			<div class='step2_entry_name'>{$basic[$key].NAME}</div>
			<div class='step2_entry_status'>

			{if $basic[$key].RESULT}
				<div class='step2_successful'>{t}Ok{/t}</div>
			{else}
				{if $basic[$key].MUST}
					<div class='step2_failed'>{t}Error{/t}</div>
				{else}
					<div class='step2_warning'>{t}Warning{/t}</div>
				{/if}
				{if $basic[$key].SOLUTION != ""}
	
				<!-- Inforamtion popup -->
				<div class='solution_visible' id='sol_{$key}'>
					{if $basic[$key].MUST}
						<div class='step2_failed_text' style="background-image: url('images/small_error.png')">
						{$basic[$key].SOLUTION}
						<b>{t}GOsa will NOT run without fixing this.{/t}</b>
					{else}
						<div class="step2_warning_text" style="background-image: url('images/small_warning.png')">
						{$basic[$key].SOLUTION}
						<b>{t}GOsa will run without fixing this.{/t}</b>
					{/if}
					</div>
				</div>
				{/if}
			{/if}
				</div>
			</div>
		{/foreach}
	</div>
</div>


<!-- 
	Div container with PHP config checks 
-->
<div style='width:50%; float:left;'>

	<!-- Set content styles -->	
	<div class='default' style='margin:10px;'>

		<!-- Beginning PHP config check output -->
		<h2 class='step2_container_title'>{t}PHP setup configuration{/t}</h2>

		{foreach from=$config item=val key=key}

			<!-- Entry {$key} -->
            {if $config[$key].SOLUTION != "" && !$config[$key].RESULT}

                <!-- Add ability to display info popup -->
                <div class='step2_entry_container_info'
                    onMouseOver="javascript: display_solution('sol_config_{$key}');"
                    onMouseOut="javascript: hide_solution('sol_config_{$key}');" >
            {else}

                <!-- Normal entry everything is fine -->
                <div class='step2_entry_container'>
            {/if}


			<div class='step2_entry_name'>{$config[$key].NAME}</div>
			<div class='step2_entry_status'>
			{if $config[$key].RESULT}
				<div class='step2_successful'>{t}Ok{/t}</div>
			{else}

				{if $config[$key].MUST}
					<div class='step2_failed'>{t}Error{/t}</div>
				{else}
					<div class='step2_warning'>{t}Warning{/t}</div>
				{/if}

				{if $config[$key].SOLUTION != ""}
	
				<!-- Inforamtion popup -->
				<div class='solution_visible' id='sol_config_{$key}'>
					{if $config[$key].MUST}
						<div class='step2_failed_text' style="background-image: url('images/small_error.png')">
						{$config[$key].SOLUTION}
						<b>{t}GOsa will NOT run without fixing this.{/t}</b>
					{else}
						<div class='step2_warning_text' style="background-image: url('images/small_warning.png')">
						{$config[$key].SOLUTION}
						<b>{t}GOsa will run without fixing this.{/t}</b>
					{/if}
					</div>
				</div>
				{/if}
			{/if}
			</div>
			</div>
		{/foreach}


<!-- 
	Beginning PHP config check output 
-->
		<h2 class='step2_container_title'>{t}Configuration file writeable{/t}</h2>
		
		{foreach from=$is_writeable item=val key=key}

			<!-- Entry {$key} -->
            {if $is_writeable[$key].SOLUTION != "" && !$is_writeable[$key].RESULT}

                <!-- Add ability to display info popup -->
                <div class='step2_entry_container_info'
                    onMouseOver="javascript: display_solution('sol_is_writeable{$key}');"
                    onMouseOut="javascript: hide_solution('sol_is_writeable{$key}');" >
            {else}

                <!-- Normal entry everything is fine -->
                <div class='step2_entry_container'>
            {/if}


			<div class='step2_entry_name'>{$is_writeable[$key].NAME}</div>
			<div class='step2_entry_status'>
			{if $is_writeable[$key].RESULT}
				<div class='step2_successful'>{t}Ok{/t}</div>
			{else}

				{if $is_writeable[$key].MUST}
					<div class='step2_failed'>{t}Error{/t}</div>
				{else}
					<div class='step2_warning'>{t}Warning{/t}</div>
				{/if}

				{if $is_writeable[$key].SOLUTION != ""}
	
				<!-- Inforamtion popup -->
				<div class='solution_visible' id='sol_is_writeable{$key}'>
					{if $is_writeable[$key].MUST}
						<div class='step2_failed_text' style="background-image: url('images/small_error.png')">
						{$is_writeable[$key].SOLUTION}
						<b>{t}GOsa will NOT run without fixing this.{/t}</b>
					{else}
						<div class='step2_warning_text' style="background-image: url('images/small_warning.png')">
						{$is_writeable[$key].SOLUTION}
						<b>{t}GOsa will run without fixing this.{/t}</b>
					{/if}
					</div>
				</div>
				{/if}
			{/if}
			</div>
			</div>
		{/foreach}
	</div>
</div>

<div style="clear:both"></div>
