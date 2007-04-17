<div class="contentboxh">
 <p class="contentboxh"><img src="{$launchimage}" align="right" alt="[F]">
	{t}Options{/t}&nbsp;
 </p>
</div>
<div class="contentboxb" style="margin-bottom:10px">

	<table summary="" width="100%" class="contentboxb" style="border-top:1px solid #B0B0B0; padding:0px;">
		<tr>
			<td>
				{t}Export{/t}&nbsp;
				{if $is_readable}
					<input type='submit' name='export_gotomass_csv' value='{t}Export{/t}'>
				{else}
					<input type='button' name='empty_1' value='{t}Export{/t}' disabled>
				{/if}
			</td>
			<td style='width:20px;'>
			</td>
			<td>
				{t}Import{/t}&nbsp;
		
				{if $is_writeable}	
					<input type='file' name='mass_file'>
					<input type='submit' name='import_gotomass_csv' value='{t}Import{/t}'>
				{else}
					<input type='file' name='epmpty_file' disabled>
					<input type='button' name='empty_2' value='{t}Import{/t}' disabled>
				{/if}
			</td>
		</tr>
	</table>
</div>
	<table summary="" width="100%" class="contentboxb" cellspacing=0>
		<tr>
			<td style='width:40px'>
				<b>{t}No.{/t}</b>
			</td>
			<td style='width:200px'>
				<b>{t}Mac address{/t}</b>
			</td>
			<td>&nbsp;
			</td>
			<td style='width:40%;'>
				<b>{t}Assigned object group{/t}</b>
			</td>
			<td style='text-align:right ; width:20%;'>
				<b>{t}Action{/t}</b>
			</td>
		</tr>
	{foreach from=$contents item=val key=key}

		{if ($key+2)%2 ==0 }
		<tr class='rowxp1'>
		{else}
		<tr class='rowxp0'>
		{/if}
			<td>
				{$key+1}
			</td>

			{if !$contents.$key.VALID_MAC}
			<td  style='background-color: #FF0000;' >
			{else}
			<td>
			{/if}
				<input name='mac_{$key}' type='text' value='{$contents.$key.MAC}' style='width:200px'>
			</td>
			<td>
			</td>
			<td>
				<select name='og_{$key}' title='{t}Select object group{/t}'>
				 {html_options options=$ogs selected=$contents.$key.OG}
				</select>
			</td>
			<td style='text-align:right'>
				{if $is_writeable}
				<input type='image' name='remove_{$key}' src='images/edittrash.png'
				 	title='{t}Remove this entry{/t}'>
				{else}
					&nbsp;	
				{/if}
			</td>
		</tr>

	{/foreach}
		{if $is_writeable}
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
			<td>
				&nbsp;-	
			</td>
			<td style='text-align:right'>
				<input type='submit' name='add_new_entry' value='{t}New entry{/t}'>
			</td>
		</tr>
		{/if}
	</table>
<div class="contentboxb" style="border-top: 1px solid #B0B0B0; padding:0px; margin-top:10px;">
 <p class="contentboxb" style='text-align:right'>
{if $contents_modified}
	{if $is_writeable}
		<input type='submit' name='save_gotomass_changes' value='{t}Apply{/t}'>
	{else}
		<input type='submit' name='save_gotomass_changes' value='{t}Apply{/t}' disabled>
	{/if}
	<input type='submit' name='reload_gotomass_data' value='{t}Cancel{/t}'>
{else}
	<input type='button' name='empty_3' value='{t}Apply{/t}' disabled>
	<input type='button' name='empty_4' value='{t}Cancel{/t}' disabled>
{/if}
 </p>
</div>
<input type='hidden' name='gotomasses' value='1'>

<!-- Display a warning if the content was modified, but not saved yet -->
{if !$contents_modified}
	<input type='hidden' name='ignore' value='1'>
{/if}







