<b>{t}Kerberos kadmin access{/t}</b> 
 <table summary="">
    <tr>
     <td>{t}FAX DB user{/t}</td>
     <td><input name="goFaxAdmin" size=30 maxlength=60 {$goFaxAdminACL} id="goFaxAdmin" value="{$goFaxAdmin}" ></td>
    </tr>
    <tr>
     <td>{t}Password{/t}</td>
     <td><input type=password name="goFaxPassword" id="goFaxPassword" size=30 maxlength=60 {$goFaxPasswordACL}   value="{$goFaxPassword}" ></td>
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
<input type="hidden" name="goFaxServerPosted" value="1">
