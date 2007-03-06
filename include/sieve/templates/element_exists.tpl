<b>{t}Exists{/t}</b>
<table >
    <tr>
        <td style='vertical-align:top;'>
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
