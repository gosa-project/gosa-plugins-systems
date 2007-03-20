<table class='sieve_allof_container'>
	<tr>
    	<td class='sieve_allof_left'>
            {if $Inverse}
                <input type='submit' name='toggle_inverse_{$ID}' value='{t}Not{/t}'>
            {else}
                <input type='submit' name='toggle_inverse_{$ID}' value='{t}-{/t}'>
            {/if}
			<br>
			<b>{t}All of{/t}</b>
		</td>
        <td style='background-color:#BDBDBD ; border: solid 0px #EEEEEE'>
			{$Contents}
        </td>
	</tr>
</table>
