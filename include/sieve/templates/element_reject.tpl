<table cellspacing=0 width='100%' style='background-color: #ffba4d'>
	<tr>
		<td style='width:100%;'>
			<b>{t}Reject mail{/t}</b>
			&nbsp;
			{if $Multiline}
				{t}This will be a multiline text element{/t}
			{else}
				{t}this will be stored as single string{/t}
			{/if}
		</td>
	</tr>
	<tr>
		<td style='padding-left:20px;'>
			<textarea name='reject_message_{$ID}' style='width:100%;height:40px;' >{$Message}</textarea>
		</td>
	</tr>
</table>
