
{if $warnings_cnt}
<b><font style='color:red' >Errors:</font></b>
{foreach from=$warnings item=val key=key}
	<br>
	<font style='color:red' >{$val}</font>
{/foreach}
{/if}

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
        </div>
    </div>

    <div class='step4_container'>
        <div class='step4_name'>
            {t}People dn attribute{/t}
        </div>
        <div class='step4_value'>
            <select size="1" name="peopledn">
                {html_options values=$peopledns output=$peopledns selected=$peopledn}
            </select>
        </div>
        <div class='step4_status'>
        </div>
    </div>

	<div class='step4_container'>
        <div class='step4_name'>
            {t}People storage ou{/t}
        </div>
        <div class='step4_value'>
            <input type='text' name='peopleou' maxlength='40' size='20' value='{$peopleou}'>
        </div>
        <div class='step4_status'>
        </div>
    </div>


	<div class='step4_container'>
        <div class='step4_name'>
            {t}Group storage ou{/t}
        </div>
        <div class='step4_value'>
            <input type='text' name='groupou' maxlength='40' size='20' value='{$groupou}'>
        </div>
        <div class='step4_status'>
        </div>
    </div>

    <div class='step4_container'>
        <div class='step4_name'>
            {t}ID base for users/groups{/t}
        </div>
        <div class='step4_value'>
            <input type='text' name='uidbase' maxlength='20' size='10' value='{$uidbase}'>
        </div>
        <div class='step4_status'>
        </div>
    </div>

    <div class='step4_container'>
        <div class='step4_name'>
            {t}Display PHP errors{/t}
        </div>
        <div class='step4_value'>
            <select name="errorlvl" size="1" title="">
			{html_options options=$bool selected=$errorlvl}
            </select>
        </div>
        <div class='step4_status'>
        </div>
    </div>
    <div style='float:left; width:600px;'>
		<br>
		<b>{t}Password settings{/t}</b>
	</div>

    <div class='step4_container'>
        <div class='step4_name'>
            {t}Password encryption algorithm{/t}
        </div>
        <div class='step4_value'>
            <select name="encryption" size="1" title="">
                {html_options options=$crypt_methods selected=$encryption}
            </select>
        </div>
        <div class='step4_status'>
        </div>
    </div>

    <div class='step4_container' style='height:4em;'>
        <div class='step4_name' style='height:4em;'>
            {t}Password restrictions{/t}
        </div>
        <div class='step4_value' style='height:4em;'>

			{if $pwd_rules.pwminlen_active == FALSE}
				<input type='checkbox' value='1' name='pwminlen_active'
					onClick='changeState("pwminlen");'>
					{t}Password minimum length{/t}
				<input id='pwminlen' name='pwminlen' type='text' value='{$pwd_rules.pwminlen}' size=3 disabled>
			{else}
				<input type='checkbox' value='1' name='pwminlen_active' checked>
				{t}Password minimum length{/t}
				<input id='pwminlen' name='pwminlen' type='text' value='{$pwd_rules.pwminlen}' size=3 >
			{/if}
			<br>
			{if $pwd_rules.pwdiffer_active == FALSE}
				<input type='checkbox' value='1' name='pwdiffer_active' 
					onClick='changeState("pwdiffer");'>
				{t}Different characters from old password{/t}
				<input id='pwdiffer' name='pwdiffer' type='text' value='{$pwd_rules.pwdiffer}' size=3 disabled>
			{else}
				<input type='checkbox' value='1' name='pwdiffer_active' checked>
				{t}Different characters from old password{/t}
				<input id='pwdiffer' name='pwdiffer' type='text' value='{$pwd_rules.pwdiffer}' size=3 >
			{/if}
	
        </div>
        <div class='step4_status'  style='height:4em;'>
			{t}Infos in FAQ{/t}&nbsp;
			<img class='center' src='images/info_small.png' title='{t}Please read the FAQ for more informations{/t}'>
        </div>
    </div>

	   <div class='step4_container'>
        <div class='step4_name'>
                {t}External script{/t}
        </div>
        <div class='step4_value'>
            {if $pwd_rules.externalpwdhook_active == FALSE}
                <input type='checkbox' value='1' name='externalpwdhook_active'
                    onClick='changeState("externalpwdhook");'>
                <input style='width:90%' id='externalpwdhook' name='externalpwdhook' type='text' value='{$pwd_rules.externalpwdhook}' disabled>
            {else}
                <input type='checkbox' value='1' name='externalpwdhook_active' checked>
                <input style='width:90%' id='externalpwdhook' name='externalpwdhook' type='text' value='{$pwd_rules.externalpwdhook}'  >
            {/if}
        </div>
        <div class='step4_status'>
			{t}Infos in FAQ{/t}&nbsp;
			<img class='center' src='images/info_small.png' title='{t}Please read the FAQ for more informations{/t}'>
        </div>
    </div>

    <div style='float:left; width:600px;'>
		<br>
		<b>{t}Mail settings{/t}</b>
	</div>

    <div class='step4_container'>
        <div class='step4_name'>
            {t}Mail method{/t}
        </div>
        <div class='step4_value'>
            <select name="mail" size="1" title="">
                <option name="mail" value="disabled">{t}disabled{/t}</option>
                {html_options options=$mail_methods selected=$mail}
            </select>
        </div>
        <div class='step4_status'>
        </div>
    </div>

    <div class='step4_container'>
        <div class='step4_name'>
            {t}Vacation templates{/t}
        </div>
        <div class='step4_value'>
            {if $mail_settings.vacationdir_active == FALSE}
                <input type='checkbox' value='1' name='vacationdir_active'
                    onClick='changeState("vacationdir");'>
                <input style='width:90%' id='vacationdir' name='vacationdir' type='text' value='{$mail_settings.vacationdir}' disabled>
            {else}
                <input type='checkbox' value='1' name='vacationdir_active' checked>
                <input style='width:90%' id='vacationdir' name='vacationdir' type='text' value='{$mail_settings.vacationdir}'  >
            {/if}
        </div>
        <div class='step4_status'>
			{t}Infos in FAQ{/t}&nbsp;
			<img class='center' src='images/info_small.png' title='{t}Please read the FAQ for more informations{/t}'>
        </div>
    </div>

    <div class='step4_container'>
        <div class='step4_name'>
            {t}Use cyrus account style{/t}
        </div>
        <div class='step4_value'>
            <select name="cyrusunixstyle" size="1" title="">
			{html_options options=$bool selected=$cyrusunixstyle}
            </select>
        </div>
        <div class='step4_status'>
			{t}Infos in FAQ{/t}&nbsp;
			<img class='center' src='images/info_small.png' title='{t}Please read the FAQ for more informations{/t}'>
        </div>
    </div>

<input type='hidden' value='1' name='step5_posted'>
<!--
         {t}GOsa supports several encryption types for your passwords. Normally this is adjustable via user templates, but you can specify a default method to be used here, too.{/t}
         {t}GOsa always acts as admin and manages access rights internally. This is a workaround till OpenLDAP's in directory ACI's are    fully implemented. For this to work, we need the admin DN and the corresponding password.{/t}
         {t}Some basic LDAP parameters are tunable and affect the locations where GOsa saves people and groups, including the way accounts get created. Check the values below if the fit your needs.{/t}
         {t}GOsa has modular support for several mail methods. These methods provide interfaces to users mailboxes and general handling    for quotas. You can choose the dummy plugin to leave all your mail settings untouched.{/t}-->

