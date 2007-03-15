
{if $LastErrorCnt != 0}
    <table style='width:100%;'>
        <tr>
            <td style='width:40px; background-color: #FF0000; text-align:center; border: dotted 3px #ff7c1c'>
                <img src='images/warning.png' alt='{t}Error{/t}'>
            </td>
            <td>
{/if}
<table cellspacing=0 cellpadding=2 style='background-color:#EEEEDD;width:100%; border: solid 1px #CCCCCC'>

	{foreach from=$LastError item=val key=key}
		<tr>
			<td colspan=4>
				<font color='red'><b>{$LastError[$key]}</b></font>
			</td>
		</tr>

	{/foreach}
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
			{t}Alternative sender addresses{/t}
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

{if $LastErrorCnt != 0}

            </td>
        </tr>
    </table>
{/if}


