{if $LastErrorCnt != 0}
	<table style='width:100%;'> 
		<tr>
			<td style='width:40px; background-color: #FF0000; text-align:center; border: dotted 3px #ff7c1c'>
				<img src='images/warning.png' alt='{t}Error{/t}'>
			</td>
			<td>
{/if}

<table cellspacing=0 width='100%' style='background-color: #d1ffcc'>
	<tr>
		<td>
			<b>{t}Redirect{/t}</b>
			{t}Redirect mail to following recipients{/t}
		</td>
	</tr>

	{foreach from=$LastError item=val key=key}
        <tr>
            <td colspan=4>
                <font color='red'><b>{$LastError[$key]}</b></font>
            </td>
        </tr>

    {/foreach}

	<tr>
		<td style='padding-left:20px;'>
			<textarea name='redirect_to_{$ID}' style='width:100%; height:30px;'>{$Destinations}</textarea>
		</td>
	</tr>
</table>

{if $LastErrorCnt != 0}

			</td>
		</tr>
	</table>
{/if}
