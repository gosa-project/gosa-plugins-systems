<table cellspacing=0 width='100%' style='width:100%;background-color: #d8e1f2; '>
	<tr>
		<td>
			{t}Move mail into folder{/t}
		</td>
		<td style='text-align:right;'>
			{if $User_Mode}
				<input type='submit' name='user_mode_{$ID}' value='{t}Select from list{/t}'>
			{else}
				<input type='submit' name='user_mode_{$ID}' value='{t}Manual selection{/t}'>
			{/if}
		</td>
	</tr>
	<tr>
		<td colspan=2>
			<b>{t}Folder{/t}</b>
			{if $User_Mode}
				<input type='text' value='{$Selected}' name='fileinto_{$ID}' style='width:300px;'>
			{else}
				<select name='fileinto_{$ID}'>
					{html_options values=$Boxes output=$Boxes selected=$Selected}
				</select>
			{/if}
		</td>
	</tr>
</table>
