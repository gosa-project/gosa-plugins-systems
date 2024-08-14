{* GOsa dhcp host - smarty template *}
<div class="dhcp-tsigkey-wrapper">
  <div class="row">
    <div class="col s12 xl6">
      <h3>{t}DNS update key{/t}</h3>

      <div class="input-field">
        <input id='cn' type='text' name='cn' size='25' maxlength='80' value='{$cn}' title='{t}Name of Key{/t}'>
        <label for="cn">{t}Key name{/t}{$must}</label>
      </div>

      <div class="input-field">
        <select name='dhcpKeyAlgorithm'>
          {html_options options=$dhcpKeyAlgorithms selected=$dhcpKeyAlgorithm}
        </select>
        <label>{t}Encryption method{/t}</label>
      </div>
      
      <div class="input-field">
        <input type='text' name='dhcpKeySecret' size='30' maxlength='80' value='{$dhcpKeySecret}'>
        <label for="dhcpKeySecret">{t}Key{/t}{$must}</label>
      </div>
    </div>
  </div>
</div>

<input type='hidden' name='dhcp_tsigkey_posted' value='1'>

<!-- Place cursor in correct field -->
<script language="JavaScript" type="text/javascript">
  // First input field on page
  focus_field('cn');
</script>