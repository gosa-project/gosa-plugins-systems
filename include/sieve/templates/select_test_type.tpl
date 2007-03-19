<h2>{t}Select the test type you want to add{/t}</h2>

<b>{t}Available test types{/t}</b>&nbsp;:&nbsp;
<select name='test_type_to_add_{$ID}'>
	{html_options options=$test_types_to_add}
</select>

<p class='seperator'>&nbsp;</p>
<br>
<div class='seperator' style='text-align:right; width:100%;'>
	<input type='submit' name='does_nothing' value='{t}Abort{/t}'>
    	&nbsp;
	<input type='submit' name='add_type' value='{t}Next{/t}'>
</div>
