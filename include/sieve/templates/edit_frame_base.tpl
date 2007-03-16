
<table style='width:100%;' cellspacing=0 cellpadding=0>
	<tr>
		<td style='background-color: #CCCCCC; padding:5px;'>
			
			<input type='submit' name='Add_Element' value="{t}Add element{/t}">
			<input type='submit' name='Save_Copy' value="{t}Save copy{/t}">
			<input type='submit' name='Import_Copy' value="{t}Import copy{/t}">

			{if $Mode != "Source-Only"}			
				<input type='submit' name='View_Source' value="{t}View source{/t}">
				<input type='submit' name='View_Structured' value="{t}View structured{/t}">
			{/if}
		</td>
	</tr>
	<tr>
		<td style='background-color: #FFFFFF; border: solid 2px #CCCCCC; '>

			{if $Script_Error != ""}
			<table style='width:100%;' cellspacing=0 cellpadding=0>	
				<tr>	
					<td style='background-color: orange;padding:5px;'>
						<b>{$Script_Error}</b>
					</td>
				</tr>
			</table>
			{/if}


			{if $Mode == "Structured"}
				{$Contents}
			{else}
				<textarea name='script_contents' style='width:100%;height:350px;'>{$Contents}</textarea>
			{/if}

		</td>
	</tr>
</table>
<div class='seperator' style='text-align:right; width:100%;'>
	<input type='submit' name='save_sieve_changes' value='{t}Save{/t}'>
        &nbsp;
    <input type='submit' name='cancel_sieve_changes' value='{t}Cancel{/t}'>
</div>
