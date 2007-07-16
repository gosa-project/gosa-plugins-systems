<div>
	<div class='default'>
		<p><b>{t}Please select the preferred language{/t}</b></p>
		<p>
		{t}At this point, you can select the site wide default language. Choosing 'automatic' will use the language requested by the browser. This setting can be overriden per user.{/t}
		</p>
	</div>
	<div class='default' style='padding-right:10px; margin-bottom:20px;'>
		<select name='lang_selected' title='{t}Please your preferred language here{/t}' size=20 style="width:100%">
		{html_options options=$languages selected=$lang_selected}
		</select>
	</div>
  	<div class='step4_container'>
        <div class='step4_name'>
            {t}Timezone{/t}
        </div>
        <div class='step4_value'>
			<select name='timezone' title='{t}Please your preferred timezone here{/t}' style="width:100%">
		{foreach from=$timezones item=val key=key}
			{if $val.name == $timezone}
			<option selected value="{$val.name}">{$val.name}</option>
			{else}
			<option value="{$val.name}">{$val.name}</option>
			{/if}
		{/foreach}
			</select>
        </div>
    </div>
</div>
