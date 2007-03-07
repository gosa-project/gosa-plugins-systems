
<table cellspacing=0 cellpadding=2 style='background-color:#EEEEDD;width:100%; border: solid 1px #CCCCCC'>


	{if $Expert}
	<tr>
		<td>
			{t}Release date{/t}&nbsp;
			<input type='text' name='vacation_release_{$ID}' value='{$Days}'>
				
		</td>
		<td>
			{t}Receiver{/t}
		</td>
		<td style='width:50%;'>
    		<textarea name='vacation_receiver_{$ID}' style='width:100%;height:20px;'>{$Addresses}</textarea>
		</td>
        <td style='text-align:right; vertical-align:top;'>
            <input type='submit' name='Toggle_Expert_{$ID}' value='{t}Normal mode{/t}'>
        </td>
	</tr>
	<tr>
		<td colspan="4">
		    <textarea name='vacation_reason_{$ID}' style='width:100%;height:60px;'>{$Reason}</textarea>
		</td>
	</tr>
	{else}
	<tr>
		<td>
		    <textarea name='vacation_reason_{$ID}' style='width:100%;height:60px;'>{$Reason}</textarea>
		</td>
		<td width='10%' style='vertical-align:top;'>
            <input type='submit' name='Toggle_Expert_{$ID}' value='{t}Expert mode{/t}'>
		</td>
	</tr>
	{/if}
</table>

