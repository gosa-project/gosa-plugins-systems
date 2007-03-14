<table width='100%'  cellspacing=0 cellpadding=0>
	<tr>
    	<td style='	text-align:center; 
					vertical-align: middle; 
					width:45px;
					background-color: #AAAAAA; 
					border: solid 1px #EEEEEE
					'>
			<b>{t}Any of{/t}</b>
            {if $Inverse}
                <input type='submit' name='toggle_inverse_{$ID}' value='{t}Not{/t}'>
            {else}
                <input type='submit' name='toggle_inverse_{$ID}' value='{t}-{/t}'>
            {/if}
		</td>
        <td style='background-color:#BDBDBD ; border: solid 1px #EEEEEE'>
			{$Contents}
        </td>
	</tr>
</table>
