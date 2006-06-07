<b>{t}LDAP Service{/t}</b> 

<input type="text" style="width:70%" value="{$goLdapBase}" {$goLdapBaseACL} name="goLdapBase" id="goLdapBaseId">

<p class='seperator'>&nbsp;</p>
<p>
<div style="width:100%; text-align:right;">
    <input type='submit' name='SaveService' value='{t}Save{/t}'>
    &nbsp;
    <input type='submit' name='CancelService' value='{t}Cancel{/t}'>
</div>
</p>
<input type="hidden" name="goLdapServerPosted" value="1">
