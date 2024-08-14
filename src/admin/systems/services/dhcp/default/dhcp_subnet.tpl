{* GOsa dhcp subnet - smarty template *}
<div class="dhcp-subnet-wrapper">
  <div class="row">
    <div class="col s12 xl6">
      <h3>{t}Generic{/t}</h3>
      {render acl=$acl}
      <div class="input-field">
        <input id='cn' type='text' name='cn' value='{$cn}'>
        <label for="cn">{t}Network address{/t}{$must}</label>
      </div>
      {/render}

      {render acl=$acl}
      <div class="input-field">
        <input type='text' name='dhcp_netmask' id="dhcp_netmask" value='{$dhcp_netmask}'>
        <label for="dhcp_netmask">{t}Net mask{/t}{$must}</label>
      </div>
      {/render}
    </div>

    <div class="col s12 xl6">
      {render acl=$acl}
      <label>
        <input type="checkbox" name="use_range" value="1"
          onChange="changeState('range_start');changeState('range_stop');" {$use_range}>
        <span>{t}Range for dynamic address assignment{/t}</span>
      </label>
      {/render}

      {render acl=$acl}
      <div class="input-field">
        <input type='text' id='range_start' name='range_start' value='{$range_start}' {$range_disabled}>
      </div>
      {/render}

      {render acl=$acl}
      <div class="input-field">
        <input type='text' id='range_stop' name='range_stop' value='{$range_stop}' {$range_disabled}>
      </div>
      {/render}
    </div>
  </div>
</div>

<input type='hidden' name='dhcp_subnet_posted' value='1'>

<!-- Place cursor in correct field -->
<script language="JavaScript" type="text/javascript">
  // First input field on page	 
  focus_field('cn');
</script>