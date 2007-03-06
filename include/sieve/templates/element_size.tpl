<b>{t}Size{/t}</b>
<table>
	<tr>
		<td>
			{if $LastError != ""}
				<font color='red'>{$LastError}</font>
				<br>
			{/if}			
		
			{if $Inverse}
				{t}If size is{/t} <b>{t}not{/t}</b>
			{else}
				{t}If size is{/t}
			{/if}
			<select name='Match_type_{$ID}' title='{t}Select match type{/t}'>
				{html_options options=$Match_types selected=$Match_type}
			</select>
			<input type='text' name='Value_{$ID}' value='{$Value}'>
			<select name='Value_Unit_{$ID}' title='{t}Select value unit{/t}'>
				{html_options options=$Units selected=$Value_Unit}
			</select>
			<input type='submit'>
		</td>
	</tr>
</table>
