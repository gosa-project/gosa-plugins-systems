<div class='default' style='margin:12px; '>  

	<h2>
		<input type='checkbox' name='use_gosa_announce' value='1' class='center'>&nbsp;{t}Subscribe to the gosa-announce mailinglist{/t}
	</h2>

	{t}When checking this option, GOsa will try to connect http://oss.gonicus.de in order to subscribe you to the gosa-announce mailing list. You've to confirm this by mail.{/t}

	<table>	
		<tr>
			<td>
				{t}Organization{/t}
			</td>
			<td>
				<input name='' type='text' value=''>
			</td>
		</tr>
		<tr>
			<td>
				{t}Name{/t}
			</td>
			<td>
				<input name='' type='text' value=''>
			</td>
		</tr>
		<tr>
			<td>
				{t}eMail{/t}{$must}
			</td>
			<td>
				<input name='' type='text' value=''>	
			</td>
		</tr>
	</table>


	<p>&nbsp;</p>
	<h2>
  		<input type='checkbox' name='use_gosa_announce' value='1' class='center'>&nbsp;{t}Send feedback to the GOsa project team{/t}
	</h2>
	{t}When checking this option, GOsa will try to connect http://oss.gonicus.de in order to submit your form anonymously.{/t}
	<br><b>{t}Generic{/t}</b>
	<table>	
		<tr>
			<td>
				{t}Did the setup procedure help you to get started?{/t}   
			</td>
			<td>
				<input type='radio' name='get_started' value='1'>{t}Yes{/t}
				<br>
				<input type='radio' name='get_started' value='0'>{t}No{/t}
			</td>
		</tr>
		<tr>
			<td>
				{t}If not, what problems did you encounter{/t}:
			</td>
			<td>
				<textarea name='problems_encountered' rows='4' cols='20'style='width:100%'></textarea>
			</td>
		</tr>
		<tr>
			<td>
				{t}Is this the first time you use GOsa?{/t}
			</td>
			<td>
				<input type='radio' name='first_use' value='1'>{t}Yes{/t}
				<br>
				<input type='radio' name='first_use' value='0'>{t}No{/t}
			</td>	
		</tr>
		<tr>
			<td></td>
			<td>
				{t}I use it since{/t}
				<select name='use_since_month' title='{t}Select the month since when you are using GOsa{/t}'> 
					<option></option>
				</select>
				<select name='use_since_year' title='{t}Select the year since when you are using GOsa{/t}'> 
					<option></option>
				</select>
			</td>
		</tr>
		<tr>
			<td>
				{t}What operation system / distribution do you use?{/t}
			</td>
			<td>
				<input type='text' name='distribution' value=''>
			</td>
		</tr>
		<tr>
			<td>
				{t}What web server do you use?{/t}
			</td>
			<td>
				<input type='text' name='distribution' value=''>
			</td>
		</tr>
		<tr>
			<td>
				{t}What PHP version do you use?{/t}
			</td>
			<td>
				<input type='text' name='distribution' value=''>
			</td>
		</tr>
	</table>

	<br><b>{t}LDAP{/t}</b>
	<table>
		<tr>
			<td>{t}What kind of LDAP server(s) do you use?{/t}
			</td>
			<td><input type='text' name='distribution' value=''>
			</td>
		</tr>
		<tr>
			<td>{t}How many objects are in your LDAP?{/t}
			</td>
			<td><input type='text' name='distribution' value=''>
			</td>
		</tr>
	</table>

	<br><b>{t}Features{/t}</b>
	<table>
		<tr>
			<td>{t}What features of GOsa do you use?{/t}
			</td>
			<td>
				[ ] POSIX (UNIX accounts/groups)
				[ ] Samba management
				[ ] Mailsystem management
				[ ] FAX system administration
				[ ] Asterisk administration
				[ ] System invetory
				[ ] System-/Configmanagement
				[ ] Addressbook
			</td>
		</tr>
		<tr>
			<td>{t}What features do you want to see in next versions of GOsa?{/t}
			</td>
			<td><textarea name='want_to_see_next'></textarea>
			</td>
		</tr>
	</table>
</div>
