<div class="ppdRemove-wrapper">
    <div style="font-size:18px;">
        {image path="<i class='material-icons'>warning</i>" align="top"} {t}Warning{/t}
    </div>
    <p>
        {$warning}
    </p>
    <p>
        {t}The PPD file will be removed from the server and can not be restored.{/t}
    </p>

</div>

<div class="card-action">
    <button class="btn-small primary" type='submit' name='delete_ppd_confirm'>
        {msgPool type=delButton}</button>
    <button class="btn-small primary" type='submit' name='delete_ppd_cancel'>
        {msgPool type=cancelButton}</button>
</div>