<div>
	<div class='default'>
		<b>{t}Please select the language you prefer to use GOsa with.{/t}</b>
	</div>
	<br>
	<div  class='default' style='float:left;width:160px;'>
		{t}Setup language{/t}
	</div>
	<div class='default' style=';'>
		<select name='lang' title='{t}Please your prefered language here{/t}' size=5 style='width:300px;'>
		{html_options options=$languages selected=$lang}
		</select>
	</div>
	
	<div  class='default' style='float:left;width:160px;padding-top:25px;'>
		{t}GOsa language settings{/t}
	</div>
	<div class='default' style='float:left;padding-top:25px;'>
		<input type='radio' value='0' name='force_global_lang' {if !$force_global_lang} checked {/if}>&nbsp;
		{t}GOsa will automatically detect the language to use in the GOsa user interface.{/t}
		<br>
		<input type='radio' value='1' name='force_global_lang' {if $force_global_lang} checked {/if}>&nbsp;
		{t}Force GOsa to use the above selected language as default language.{/t}
	</div>
</div>
