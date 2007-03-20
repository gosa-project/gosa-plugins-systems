<table class='sieve_test_container'>
	<tr>
		<td style='width:20px; ; text-align:center;'>	
			{if $DisplayAdd}
				<input type='image' src='images/sieve_add_test.png' name='Add_Test_Object_{$ID}' 
					title='{t}Add this object{/t}' alt='R' class='center'>
			{/if}
			{if $DisplayDel}
				<input type='image' src='images/sieve_del_object.png' name='Remove_Test_Object_{$ID}' 
					title='{t}Remove this object{/t}' alt='R' class='center'>
			{/if}
		</td>
		<td>
			%%OBJECT_CONTENT%%
		</td>
	</tr>
</table>
