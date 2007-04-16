<div class="contentboxh">
 <p class="contentboxh"><img src="{$launchimage}" align="right" alt="[F]">
	{t}Options{/t}&nbsp;
 </p>
</div>
<div class="contentboxb">

	<table summary="" width="100%" class="contentboxb" style="border-top:1px solid #B0B0B0; padding:0px;">
		<tr>
			<td>
				{t}Export{/t}&nbsp;
				<input type='submit' name='export_gotomass_csv' value='{t}Export{/t}'>
			</td>
			<td style='width:20px;'>
			</td>
			<td>
				{t}Import{/t}&nbsp;
			
				<input type='file' name='mass_file'>

				<input type='submit' name='import_gotomass_csv' value='{t}Import{/t}'>
			</td>
		</tr>
	</table>
</div>
	<table summary="" width="100%" class="contentboxb">
		<tr>
			<td>
				<b>{t}No.{/t}</b>
			</td>
			<td>
				<b>{t}Mac address{/t}</b>
			</td>
			<td>
				<b>{t}Object group{/t}</b>
			</td>
			<td style='text-align:right'>
				<b>{t}Options{/t}</b>
			</td>
		</tr>
	{foreach from=$contents item=val key=key}

		<tr>
			<td>
				{$key+1}
			</td>
			<td>
				<input name='mac_{$key}' type='text' value='{$contents.$key.MAC}'>
			</td>
			<td>
				<select name='og_{$key}' title='{t}Select object group{/t}'>
				 {html_options options=$ogs selected=$contents.$key.OG}
				</select>
			</td>
			<td style='text-align:right'>
				<input type='image' name='remove_{$key}' src='images/edittrash.png'
				 	title='{t}Remove this entry{/t}'>
			</td>
		</tr>

	{/foreach}
		<tr>
			<td>
				&nbsp;-	
			</td>
			<td>
				&nbsp;-	
			</td>
			<td>
				&nbsp;-	
			</td>
			<td style='text-align:right'>
				<input type='submit' name='add_new_entry' value='{t}New entry{/t}'>
			</td>
		</tr>
	</table>
<p>&nbsp;</p>
<div class="contentboxb" style="border-top: 1px solid #B0B0B0; padding:0px;">
 <p class="contentboxb" style='text-align:right'>
	<input type='submit' name='reload_gotomass_data' value='{t}Discard changes{/t}'>
	<input type='submit' name='save_gotomass_changes' value='{t}Apply changes{/t}'>
 </p>
</div>
<input type='hidden' name='gotomasses' value='1'>
