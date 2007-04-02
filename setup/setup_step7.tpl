
{foreach from=$checks item=val key=key}
    <div class='step4_container'>
        <div class='step4_name'>
			{$key}
        </div>
        <div class='step4_value'>
	{$checks[$key].INFO}
        </div>

        <div class='step4_status'>
	{$checks[$key].MSG}
        </div>
    </div>
{/foreach}


<input type='hidden' value='1' name='step7_posted'>
