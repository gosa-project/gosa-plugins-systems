<h2><img class="center" alt="" align="middle" src="images/rightarrow.png" /> {t}Terminal service{/t}</h2>
<table summary="">
    <tr>
     <td colspan=2>
      <input type="checkbox" value="true" name="goXdmcpIsEnabled" {$goXdmcpIsEnabledACL} 
		id="Temporariid" {if $goXdmcpIsEnabled} checked {/if}>{t}Temporary disable login{/t}</td>
    </tr>
    <tr>
     <td>{t}Font path{/t}</td>
     <td> <input type="text" value="{$goFontPath}" name="goFontPath" {$goXdmcpIsEnabledACL}  id="FontPathId"></td>
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
<input type="hidden" name="goTerminalServerPosted" value="1">
