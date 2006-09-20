<h2><img class="center" alt="" align="middle" src="images/rightarrow.png" /> {t}VoIP database information{/t}</h2>
  <table summary="">
    <tr>
     <td>{t}Asterisk DB user{/t}{$must}</td>
     <td>
{render acl=$goFonAdminACL}
      <input name="goFonAdmin" size=30 maxlength=60 id="goFonAdmin" {$goFonAdminACL} value="{$goFonAdmin}">
{/render}
     </td>
    </tr>
    <tr>
     <td>{t}Password{/t}{$must}</td>
     <td>
{render acl=$goFonPasswordACL}
      <input type=password name="goFonPassword" id="goFonPassword" size=30 maxlength=60 {$goFonPasswordACL}   value="{$goFonPassword}">
{/render}
     </td>
    </tr>
    <tr>
     <td>{t}Country dial prefix{/t}{$must}</td>
     <td>
{render acl=$goFonCountryCodeACL}
      <input name="goFonCountryCode" size=10 maxlength=30 id="goFonCountryCode" {$goFonCountryCodeACL}   value="{$goFonCountryCode}">
{/render}
     </td>
    </tr>
    <tr>
     <td>{t}Local dial prefix{/t}{$must}</td>
     <td>
{render acl=$goFonAreaCodeACL}
      <input name="goFonAreaCode" size=10 maxlength=30 id="goFonAreaCode" {$goFonAreaCodeACL}   value="{$goFonAreaCode}">
{/render}
     </td>
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
<input type="hidden" name="goFonServerPosted" value="1">
