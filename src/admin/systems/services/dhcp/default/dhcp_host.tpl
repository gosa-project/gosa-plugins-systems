{* GOsa dhcp host - smarty template *}
<div class="dhcp-host-wrapper">
  <div class="row">
    <div class="col s12 xl6">
      <h3>{t}Generic{/t}</h3>

      <div class="input-field">
        {render acl=$acl}
        <input {if $realGosaHost} disabled {/if} id='cn' type='text' name='cn' value='{$cn}'
          title='{t}Name of host{/t}'>
        <label for="cn">{t}Name{/t}{$must}</label>
      </div>
      {/render}

      {render acl=$acl}
      <div class="input-field">
        <input {if $realGosaHost} disabled {/if} type='text' name='fixedaddr' id="fixeaddr" value='{$fixedaddr}'
          title='{t}Use host name or IP-address to assign fixed address{/t}'>
        <label for="fixeaddr"></label>{t}Fixed address{/t}
      </div>
      {/render}
    </div>

    <div class="col s12 xl6">
      <h3>{t}Hardware{/t}</h3>

      {render acl=$acl}
      <div class="input-field">
        <select name='hwtype' {if $realGosaHost} disabled {/if}size=1>
          {html_options options=$hwtypes selected=$hwtype}
        </select>
        <label>{t}Hardware type{/t}</label>
      </div>
      {/render}

      {render acl=$acl}
      <div class="input-field">
        <input {if $realGosaHost} disabled {/if} type='text' name='dhcpHWAddress' value='{$dhcpHWAddress}'>
        <label for="dhcpHWAddress">{t}Hardware address{/t}{$must}</label>
      </div>
      {/render}
    </div>
  </div>
</div>

<input type='hidden' name='dhcp_host_posted' value='1'>

<!-- Place cursor in correct field -->
<script language="JavaScript" type="text/javascript">
  // First input field on page	 
  focus_field('cn');
</script>