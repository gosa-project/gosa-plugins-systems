
<table>
	<tr>
		<td colspan=3>
			<b>{t}Address{/t}</b>
			<input type='submit' name='Toggle_Expert_{$ID}' value='{t}Toggle expert mode{/t}'>
		</td>
	</tr>
	{if $Expert}
	<tr>
		<td>
			{t}Match type{/t}
		</td>
		<td>
			<select name='matchtype_{$ID}' title='{t}Boolean value{/t}' onChange='document.mainform.submit();'> 
				{html_options options=$match_types selected=$match_type}
			</select>

		</td>
	</tr>
	<tr>
		<td>
			{t}Address part that should be used{/t}
		</td>
		<td>
			<select name='address_part_{$ID}' title='{t}Boolean value{/t}'> 
				{html_options options=$address_parts selected=$address_part}
			</select>
		</td>
		<td>
			{t}Comparator{/t}
		</td>
		<td>
			<select name='comparator_{$ID}' title='{t}Boolean value{/t}'> 
				{html_options options=$comparators selected=$comparator}
			</select>
		</td>
		{if $match_type == ":count" || $match_type == ":value"}
		<td>
			{t}operator{/t}
		</td>
		<td>
			<select name='operator_{$ID}' title='{t}Boolean value{/t}' onChange='document.mainform.submit();'>
				{html_options options=$operators selected=$operator}
			</select>
		</td>
		{/if}
	</tr>
	<tr>
		<td>
			{t}Address fields to include{/t}
		</td>
		<td>
			<textarea name='keys_{$ID}'>{$keys}</textarea>
		</td>
		<td>
			{t}Values to match for{/t}
		</td>
		<td>
			<textarea name='values_{$ID}'>{$values}</textarea>
		</td>
	</tr>
	{else}
	<tr>
		<td style='vertical-align:top;'>
			{t}If{/t}
			&nbsp;
			<select onChange='document.mainform.submit();' name='matchtype_{$ID}' title='{t}Boolean value{/t}'> 
				{html_options options=$match_types selected=$match_type}
			</select>

			{if $match_type == ":count" || $match_type == ":value"}
			<select name='operator_{$ID}' title='{t}Boolean value{/t}' onChange='document.mainform.submit();'>
				{html_options options=$operators selected=$operator}
			</select>
			{/if}
		</td>
		<td>
			<textarea style='height:30px;' name='keys_{$ID}'>{$keys}</textarea>
			<textarea style='height:30px;' name='values_{$ID}'>{$values}</textarea>
		</td>
	</tr>
	{/if}
</table>
