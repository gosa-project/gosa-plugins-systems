<table width='100%'  cellspacing=0 cellpadding=0>
	<tr>
    	<td style='	text-align:center; 
					vertical-align: middle; 
					width:45px;
					background-color: #AAAAAA; 
					border: solid 1px #EEEEEE
					'>
            {if $Inverse}
                <input type='submit' name='toggle_inverse_{$ID}' value='{t}Not{/t}'>
            {else}
                <input type='submit' name='toggle_inverse_{$ID}' value='{t}-{/t}'>
            {/if}
			<img alt='' class='center' src='images/select_department.png'>
		</td>
        <td style='background-color:#BDBDBD ; border: solid 1px #EEEEEE'>
			{$Contents}
        </td>
	</tr>
</table>
