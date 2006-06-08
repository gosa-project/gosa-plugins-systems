<br>
<h2>{t}Time server{/t}</h2>
<br>
<table summary="" style="width:100%">
<tr>
 <td>
	<select style="width:100%;" id="goTimeEntry" name="goTimeSource[]" {$goNtpServerACL}  size=8 multiple>
		{html_options values=$goTimeSource output=$goTimeSource}
		<option disabled>&nbsp;</option>
	</select>
<br>
	<input type="text" name="NewNTPExport"  id="NewNTPExportId">
	<input type="submit"    value="{t}Add{/t}"      name="NewNTPAdd"  {$goNtpServerACL} id="NewNTPAddId">
	<input type="submit"    value="{t}Delete{/t}"   name="DelNTPEnt"  {$goNtpServerACL} id="DelNTPEntId">
</td>
</tr>
</table>

<p class="seperator">&nbsp;</p>
<p>
<div style="width:100%; text-align:right;">
	<input type='submit' name='SaveService' value='{t}Save{/t}'>
	&nbsp; 
	<input type='submit' name='CancelService' value='{t}Cancel{/t}'> 
</div>
</p>
