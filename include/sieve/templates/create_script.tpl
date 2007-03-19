<h2>Create a new sieve script</h2>
{t}Please enter the name for yout new script below.{/t}<br>
{t}Script names must be lower case and only characters are allowed.{/t}

<br>
<br>
{if $Error != ""}
	<font color='red'>{$Error}</font>
	<br>
{/if}
<b>{t}Script name{/t}&nbsp;:&nbsp;</b><input type='text' name='NewScriptName' value='{$NewScriptName}'>

<br>

<div class='seperator' style='text-align:right; width:100%;'>
    <input type='submit' name='create_script_cancel' value='{t}Cancel{/t}'>
        &nbsp;
    <input type='submit' name='create_script_save' value='{t}Next{/t}' id='create_script_save'>
</div>
<script language="JavaScript" type="text/javascript">
	document.mainform.NewScriptName.focus();
</script>
