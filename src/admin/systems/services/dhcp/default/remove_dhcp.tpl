<div class="dhcp-remove-wrapper">
    <div class="row">
        {image path="<i class="material-icons">warning</i>" align="top"}&nbsp;{t}Warning{/t}

        <p>
            {$warning}{t}This includes 'all' DHCP subsections that are located within this section. Please double check
            if your
            really want to do this since there is no way for GOsa to get your data back.{/t}
        </p>
        <p>{t}Best thing to do before performing this action would be to save the current contents of your LDAP tree in
            a file.
            So - if you've done so - press 'Delete' to continue or 'Cancel' to abort.{/t}
        </p>
    </div>
</div>

<div class="card-action">
    <button class="btn-small primary" type='submit' name='delete_dhcp_confirm'>{msgPool type=delButton}</button>
    <button class="btn-small primary" type='submit' name='cancel_section'>{msgPool type=cancelButton}</button>
</div>