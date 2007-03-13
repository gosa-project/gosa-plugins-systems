

{if $LastError != ""}
	<table style='width:100%;'> 
		<tr>
			<td style='width:40px; background-color: #FF0000; text-align:center; border: dotted 3px #ff7c1c'>
				<img src='images/warning.png' alt='{t}Error{/t}'>
			</td>
			<td>
{/if}

<table cellspacing=0 cellpadding=2 style='background-color:#EEEEDD;width:100%; border: solid 1px #CCCCCC'>
	{if $Expert}
	{if $Expert}
    	{if $LastError != ""}
        	<tr>
				<td colspan=4>
		            <font color='red'><b>{$LastError}</b></font>
				</td>
			</tr>

        {/if}

	<tr>
		<td>
			{t}Match type{/t}
		</td>
		<td>
			<select name='matchtype_{$ID}' title='{t}Boolean value{/t}' onChange='document.mainform.submit();'> 
				{html_options options=$match_types selected=$match_type}
			</select>

		</td>
		<td>
			{t}Invert test{/t}?
		</td>
		<td>
			{if $Inverse}
				<input type='submit' name='toggle_inverse_{$ID}' value='{t}Yes{/t}'>
			{else}
				<input type='submit' name='toggle_inverse_{$ID}' value='{t}No{/t}'>
			{/if}
		</td>
		<td style='text-align:right; vertical-align:top;'>
			<input type='submit' name='Toggle_Expert_{$ID}' value='{t}Normal mode{/t}'>
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
		{if $match_type == ":count" || $match_type == ":value"}
		<td>
			{t}operator{/t}
		</td>
		<td>
			<select name='operator_{$ID}' title='{t}Boolean value{/t}' onChange='document.mainform.submit();'>
				{html_options options=$operators selected=$operator}
			</select>
		</td>
		{else}
		<td style='text-align:right;'>
			&nbsp;
		</td>
		{/if}
		<td style='text-align:right;'>
			&nbsp;
		</td>
	</tr>
	<tr>
		<td>
			{t}Address fields to include{/t}
		</td>
		<td>
			<textarea style='width:95%;height:50px;' name='keys_{$ID}'>{$keys}</textarea>
		</td>
		<td>
			{t}Values to match for{/t}
		</td>
		<td>
			<textarea style='width:95%;height:50px;' name='values_{$ID}'>{$values}</textarea>
		</td>
	</tr>
	{else}
    	{if $LastError != ""}
        	<tr>
				<td colspan=4>
		            <font color='red'><b>{$LastError}</b></font>
				</td>
			</tr>

        {/if}
	<tr>
		<td style='vertical-align:top;'>
			{t}If{/t}
			&nbsp;
			<b>{t}Envelope{/t}</b>
		
			{if $Inverse}
				<input type='submit' name='toggle_inverse_{$ID}' value='{t}Not{/t}'>
			{else}
				<input type='submit' name='toggle_inverse_{$ID}' value='{t}-{/t}'>
			{/if}
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
			<textarea style='width:45%;height:20px;' name='keys_{$ID}'>{$keys}</textarea>
			<textarea style='width:45%;height:20px;' name='values_{$ID}'>{$values}</textarea>
		</td>
		<td style='text-align:right; vertical-align:top;'>
			<input type='submit' name='Toggle_Expert_{$ID}' value='{t}Expert mode{/t}'>
		</td>
	</tr>
	{/if}
</table>
{if $LastError != ""}

			</td>
		</tr>
	</table>
{/if}
