<div class="servdhcp-wrapper">
    <div class="row">
        <div class="col s12 xl6">
            {if $dns_take_over}
            <h3>{t}DHCP take over will take place when saving this entry. Choose 'Cancel' to abort.{/t}</h3>

            {else}
            {if $dhcp_server_list_cnt}
            <div class="input-field add">
                <select name='take_over_src' size=1>
                    {html_options options=$dhcp_server_list}
                </select>
                <label>{t}Take over DHCP configuration from following server{/t}</label>
                <button class="btn-small" type='submit' name='take_over'>{msgPool type=applyButton}</button>
            </div>

            {/if}
            <h3>{t}DHCP sections{/t}</h3>
            {$DhcpList}
            
            {/if}
        </div>
    </div>
</div>

<div class="card-action">
    <button class="btn-small primary" type='submit' name='SaveService'>{msgPool type=saveButton}</button>
    <button class="btn-small primary" type='submit' name='CancelService'>{msgPool type=cancelButton}</button>
</div>