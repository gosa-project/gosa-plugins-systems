
    <div style='float:left; width:600px;'>
		<br>
		<b>{t}GOsa settings{/t}</b>
	</div>

    <div class='step4_container'>
        <div class='step4_name'>
            {t}GOsa theme{/t}
        </div>
        <div class='step4_value'>
            <select name="theme">
                {html_options options=$themes selected=$theme}
            </select>
        </div>

        <div class='step4_status'>
            {t}Infos in FAQ{/t}&nbsp;
            <img class='center' src='images/info_small.png' title='{t}Please read the FAQ for more informations{/t}'>
        </div>
    </div>

<input type='hidden' value='1' name='step7_posted'>
