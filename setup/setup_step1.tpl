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
	
	<div  class='default' style='float:left;width:160px;'>
		<br>
		<br>
		{t}GOsa language settings{/t}
	</div>
	<div class='default' style='float:left;'>
		<br>
		<br>
		{t}To force GOsa to use the selected language as global default language, use the option below.{/t}
		<br> 
		{t}If you want GOsa to automatically detect the language settings, leave this option clear.{/t} 
		&nbsp; 
		<br> 
		{if $force_global_lang}
		<input checked type='checkbox' name='force_global_lang'>&nbsp;{t}Force global language{/t} 
		{else}
		<input type='checkbox' name='force_global_lang'>&nbsp;{t}Force global language{/t} 
		{/if}
	</div>
</div>
