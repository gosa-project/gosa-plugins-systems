<h3>{t}Network	settings{/t}</h3>

<table	summary="{t}Network settings{/t}" width="100%">
	<tr>
		<td style='width:50%; '>

        <table	summary="{t}Address settings{/t}">
          <tr>
            <td>
              <LABEL	for="ipHostNumber">{t}IP-address{/t}{if $IPisMust}{$must}{/if}</LABEL>
            </td>
            <td>
              {render acl=$ipHostNumberACL}
                <input	type='text' id="ipHostNumber"	name="ipHostNumber"	value="{$ipHostNumber}">
              {/render}
              {render acl=$ipHostNumberACL}
                {if $DNS_is_account == true}
                  <button type='submit' name='propose_ip' id="propose_ip">{t}Propose ip{/t}</button>
                {else}
                  <button type='submit' name='propose_ip' id="propose_ip">{t}Propose ip{/t}</button>
                {/if}
              {/render}

              {foreach from=$additionalHostNumbers item=item key=key}
                <br>
                {render acl=$ipHostNumberACL}
                  <input type='text' name='additionalHostNumbers_{$key}' value='{$item}'>
                {/render}
                {render acl=$ipHostNumberACL}
                  {image path="images/lists/trash.png" action="additionalHostNumbers_del_{$key}"}
                {/render}
              {/foreach}

              {render acl=$ipHostNumberACL}
                {image path="images/lists/edit.png[new]" action="additionalHostNumbers_add"}
              {/render}

          </td>
				</tr>
				<tr>
					<td>
            <LABEL	for="macAddress">{t}MAC-address{/t}</LABEL>{$must}
          </td>
					<td>
            {render acl=$macAddressACL}
              <input	type='text' name="macAddress"	id="macAddress"	value="{$macAddress}">
            {/render}
            {render acl=$autonetACL}
      	      <button type='submit' name='autonet'>{t}Autodetect{/t}</button>
            {/render}
          </td>
				</tr>
      </table>

      <hr>

{if $dhcpEnabled}

      <table  summary="{t}DHCP settings{/t}">
        {if $dhcpParentNodeCnt}
        <tr>
          <td>
            {render acl=$dhcpSetupACL}
              <input onClick='document.mainform.submit();'
                {if $dhcp_is_Account} checked {/if} type='checkbox' name='dhcp_is_Account' class='center'>
            {/render} 
          </td>
          <td colspan="2">
            {t}Enable DHCP for this device{/t}
            {render acl=$dhcpSetupACL}
              {image path="images/lists/reload.png"}
            {/render}
          </td>
        </tr>
        {else}
        <tr>
          <td>
            <input type='checkbox' name='dummy' class='center' disabled>
            {t}Enable DHCP for this device{/t} ({t}not configured{/t})
            {image path="images/lists/reload.png"}
          </td>
        </tr>
        {/if}
        {if $dhcp_is_Account}
        <tr>
          <td>&nbsp;</td>
          <td>{t}Parent node{/t}</td>            
          <td>

            {render acl=$dhcpSetupACL}
              <select name='dhcpParentNode' size=1>      
                {html_options options=$dhcpParentNodes selected=$dhcpParentNode}
              </select>
            {/render}
            <button type='submit' name='dhcpEditOptions'>{t}Edit settings{/t}</button>

          </td>
        </tr>
        {/if}
      </table>

    {/if}
    </td>

		<td style='width:50%;' class='left-border'>
    {if $DNSenabled==true}

    {if $ZoneCnt}
      
{render acl=$dnsSetupACL}
      {if $DNS_is_account == true}
      {if $hide_dns_check_box} 
        <input type="hidden" name="DNS_is_account" value="1"  
        <input type="checkbox" name="dummy" value="1" disabled

      {else}
        <input type="checkbox" name="DNS_is_account" value="1"  
      {/if}


        checked="checked"
        onclick="
          $('test2').toggle();
          changeState('zoneName');
          changeState('dNSTTL');
          $('propose_ip').toggle();
          changeState('AddNewRecord');
          {$changeStateForRecords}"/>
        {else}
      <input type="checkbox" name="DNS_is_account" value="1"  
        onclick="$('test2').toggle();
          changeState('zoneName');
          changeState('dNSTTL');
          $('propose_ip').toggle();
          changeState('AddNewRecord');"/>
        {/if}
{/render}
      {t}Enable DNS for this device{/t}
      {image path="images/lists/reload.png" action="reloadThisDNSStuff"}

      {if $DNS_is_account == true}
      <div style="padding-left:20px" id="test2">
      {else}
      <div style="padding-left:20px;display: none;" id="test2">
      {/if}
			<table summary="{t}DNS zone settings{/t}">
				<tr>
					<td><LABEL	for="zoneName">{t}Zone{/t}</LABEL></td>
					<td>
{render acl=$dnsSetupACL}
              <select name="zoneName" id="zoneName" {if $DNS_is_account == false} disabled {/if} size=1>
                {html_options values=$ZoneKeys output=$Zones selected=$zoneName}
              </select>
{/render}
          </td>
        </tr>
        <tr>
          <td>{t}TTL{/t}</td>
          <td>
{render acl=$dnsSetupACL}
            <input type="text" name="dNSTTL" value="{$dNSTTL}" id="dNSTTL" {if $DNS_is_account == false} disabled {/if}>
{/render}
          </td>
        </tr>
        <tr>
          <td valign="top">{t}Dns records{/t}</td>
          <td>
            {if $DNS_is_account == true}
{render acl=$dnsSetupACL}
              {$records}
{/render}
            {/if}
          </td>
				</tr>
			</table>
      </div>
    {else}        
      <input type="checkbox" name="dummy" value="1" disabled class='center' {if $DNS_is_account} checked {/if}>
      {t}Enable DNS for this device{/t} ({t}not configured{/t})
      {image path="images/lists/reload.png"}

    {/if}

		</td>
    {/if}
    
	</tr>
</table>
<input type="hidden" name="network_tpl_posted" value="1">

<!--
vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
-->
