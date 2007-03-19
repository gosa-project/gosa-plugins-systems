<table cellspacing=0 style='width:100%;background-color:d8d8ff;'>
	<tr>
		<td>
			<b>{t}Comment{/t}</b>
		</td>
		<td style='text-align: right;'>
			{if $Small}
				<input type='submit' name='toggle_small_{$ID}' value='{t}Edit{/t}'> 	
			{else}
				<input type='submit' name='toggle_small_{$ID}' value='{t}Close{/t}'> 	
			{/if}
		</td>
	</tr>
	<tr>
		<td style='padding-left:20px;' colspan=2>
		{if $Small}
			{$Comment}
		{else}
			<textarea  name='comment_{$ID}' style='width:100%;height:80px'>{$Comment}</textarea>
		{/if}
		</td>
	</tr>
</table>
