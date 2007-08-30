<h2>{t}Zones{/t}</h2>
<table summary="" width="100%">
<tr>
	<td style="width:100%;vertical-align:top;">
		{$ZoneList}
		{if $is_createable}
		<input type="submit" name="AddZone" value="{t}Add{/t}">
		{else}
		<input type="button" value="{t}Add{/t}" disabled>
		{/if}
	</td>
</tr>
</table>
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	focus_field('AddZone');
  -->
</script>

<p class="seperator">&nbsp;</p>
<p>
<div style="width:100%; text-align:right;">
    <input type='submit' name='SaveService' value='{t}Save{/t}'>
    &nbsp;
    <input type='submit' name='CancelService' value='{t}Cancel{/t}'>
</div>
</p>
