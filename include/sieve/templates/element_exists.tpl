<table cellspacing=0 cellpadding=2 style='background-color:#EEEEDD;width:100%; border: solid 1px #CCCCCC'>
    <tr>
        <td style='vertical-align:top;'>
 <b>{t}Exists{/t}</b>
            {if $LastError != ""}
                <font color='red'>{$LastError}</font>
                <br>
            {/if}

            {if $Inverse}
                {t}If header attribute does not exists{/t}
            {else}
                {t}If header attribute(s) exists{/t}
            {/if}
		</td>
		<td>
            <textarea style='width:300px;height:30px;' name='Values_{$ID}'>{$Values}</textarea>
		</td>
		<td>
			<input type='submit'>
        </td>
    </tr>
</table>
