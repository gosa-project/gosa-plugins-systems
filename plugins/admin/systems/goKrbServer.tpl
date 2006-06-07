<h2><img class="center" alt="" align="middle" src="images/rightarrow.png" /> {t}Kerberos kadmin access{/t}</h2>
  <table summary="">
    <tr>
     <td>{t}Kerberos Realm{/t}</td>
     <td><input name="goKrbRealm" id="goKrbRealm" size=30 maxlength=60 {$goKrbRealmACL} value="{$goKrbRealm}"></td>
    </tr>
    <tr>
     <td>{t}Admin user{/t}</td>
     <td><input name="goKrbAdmin" id="goKrbAdmin" size=30 maxlength=60 {$goKrbAdminACL} value="{$goKrbAdmin}"></td>
    </tr>
    <tr>
     <td>{t}Password{/t}</td>
     <td><input type=password name="goKrbPassword" id="goKrbPassword" size=30 maxlength=60 {$goKrbPasswordACL} value="{$goKrbPassword}"></td>
    </tr>
   </table>
<p class='seperator'>&nbsp;</p>
<p>
<div style="width:100%; text-align:right;">
    <input type='submit' name='SaveService' value='{t}Save{/t}'>
    &nbsp;
    <input type='submit' name='CancelService' value='{t}Cancel{/t}'>
</div>
</p>
<input type="hidden" name="goKrbServerPosted" value="1">
