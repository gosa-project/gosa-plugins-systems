{* GOsa dhcp sharedNetwork - smarty template *}
<div class="dhcp-advanced-wrapper">
    {if $show_advanced}


    <div class="row">
        <button class="btn-small" type='submit' name='hide_advanced'>{t}Hide advanced settings{/t}</button>
        <div class="col s12 xl6">
            <h3>{t}DHCP statements{/t}</h3>

            {render acl=$acl}
            <div class="input-field">
                <select name='dhcpstatements' style="width:100%;" size="14">
                    {html_options options=$dhcpstatements}
                </select>
            </div>
            {/render}

            {render acl=$acl}
            <div class="input-field add">
                <input type='text' name='addstatement' size='25' maxlength='250'>
                <button class="btn-small" type='submit' name='add_statement'>{msgPool type=addButton}</button>&nbsp;
                <button class="btn-small" type='submit' name='delete_statement'>{msgPool type=delButton}</button>
            </div>
            {/render}
        </div>

        <div class="col s12 xl6">
            <h3>{t}DHCP options{/t}</h3>

            {render acl=$acl}
            <div class="input-field">
                <select name='dhcpoptions' style="width:100%;" size="14">
                    {html_options options=$dhcpoptions}
                </select>
            </div>
            {/render}

            {render acl=$acl}
            <div class="input-field">
                <input type='text' name='addoption' size='25' maxlength='250'>
                <button class="btn-small" type='submit' name='add_option'>{msgPool type=addButton}</button>
                <button class="btn-small" type='submit' name='delete_option'>{msgPool type=delButton}</button>
            </div>
            {/render}
        </div>
    </div>
    {else}
    <button class="btn-small" type='submit' name='show_advanced'>{t}Show advanced settings{/t}</button>
    {/if}
</div>