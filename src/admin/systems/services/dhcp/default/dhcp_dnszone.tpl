{* GOsa dhcp host - smarty template *}
<div class="dhcp-dns-wrapper">
  <div class="row">
    <div class="col s12 xl6">
      <h3>{t}DNS update zone{/t}</h3>
      
      <div class="input-field">
        <select name='cn'>
          {html_options options=$cns selected=$cn}
        </select>
        <label>{t}DNS zone{/t}{$must}</label>
      </div>
      
      
      <div class="input-field">
        <select name='dhcpDnsZoneServer'>
          {html_options options=$dhcpDnsZoneServers selected=$dhcpDnsZoneServer}
        </select>
        <label>{t}DNS server{/t}{$must}</label>
      </div>
      
      <div class="input-field">
        <select name='dhcpKeyDN'>
          {html_options options=$dhcpKeyDNs selected=$dhcpKeyDN}
        </select>
        <label>{t}Key DN{/t}{$must}</label>
      </div>
    </div>
  </div>
</div>

<input type='hidden' name='dhcp_dnszone_posted' value='1'>

<!-- Place cursor in correct field -->
<script language="JavaScript" type="text/javascript">
  // First input field on page
  focus_field('cn');
</script>