{* GOsa dhcp sharedNetwork - smarty template *}
<div class="dhcp-network-wrapper">
  <div class="row">
    <div class="col s12 xl6">
      <h3>{t}Network configuration{/t}</h3>

      {render acl=$acl}
      <div class="input-field">
        <input id='routers' type='text' name='routers' value='{$routers}'
          title='{t}Enter name or IP address of router to be used in this section{/t}'>
        <label for="routers">{t}Router{/t}</label>
      </div>
      {/render}

      {render acl=$acl}
      <div class="input-field">
        <input type='text' name='subnet_mask' id="subnet_mask" value='{$subnet_mask}'>
        <label for="subnet_mask">{t}Net mask{/t}</label>
      </div>
      {/render}

      {render acl=$acl}
      <div class="input-field">
        <input type='text' name='broadcast_address' value='{$broadcast_address}'>
        <label for="broadcast_address">{t}Broadcast address{/t}</label>
      </div>
      {/render}
    </div>

    <div class="col s12 xl6">
      <h3>{t}Boot up{/t}</h3>

      {render acl=$acl}
      <div class="input-field">
        <input type='text' name='filename' id="filename" value='{$filename}'
          title='{t}Enter name of file that will be loaded via TFTP after client has started{/t}'>
        <label for="filename">{t}Filename{/t}</label>
      </div>
      {/render}

      {render acl=$acl}
      <div class="input-field">
        <input type='text' name='nextserver' id="nextserver" value='{$nextserver}'
          title='{t}Enter name of server to retrieve boot images from{/t}'>
        <label for="nextserver">{t}Next server{/t}</label>
      </div>
      {/render}
    </div>
  </div>

  <div class="row">
    <div class="col s12 xl6">
      <h3>{t}Domain Name Service{/t}</h3>
      {render acl=$acl}
      <div class="input-field">
        <input type='text' name='domain' id="domain" value='{$domain}' title='{t}Name of domain{/t}'>
        <label for="domain">{t}Domain{/t}</label>
      </div>
      {/render}

      {render acl=$acl}
      <div class="input-field">
        <select name='dnsserver' title='{t}List of DNS servers to be propagated{/t}' style="width:350px;" size="4">
          {html_options options=$dnsservers}
        </select>
        <label>{t}DNS server{/t}</label>
      </div>
      {/render}

      {render acl=$acl}
      <div class="input-field add">
        <input type='text' name='addserver' title='{t}DNS server do be added{/t}'>
        <button class="btn-small" type='submit' name='add_dns'
          title="{t}Click here add the selected server to the list{/t}">
          {msgPool type=addButton}</button>
        <button class="btn-small" type='submit' name='delete_dns'
          title="{t}Click here remove the selected servers from the list{/t}">{msgPool type=delButton}</button>
      </div>

    </div>

    <div class="col s12 xl6">
      <h3>{t}Domain Name Service options{/t}</h3>

      {render acl=$acl}
      <label>
        <input type=checkbox name="autohost" value="1" {$autohost}>
        <span>{t}Assign host names found via reverse mapping{/t}</span>
      </label>
      {/render}

      {render acl=$acl}
      <label>
        <input type=checkbox name="autohostdecl" value="1" {$autohostdecl}>
        <span>{t}Assign host names from host declarations{/t}</span>
      </label>
      {/render}
    </div>
  </div>
</div>

<!-- Place cursor in correct field -->
<script language="JavaScript" type="text/javascript">
  // First input field on page     
  focus_field('cn', 'routers');
</script>