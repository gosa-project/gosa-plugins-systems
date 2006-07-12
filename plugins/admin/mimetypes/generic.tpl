<table style="width:100%">
	<tr>
		<td colspan="2">
			<h2><img src='images/list_mime.png' class='center'>&nbsp;{t}Generic{/t}</h2>
		</td>
	</tr>
	<tr>
		<td style='width:50%; vertical-align:top;' >
				
			<table>
				<tr>
					<td>
						{t}Mime type{/t}{$must}
					</td>
					<td>
						<input type="text" name='cn' value="{$cn}" {$cnACL} title='{t}Please enter a name for the mime type here{/t}'>
					</td>
				</tr>
				<tr>
					<td>
						{t}Mime group{/t}
					</td>
					<td>
						<select name='gotoMimeGroup' {$gotoMimeGroupACL} title='{t}Categorize this mime type{/t}'>
							{html_options output=$gotoMimeGroups values=$gotoMimeGroups selected=$gotoMimeGroup}
						</select>
					</td>
				</tr>
				<tr>
					<td>
						{t}Description{/t}
					</td>
					<td>
						<input type="text" name='description' value="{$description}" {$descriptionACL} title='{t}Please sepecify a description{/t}'>
					</td>
				</tr>
				<tr>
				  	<td><LABEL for="base">{t}Base{/t}{$must}</LABEL></td>
				  	<td>
				   		<select size="1" id="base" name="base" title="{t}Choose subtree to place application in{/t}"
							{if $isReleaseMimeType} disabled {/if} {$baseACL}>
							{html_options options=$bases selected=$base_select}
				   		</select>
						{if $baseACL == "" && !$isReleaseMimeType}
							<input type="image" name="chooseBase" src="images/folder.png" class="center" title="{t}Select a base{/t}">
						{else}
							<img src="images/folder_gray.png" class="center" title="{t}Select a base{/t}">
						{/if}
				  </td>
				 </tr>

			</table>
			
		</td>
		<td style="border-left:1px solid #A0A0A0;vertical-align:top;">
			<table summary="">
				<tr>
					<td>
						<LABEL for="picture_file">{t}Icon{/t}</LABEL><br>
						<img src="{$gotoMimeIcon}" border=1 style="width:48px; height:48; background-color:white; vertical-align:bottom;">
					</td>
					<td style="vertical-align:top">
						&nbsp;<br>
						<input type="hidden" name="MAX_FILE_SIZE" value="100000">
						<input name="picture_file" type="file" size="20" maxlength="255" accept="image/*.png" id="picture_file" {$gotoMimeIconACL}>
						<input type="submit" name="update_icon" value="{t}Update{/t}" title="{t}Update mime type icon{/t}" {$gotoMimeIconACL}>
					</td>
				</tr>
			</table>
		</td>
	</tr>
	<tr>
		<td colspan="2">
			<p class='seperator'>&nbsp;</p>
		</td>
	</tr>
	<tr>
		<td colspan="2">
			<h2><img src='images/mime_leftclick.png' class='center'>&nbsp;{t}Left click{/t}</h2>
		</td>
	</tr>
	<tr>
		<td style='width:50%; vertical-align:top;' >
			{t}File patterns{/t}{$must}
			{$gotoMimeFilePatterns}		
			<input type='text'	 name='NewFilePattern'	  value='' title='{t}Please specify a new file pattern{/t}' {$gotoMimeFilePatternACL}>
			<input type='submit' name='AddNewFilePattern' value='{t}Add{/t}' title='{t}Add a new file pattern{/t}' {$gotoMimeFilePatternACL}>
		</td>
		<td style="border-left:1px solid #A0A0A0;vertical-align:top;">
			{t}Applications{/t}
			{$gotoMimeApplications}		
			<input type='text'	 name='NewApplication'	  value='' title='{t}Enter an application name here{/t}' {$gotoMimeApplicationACL}>
			<input type='submit' name='AddNewApplication' value='{t}Add{/t}' title='{t}Add application{/t}' {$gotoMimeApplicationACL}>
		</td>
	</tr>
	<tr>	
		<td colspan="2">
			<p class='seperator'>&nbsp;</p>
		</td>
	</tr>
	<tr>	
		<td colspan="2">
			<h2><img src='images/mime_embedding.png' class='center'>&nbsp;{t}Embedding{/t}</h2>
		</td>
	</tr>
	<tr>
		<td style='width:50%; vertical-align:top;' >
				
			<table>
				<tr>
					<td style='vertical-align:top; '>
						<input type='radio' name='gotoMimeLeftClickAction_IE' value='I' 
							{if $gotoMimeLeftClickAction_I} checked {/if} {$gotoMimeLeftClickActionACL}>
						{t}Show file in embedded viewer{/t}
						<br>

						<input type='radio' name='gotoMimeLeftClickAction_IE' value='E' 
							{if $gotoMimeLeftClickAction_E} checked {/if} {$gotoMimeLeftClickActionACL}>
						{t}Show file in external viewer{/t}
						<br>

						<input type='checkbox' name='gotoMimeLeftClickAction_Q' value='1' 
							{if $gotoMimeLeftClickAction_Q} checked {/if} {$gotoMimeLeftClickActionACL}>
						{t}Ask whether to save to local disk{/t}
					</td>
				</tr>
			</table>
			
		</td>
		<td style="border-left:1px solid #A0A0A0;vertical-align:top;">
			{t}Applications{/t}
			{$gotoMimeEmbeddedApplications}		
			<input type='text'	 name='NewEmbeddedApplication'	  value='' title='{t}Enter an application name here{/t}' {$gotoMimeEmbeddedApplicationACL}>
			<input type='submit' name='AddNewEmbeddedApplication' value='{t}Add{/t}' title='{t}Add application{/t}' {$gotoMimeEmbeddedApplicationACL}>
		</td>
	</tr>
</table>
<input type="hidden" name="MimeGeneric" value="1">
<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
  document.mainform.cn.focus();
  -->
</script>
