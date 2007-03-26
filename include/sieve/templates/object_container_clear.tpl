<table class='object_container_container'> 
	<tr>
		<td class='object_container_cell_top_left'>
			&nbsp;	
		</td>
		<td class='object_container_cell_top_right'>
            <input type='image' src='images/sieve_move_object_down.png' name='Move_Down_Object_{$ID}'
                title='{t}Move this object one position down{/t}' alt='{t}Down{/t}' class='center'>
            {t}Move down{/t}
            <input type='image' src='images/sieve_move_object_up.png' name='Move_Up_Object_{$ID}'
                title='{t}Move this object one position up{/t}' alt='{t}Up{/t}' class='center'>
            {t}Move up{/t}
            <input type='image' src='images/sieve_del_object.png' name='Remove_Object_{$ID}'
                title='{t}Remove this object{/t}' alt='R' class='center'>
            {t}Remove this element{/t}
		</td>
    </tr>
    <tr>
		<td class='object_container_cell_bottom_left'>
        </td>
        <td>
            %%OBJECT_CONTENT%%
        </td>
    </tr>
</table>
