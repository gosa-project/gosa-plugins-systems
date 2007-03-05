<b>{t}Address{/t}</b>
<br>

{t}Comparator{/t}
<select name='comparator_{$ID}' title='{t}Boolean value{/t}'> 
	{html_options options=$c_values selected=$c_selected}
</select>
<br>

{t}Match type{/t}
<select name='matchtype_{$ID}' title='{t}Boolean value{/t}'> 
	{html_options values=$match_types output=$match_types selected=$match_type}
</select>
<br>

{if $match_type == ":value"}
	{t}Match type operator{/t}
	<select name='matchtypeoperator_{$ID}' title='{t}Boolean value{/t}'>
		{html_options values=$match_type_values output=$match_type_values selected=$match_type_value}
	</select>
<br>

	<textarea name='keys_{$ID}'>{$keys}</textarea>
<br>
	<textarea name='values_{$ID}'>{$values}</textarea>
	
{/if}


