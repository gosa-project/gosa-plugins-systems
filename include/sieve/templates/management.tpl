<h2>{t}Sieve script management{/t}</h2>
<!--
{if $uattrib_empty}
		
	<font color='red'><b>{t}Connection to sieve could not be established, the authentification attribute is empty.{/t}</b></font><br>
	{t}Please verfiy that the attributes uid and mail are not empty and try again.{/t}
	<br>
	<br>

{elseif $Sieve_Error != ""}

	<font color='red'><b>{t}Connection to sieve could not be established.{/t}</b></font><br>
	{$Sieve_Error}
	<br>
	{t}Possibly the sieve account was not created yet.{/t}
	<br>
	<br>
{/if}
	{t}Be careful. All your changes will be saved directly to sieve, if you use the save button below.{/t}
-->
	<br>
	<input type='submit' name='create_new_script' value='{t}Create new script{/t}'>
	{$List}
	<p style="text-align:right">
		<input type=submit name="sieve_finish" style="width:80px" value="{t}Ok{/t}">
		&nbsp;
		<input type=submit name="sieve_cancel" value="{t}Cancel{/t}">
	</p>
