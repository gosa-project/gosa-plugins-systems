<div>
	<div class='default'>
		<b>{t}Please select the language that should be used in GOsa setup.{/t}</b>
	</div>
	<br>
	<div  class='default' style='float:left;width:120px;'>
		{t}Language{/t}
	</div>
	<div class='default' style='float:left;'>
		<select name='lang' title='{t}Please your prefered language here{/t}' size=5 style='width:300px;'>
		{html_options options=$languages selected=$lang}
		</select>
	</div>
	<div class='default' style='float:left;'>
		<input type='submit' name='next' value='{t}Next{/t}'>
	</div>
</div>
