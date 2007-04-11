<h2>Create a new sieve script</h2>
{t}Please enter the name for yout new script below. Script names must be lower case and only characters are allowed.{/t}

<br>
<br>
<p class="seperator">&nbsp;</p>
<br>
<b>{t}Script name{/t}</b> <input type='text' name='NewScriptName' value='{$NewScriptName}'>
<br>
<br>

<div class='seperator' style='border-top:1px solid #999; text-align:right; width:100%; padding-top:10px;'>
   <input type='submit' name='create_script_save' value='{t}Save{/t}' id='create_script_save'>
   &nbsp;
   <input type='submit' name='create_script_cancel' value='{t}Cancel{/t}'>
</div>
<script language="JavaScript" type="text/javascript">
	document.mainform.NewScriptName.focus();
</script>
