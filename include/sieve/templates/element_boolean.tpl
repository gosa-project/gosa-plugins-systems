<table cellspacing=0 cellpadding=2 style='background-color:#DDDDFF;width:100%; border: solid 1px #CCCCCC'>
	<tr>
		<td>
			<b>{t}Bool{/t}</b>
			<select name='boolean_{$ID}' title='{t}Boolean value{/t}'> 
				{html_options options=$values selected=$selected}
			</select>
			<input type='submit' value='{t}update{/t}'>
			<br>
		</td>
	</tr>
</table>
