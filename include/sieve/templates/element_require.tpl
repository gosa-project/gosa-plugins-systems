{if $LastErrorCnt != 0}
    <table style='width:100%;'>
        <tr>
            <td style='width:40px; background-color: #FF0000; text-align:center; border: dotted 3px #ff7c1c'>
                <img src='images/warning.png' alt='{t}Error{/t}'>
            </td>
            <td>
{/if}

<table cellspacing=0 style='width:100%;background-color:#e4ffc9;'>
	
	{foreach from=$LastError item=val key=key}
		<tr>
			<td colspan=4>
				<font color='red'><b>{$LastError[$key]}</b></font>
			</td>
		</tr>

	{/foreach}
	<tr>
		<td style=''>
			<b>{t}Require{/t}</b>
		</td>
	</tr>
	<tr>
		<td style='padding-left:20px; height:50px;'>
			<input type='text'  name='require_{$ID}' style='width:100%' value='{$Require}'>
		</td>
	</tr>
</table>

{if $LastErrorCnt != 0}

            </td>
        </tr>
    </table>
{/if}

