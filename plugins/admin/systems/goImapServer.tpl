<h2><img class="center" alt="" align="middle" src="images/rightarrow.png" /> {t}Generic{/t}</h2>
<table width="100%">
	<tr>
		<td style='width:50%;'>
			<table summary="">
				<tr>
					<td>{t}Server identifier{/t}
					</td>
					<td><input name="goImapName" id="goImapName" size=40 maxlength=60 {$goImapNameACL} value="{$goImapName}" >
					</td>
				</tr>
				<tr>
					<td>{t}Connect URL{/t}
					</td>
					<td><input name="goImapConnect" id="goImapConnect" size=40 maxlength=60 {$goImapConnectACL} value="{$goImapConnect}" >
					</td>
				</tr>
				<tr>
					<td>{t}Admin user{/t}
					</td>
					<td><input name="goImapAdmin" id="goImapAdmin" size=30 maxlength=60 {$goImapAdminACL} value="{$goImapAdmin}" >
					</td>
				</tr>
				<tr>
					<td>{t}Password{/t}
					</td>
					<td><input type=password name="goImapPassword" id="goImapPassword" size=30 maxlength=60 {$goImapPasswordACL} value="{$goImapPassword}" >
					</td>
				</tr>
				<tr>
					<td>{t}Sieve port{/t}
					</td>
					<td><input name="goImapSievePort" id="goImapSievePort" size=10 maxlength=30 {$goImapSievePortACL} value="{$goImapSievePort}">
					</td>
				</tr>
			</table>
		</td>
		<td style="border-left:1px solid #A0A0A0;vertical-align:top;">
			<table>
				<tr>
					<td>{t}Start IMAP service{/t}
					</td>
					<td>
						<input type='checkbox' name='cyrusImap' value=1 {if $cyrusImap} checked {/if} {$cyrusImapACL}> 
					</td>
				</tr>
				<tr>
					<td>{t}Start IMAP SSL service{/t}
					</td>
					<td>
						<input type='checkbox' name='cyrusImapSSL' value=1 {if $cyrusImapSSL} checked {/if} {$cyrusImapSSLACL}> 
					</td>
				</tr>
				<tr>
					<td>{t}Start POP3 service{/t}
					</td>
					<td>
						<input type='checkbox' name='cyrusPop3' value=1 {if $cyrusPop3} checked {/if} {$cyrusPop3ACL}> 
					</td>
				</tr>
				<tr>
					<td>{t}Start POP3 SSL service{/t}
					</td>
					<td>
						<input type='checkbox' name='cyrusPop3SSL' value=1 {if $cyrusPop3SSL} checked {/if} {$cyrusPop3SSLACL}> 
					</td>
				</tr>
			</table>
		</td>
	</tr>
</table>
<p class='seperator'>&nbsp;</p>
<br>
<h2><img class="center" alt="" src="images/rocket.png" align="middle"> Action</h2>
{if $is_new == "new"}
    {t}The server must be saved before you can use the status flag.{/t}
{elseif !$is_acc}
    {t}The service must be saved before you can use the status flag.{/t}
{/if}
<br>
<select name="action" title='{t}{/t}' 
	{if $is_new =="new" || !$is_acc} disabled {/if}
>
	<option value="none">&nbsp;</option>
    {html_options options=$Actions}
</select>
<input type='submit' name='ExecAction' title='{t}Set status{/t}' value='{t}Execute{/t}' 
	{if $is_new == "new" | !$is_acc} disabled {/if}
>

<p class="seperator">&nbsp;</p>
<p>
<div style="width:100%; text-align:right;">
    <input type='submit' name='SaveService' value='{t}Save{/t}'>
    &nbsp;
    <input type='submit' name='CancelService' value='{t}Cancel{/t}'>
</div>
</p>
<input type="hidden" name="goImapServerPosted" value="1">
