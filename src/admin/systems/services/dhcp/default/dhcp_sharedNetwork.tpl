{* GOsa dhcp sharedNetwork - smarty template *}
<div class="dhcp-shared-wrapper">
  <div class="row">
    <div class="col s12 xl6">
      <h3>{t}Generic{/t}</h3>

      {render acl=$acl}
      <div class="input-field">
        <input id='cn' type='text' name='cn' value='{$cn}' title='{t}Name for shared network{/t}'>
        <label for="cn">{t}Name{/t}{$must}</label>
      </div>
      {/render}

      {render acl=$acl}
      <div class="input-field">
        <input type='text' name='server-identifier' id="server-identifier" value='{$server_identifier}'
          title='{t}Propagated server identifier for this shared network{/t}'>
        <label for="server-identifier">{t}Server identifier{/t}</label>
      </div>
      {/render}
    </div>

    <div class="col s12 xl6">
      {render acl=$acl}
      <label>
        <input type=checkbox name="authoritative" value="1" {if $authoritative} checked {/if}
          title="{t}Select if this server is authoritative for this shared network{/t}">
        <span>{t}Authoritative server{/t}</span>
      </label>
      {/render}
    </div>
  </div>

  <hr class="divider">

  <div class="row">
    <div class="col s12 xl6">
      <h3>{t}Leases{/t}</h3>

      {render acl=$acl}
      <div class="input-field">
        <input type='text' name='default-lease-time' id="default-lease-time" value='{$default_lease_time}'
          title='{t}Default lease time{/t}'>
        <label for="default-lease-time">{t}Default lease time{/t} ({t}seconds{/t})</label>
      </div>
      {/render}
      {render acl=$acl}
      <div class="input-field">
        <input type='text' name='max-lease-time' value='{$max_lease_time}' title='{t}Maximum lease time{/t}'>
        <label for="max-lease-time">{t}Max. lease time{/t} ({t}seconds{/t})</label>
      </div>
      {/render}
      {render acl=$acl}
      <div class="input-field">
        <input type='text' name='min-lease-time' value='{$min_lease_time}' title='{t}Minimum lease time{/t}'>
        <label for="min-lease-time">{t}Min. lease time{/t} ({t}seconds{/t})</label>
      </div>
      {/render}
    </div>

    <div class="col s12 xl6">
      <h3>{t}Access control{/t}</h3>

      {render acl=$acl}
      <label>
        <input type=checkbox name="unknown-clients" value="1" {$allow_unknown_state}
          title="{t}Select if unknown clients should get dynamic IP addresses{/t}">
        <span>{t}Allow unknown clients{/t}</span>
      </label>
      {/render}

      {render acl=$acl}
      <label>
        <input type=checkbox name="bootp" value="1" {$allow_bootp_state}
          title="{t}Select if BOOTP clients should get dynamic IP addresses{/t}">
        <span>{t}Allow BOOTP clients{/t}</span>
      </label>
      {/render}
      {render acl=$acl}
      <label>
        <input type=checkbox name="booting" value="1" {$allow_booting_state}
          title="{t}Select if clients are allowed to boot using this DHCP server{/t}">
        <span>{t}Allow booting{/t}</span>
      </label>
      {/render}
    </div>
  </div>
</div>

<!-- Place cursor in correct field -->
<script language="JavaScript" type="text/javascript">
  // First input field on page  
  document.mainform.cn.focus();
</script>