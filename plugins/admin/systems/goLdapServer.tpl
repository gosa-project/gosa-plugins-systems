<h2><img class="center" alt="" align="middle" src="images/rightarrow.png" /> {t}LDAP service{/t}</h2>
{t}Base{/t} <input type="text" size="80" value="{$goLdapBase}" {$goLdapBaseACL} name="goLdapBase" id="goLdapBaseId">

<p class='seperator'>&nbsp;</p>
<p>
<div style="width:100%; text-align:right;">
    <input type='submit' name='SaveService' value='{t}Save{/t}'>
    &nbsp;
    <input type='submit' name='CancelService' value='{t}Cancel{/t}'>
</div>
</p>
<input type="hidden" name="goLdapServerPosted" value="1">
