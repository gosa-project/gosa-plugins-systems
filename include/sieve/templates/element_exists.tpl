<table class='sieve_test_case'>
    <tr>
        <td style='vertical-align:top; width:200px;'>
            {if $LastError != ""}
                <font color='red'>{$LastError}</font>
                <br>
            {/if}
            {if $Inverse}
                <input type='submit' name='toggle_inverse_{$ID}' value='{t}Not{/t}'>
            {else}
                <input type='submit' name='toggle_inverse_{$ID}' value='{t}-{/t}'>
            {/if}

            {t}If header attribute(s) exists{/t}
		</td>
		<td>
            <textarea style='width:99%;height:20px;' name='Values_{$ID}'>{$Values}</textarea>
		</td>
    </tr>
</table>
