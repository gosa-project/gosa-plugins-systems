<div>
	<div class='default'>
		<b>{t}Please select your prefered language{/t}</b>
	</div>
	<br>
	<div  class='default' style='float:left;width:160px;'>
		{t}Setup language{/t}
	</div>
	<div class='default' style=';'>
		<select name='lang_selected' title='{t}Please your prefered language here{/t}' size=5 style='width:300px;'>
		{html_options options=$languages selected=$lang_selected}
		</select>
	</div>
</div>
