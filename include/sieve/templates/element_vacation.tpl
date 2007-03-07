
<table cellspacing=0 cellpadding=2 style='background-color:#EEEEDD;width:100%; border: solid 1px #CCCCCC'>


	{if $Expert}
	<tr>
		<td style='width:20%'>	
			<b>{t}Vacation Message{/t}</b>
		</td>	
        <td style='text-align:right; vertical-align:top;'>
            <input type='submit' name='Toggle_Expert_{$ID}' value='{t}Normal mode{/t}'>
        </td>
	</tr>
	<tr>
		<td >
			{t}Release date{/t}&nbsp;
		</td>
		<td>
			<input type='text' name='vacation_release_{$ID}' value='{$Days}'>
			&nbsp;{t}days{/t}
		</td>
	</tr>
	<tr>
		<td>
			{t}Receiver{/t}
		</td>
		<td>
    		<textarea name='vacation_receiver_{$ID}' style='width:100%;height:20px;'>{$Addresses}</textarea>
		</td>
	</tr>
	<tr>
		<td colspan="2">
		    <textarea name='vacation_reason_{$ID}' style='width:100%;height:60px;'>{$Reason}</textarea>
		</td>
	</tr>
	{else}
	<tr>
		<td>	
			<b>{t}Vacation Message{/t}</b>
		</td>	
		<td width='10%' style='vertical-align:top;'>
            <input type='submit' name='Toggle_Expert_{$ID}' value='{t}Expert mode{/t}'>
		</td>
	</tr>
	<tr>
		<td colspan=2>
		    <textarea name='vacation_reason_{$ID}' style='width:100%;height:60px;'>{$Reason}</textarea>
		</td>
	</tr>
	{/if}
</table>

