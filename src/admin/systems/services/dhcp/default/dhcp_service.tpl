<div class="dhcp-service-wrapper">
  <div class="row">
    <div class="col s12 xl6">
      <h3>{t}Generic{/t}</h3>

      {render acl=$acl}
      <label>
        <input id='authoritative' type=checkbox name="authoritative" value="1" {if $authoritative} checked {/if}>
        <span>{t}Authoritative service{/t}</span>
      </label>
      <br>
      {/render}


      {render acl=$acl}
      <div class="input-field">
        <select name='ddns_update_style' title='{t}Dynamic DNS update style{/t}' size="1">
          {html_options values=$ddns_styles output=$ddns_styles selected=$ddns_update_style}
        </select>
        <label>{t}Dynamic DNS update{/t}</label>
      </div>
      {/render}
    </div>

    <div class="col s12 xl6">
      {render acl=$acl}
      <div class="input-field">
        <input type='text' name='default_lease_time' id="default_lease_time" size='25' maxlength='80'
          value='{$default_lease_time}' title='{t}Enter default lease time in seconds.{/t}'>
        <label for="default_lease_time">{t}Default lease time (s){/t}</label>
      </div>
      {/render}

      {render acl=$acl}
      <div class="input-field">
        <input type='text' name='max_lease_time' id="max_lease_time" size='25' maxlength='80' value='{$max_lease_time}'
          title='{t}Enter maximum lease time in seconds.{/t}'>
        <label for="max_lease_time">{t}Maximum lease time (s){/t}</label>
      </div>
      {/render}
    </div>
  </div>
</div>

<!-- Place cursor in correct field -->
<script language="JavaScript" type="text/javascript">
  // First input field on page	
  focus_field('authoritative');
</script>