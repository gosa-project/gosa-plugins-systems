{* GOsa dhcp sharedNetwork - smarty template *}
<div class="dhcp-group-wrapper">
    <div class="row">
        <h3>{t}Generic{/t}</h3>
        <div class="col s12 xl6">
            <h4>{t}DHCP group settings{/t}</h4>

            {render acl=$acl}
            <div class="input-field">
                <input id='cn' type='text' name='cn' value='{$cn}' title='{t}Name of group{/t}'>
                <label lang="cn">{t}Name{/t}{$must}</label>
            </div>
            {/render}

        </div>
    </div>
</div>

<!-- Place cursor in correct field -->
<script language="JavaScript" type="text/javascript">
    // First input field on page	 
    focus_field('cn');
</script>