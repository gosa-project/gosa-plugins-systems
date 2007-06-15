{if $object == "server"}
    <table summary="">
     <tr>
      <td><LABEL for="cn">{t}Server name{/t}</LABEL>{$must}</td>
      <td>
       <input name="cn" id="cn" size=20 maxlength=60 value="{$cn}">
      </td>
     </tr>
	</table>
{/if}
{if $object == "terminal"}
    <table summary="">
     <tr>
      <td><LABEL for="cn">{t}Server name{/t}</LABEL>{$must}</td>
      <td>
       <input name="cn" id="cn" size=20 maxlength=60 value="{$cn}">
      </td>
     </tr>
	</table>
{/if}
