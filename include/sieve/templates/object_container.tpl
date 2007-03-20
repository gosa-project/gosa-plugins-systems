<table cellspacing=0 style='width:100%;background-color:#EEEEEE;border: solid 1px #AAAAAA;'>
	<tr>
		<td  style='background-color: #CCCCCC; text-align:center; '>
			&nbsp;	
		</td>
		<td  style='background-color: #CCCCCC; text-align:left; padding:2px;  border-bottom: solid 1px #AAAAAA;'>

			<input type='image' src='images/sieve_move_object_down.png' name='Move_Down_Object_{$ID}' 
				title='{t}Move this object one position down{/t}' alt='{t}Down{/t}' class='center'>
			{t}Move down{/t}
			<input type='image' src='images/sieve_move_object_up.png' name='Move_Up_Object_{$ID}' 
				title='{t}Move this object one position up{/t}' alt='{t}Up{/t}' class='center'>
			{t}Move up{/t}
			<input type='image' src='images/sieve_del_object.png' name='Remove_Object_{$ID}' 
				title='{t}Remove this object{/t}' alt='R' class='center'>
			{t}Remove this element{/t}
			<input type='image' src='images/sieve_add_new_top.png' name='Add_Object_Top_{$ID}'
				alt='{t}Add new{/t}' title='{t}Add a new new object on top of this one.{/t}' class='center'>
			{t}Add a new element above{/t}
			<input type='image' src='images/sieve_add_new_bottom.png' name='Add_Object_Bottom_{$ID}'
				alt='{t}Add new{/t}' title='{t}Add a new new object at the bottom of this one.{/t}' class='center'>	
			{t}Add a new element below{/t}
		</td>
	</tr>
	<tr>
		<td style='width:5px; background-color: #CCCCCC; text-align:center; border-right: solid 1px #AAAAAA;'>	
		</td>
		<td>
			%%OBJECT_CONTENT%%
		</td>
	</tr>
</table>
