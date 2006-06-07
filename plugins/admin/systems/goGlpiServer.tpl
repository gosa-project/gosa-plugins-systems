<h2><img class="center" alt="" align="middle" src="images/rightarrow.png" /> {t}GLPI database information{/t}</h2>
  <table summary="">
    <tr>
     <td>{t}Logging DB user{/t}</td>
     <td><input name="goGlpiAdmin" id="goGlpiAdmin" size=30 maxlength=60 {$goGlpiAdminACL}
value="{$goGlpiAdmin}"></td>
    </tr>
    <tr>
     <td>{t}Password{/t}</td>
     <td><input type="password" name="goGlpiPassword" id="goGlpiPassword" size=30 maxlength=60 {$goGlpiPasswordACL}
value="{$goGlpiPassword}"></td>
    </tr>
    <tr>
     <td>{t}Database{/t}</td>
     <td><input name="goGlpiDatabase" id="goGlpiDatabase" size=30 maxlength=60 {$goGlpiDatabaseACL}
value="{$goGlpiDatabase}"></td>
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
<input type="hidden" name="goLogDBServerPosted" value="1">
