{* GOsa dhcp sharedNetwork - smarty template *}
<div class="dhcp-pool-wrapper">
  <div class="row">
    <div class="col s12 xl6">
      <h3>{t}Generic{/t}</h3>

      
      {render acl=$acl}
      <div class="input-field">
        <input id='cn' type='text' name='cn' size='25' maxlength='80' value='{$cn}' title='{t}Name of pool{/t}'>
<label for="cn">{t}Name{/t}{$must}</label>
      </div>
      {/render}
    </div>

    <div class="col s12 xl6">
      &nbsp;
      {render acl=$acl}
      <div class="input-field add">
        <input type='text' name='range_start' id="range_start" size='25' maxlength='30' value='{$range_start}'>
        <label for="range_start">{t}Range{/t}{$must}</label>
        <input type='text' name='range_stop' size='25' maxlength='30' value='{$range_stop}'>
      </div>
      {/render}
    </div>
  </div>
</div>

<!-- Place cursor in correct field -->
<script language="JavaScript" type="text/javascript">
  // First input field on page  
  document.mainform.cn.focus();
</script>