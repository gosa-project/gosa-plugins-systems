<table width="100%" cellpadding=0 cellspacing=0 style='border: solid 1px #BBB;'>
	<tr>
		<td style='width:22px; background-color: #BBBBBB;'>
		</td>
		<td>
			<table cellspacing=0 style='width:100%;background-color:#EEEEEE;'>
				<tr>
					<td  style='background-color: #235464; text-align:center;'>
						&nbsp;	
					</td>
					<td  style='background-color: #235464; text-align:left; padding:2px;' >
						<input type='image' src='images/sieve_add_new_top.png' name='Add_Object_Top_{$ID}'
							alt='{t}Add new{/t}' title='{t}Add a new new object on top of this one.{/t}' class='center'>&nbsp;	
						<input type='image' src='images/sieve_add_new_bottom.png' name='Add_Object_Bottom_{$ID}'
							alt='{t}Add new{/t}' title='{t}Add a new new object at the bottom of this one.{/t}' class='center'>	
					</td>
				</tr>
				<tr>
					<td style='width:20px; background-color: #235464;'>	
						<input type='image' src='images/sieve_del_object.png' name='Remove_Object_{$ID}' 
							title='{t}Remove this object{/t}' alt='R'>
					</td>
					<td>
						%%OBJECT_CONTENT%%
					</td>
				</tr>
			</table>
		</td>
	</tr>
</table>
