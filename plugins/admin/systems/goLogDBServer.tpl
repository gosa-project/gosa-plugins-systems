<h2><img class="center" alt="" align="middle" src="images/rightarrow.png" /> {t}Logging database information{/t}</h2>
<table summary="">
    <tr>
     <td>{t}Logging DB user{/t}{$must}</td>
     <td>
{render acl=$goLogDBServerUserACL}
	<input name="goLogDBServerUser" id="goLogDBServerUser" size=30 maxlength=60 value="{$goLogDBServerUser}">
{/render}
     </td>
    </tr>
    <tr>
     <td>{t}Password{/t}{$must}</td>
     <td>
{render acl=$goLogDBServerPasswordACL}
 	<input type="password" name="goLogDBServerPassword" id="goLogDBServerPassword" size=30 maxlength=60 value="{$goLogDBServerPassword}">
{/render}
     </td>
    </tr>
    <tr>
     <td>{t}Database{/t}{$must}</td>
     <td>
{render acl=$goLogDBACL}
 	<input type="text" name="goLogDB" id="goLogDB" size=30 maxlength=60 value="{$goLogDB}">
{/render}
     </td>
    </tr>
   </table>

<p class='seperator'>&nbsp;</p>
<div style="width:100%; text-align:right;padding-top:10px;padding-bottom:3px;">
    <input type='submit' name='SaveService' value='{t}Save{/t}'>
    &nbsp;
    <input type='submit' name='CancelService' value='{t}Cancel{/t}'>
</div>
<input type="hidden" name="goLogDBServerPosted" value="1">
