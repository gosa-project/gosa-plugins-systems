<h2><img class="center" alt="" align="middle" src="images/rightarrow.png" /> {t}Logging database information{/t}</h2>
<table summary="">
    <tr>
     <td>{t}Logging DB user{/t}</td>
     <td><input name="goLogAdmin" id="goLogAdmin" size=30 maxlength=60 {$goLogAdminACL} value="{$goLogAdmin}"></td>
    </tr>
    <tr>
     <td>{t}Password{/t}</td>
     <td><input type=password name="goLogPassword" id="goLogPassword" size=30 maxlength=60 {$goLogPasswordACL} value="{$goLogPassword}"></td>
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
