<h2>
	<img class="center" alt="" src="images/fai_script.png" align="middle" title="{t}Objects{/t}">&nbsp;
	{t}List of scheduled tasts{/t}
</h2>
<table width="100%" summary="" style="border:1px solid #B0B0B0; " cellspacing=0 cellpadding=0>
	<tr>
		<td>
			{$divlist}
		</td>
	</tr>
</table>


<input type='hidden' name='gotomasses' value='1'>

<!-- Display a warning if the content was modified, but not saved yet -->
{if !$contents_modified}
	<input type='hidden' name='ignore' value='1'>
{/if}
