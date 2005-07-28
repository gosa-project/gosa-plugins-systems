<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding=4>
<tr>
  <td>
  <div class="contentboxh">
    <p class="contentboxh">
     {t}List of blocklists{/t} {$hint}
    </p>
  </div>
  <div class="contentboxb">
	  {$blocklistshead}
  </div>
  <div style='height:4px;'></div>
  <div class="contentboxb" style="border-top:1px solid #B0B0B0;">
	  {$blocklists}
      <input type=hidden name="edit_helper">
  </div>
  </td>
  <td style="vertical-align:top;">
   <div class="contentboxh">
    <p class="contentboxh"><img src="{$infoimage}" align="right" alt="[i]">{t}Information{/t}</p>
   </div>
   <div class="contentboxb">
   <p class="contentboxb">
    {t}This menu allows you to create, delete and edit selected blocklists. Having a large size of lists, you might prefer the range selectors on top of the select box.{/t}
   </p>
   </div>
   <br>
   <div class="contentboxh">
    <p class="contentboxh"><img src="{$launchimage}" align="right" alt="[F]">{t}Filters{/t}</p>
   </div>
   <div class="contentboxb">
     <table summary="" style="width:100%;border-top:1px solid #B0B0B0;">
      {$alphabet}
     </table>
    <table summary="" style="width:100%;border-top:1px solid #B0B0B0;">
		<tr>
			<td>
	<input type=checkbox name="sendlists" value="1" {$sendlists} onClick="mainform.submit()" title="{t}Select to see send blocklists{/t}">{t}Show send blocklists{/t}<br>
    <input type=checkbox name="receivelists" value="1" {$receivelists} onClick="mainform.submit()" title="{t}Select to see receive blocklists{/t}">{t}Show receive blocklists{/t}<br>
			</td>
		</tr>
	</table>    
<table summary="" style="width:100%;border-top:1px solid #B0B0B0;">
	<tr>
		<td width='20'><img alt=""  src="{$search_image}" align=middle>
		</td>
		<td width='50%'>		
			{t}Display lists matching{/t}
		</td>
	    <td>
			<input type='text' name='regex' maxlength='20' value='{$regex}' title='{t}Regular expression for matching list names{/t}' onChange="mainform.submit()">
		</td>
	</tr>
</table>

<table summary="" style="width:100%;border-top:1px solid #B0B0B0;">
	<tr>
		<td width='20'>
			<img alt="Search" src="{$search_image}" align=middle>
		</td>
		<td width='50%'>	
			{t}Display lists containing{/t}
		</td>
	    <td>
		<input type='text' name='entry' maxlength='20' value='{$entry}' title='{t}Show lists containing entered numbers{/t}' onChange="mainform.submit()">
		</td>
	</tr>
</table>
   {$apply}
   </div>
  </td>
</tr>
</table>

<input type="hidden" name="ignore">
