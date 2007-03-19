<table cellspacing=0 style='width:100%;background-color:#EEEEEE;border: solid 1px #BBB;346575;'>
	<tr>
		<td  style='background-color: #235464; text-align:center;'>
			&nbsp;	
		</td>
		<td  style='background-color: #235464; text-align:left; padding:2px;' >
			<input type='image' src='images/sieve_del_object.png' name='Remove_Object_{$ID}' 
				title='{t}Remove this object{/t}' alt='R' class='center'>
			<input type='image' src='images/sieve_add_new_top.png' name='Add_Object_Top_{$ID}'
				alt='{t}Add new{/t}' title='{t}Add a new new object on top of this one.{/t}' class='center'>
			<input type='image' src='images/sieve_add_new_bottom.png' name='Add_Object_Bottom_{$ID}'
				alt='{t}Add new{/t}' title='{t}Add a new new object at the bottom of this one.{/t}' class='center'>	
		</td>
	</tr>
	<tr>
		<td style='width:20px; background-color: #235464; text-align:center;'>	
			<input type='image' src='images/sieve_move_object_up.png' name='Move_Up_Object_{$ID}' 
				title='{t}Move this object one position up{/t}' alt='{t}Up{/t}'>
			<input type='image' src='images/sieve_move_object_down.png' name='Move_Down_Object_{$ID}' 
				title='{t}Move this object one position down{/t}' alt='{t}Down{/t}'>
		</td>
		<td>
			%%OBJECT_CONTENT%%
		</td>
	</tr>
</table>
