
<table>
	<tr>
		<td colspan=3>
			<b>{t}Envelope{/t}</b>
		</td>
	</tr>
	<tr>
		<td>
			{t}Comparator{/t}
		</td>
		<td>
			<select name='comparator_{$ID}' title='{t}Boolean value{/t}'> 
				{html_options options=$comparators selected=$comparator}
			</select>
		</td>
		<td>
			{t}Match type{/t}
		</td>
		<td>
			<select name='matchtype_{$ID}' title='{t}Boolean value{/t}'> 
				{html_options options=$match_types selected=$match_type}
			</select>

		</td>
		<td>
			{t}operator{/t}
		</td>
		<td>
			<select name='operator_{$ID}' title='{t}Boolean value{/t}'>
				{html_options options=$operators selected=$operator}
			</select>
		</td>
	</tr>
	<tr>
		<td>
			{t}Address fields{/t}
		</td>
		<td>
			<textarea name='keys_{$ID}'>{$keys}</textarea>
		</td>
		<td>
			{t}Match values{/t}
		</td>
		<td>
			<textarea name='values_{$ID}'>{$values}</textarea>
		</td>
	</tr>
</table>
