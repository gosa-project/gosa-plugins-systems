<div class='default' style='margin:12px; '>
    <div style='float:left; '>
		<br>
		<b>{t}Optional settings{/t}</b>
	</div>

    <div class='step4_container'>
        <div class='step4_name'>
            {t}Disable primary group filter{/t}
        </div>
        <div class='step4_value'>
	
            <select name="noprimarygroup" size="1" title="">
            	{html_options options=$bool selected=$optional.noprimarygroup}
            </select>
        </div>
        <div class='step4_status'>
            {t}Infos in FAQ{/t}&nbsp;
            <img class='center'  alt='!' src='images/info_small.png' title='{t}Please read the FAQ for more informations{/t}'>
        </div>
    </div>

    <div class='step4_container'>
        <div class='step4_name'>
            {t}Force globals{/t}
        </div>
        <div class='step4_value'>
	
            <select name="forceglobals" size="1" title="">
            	{html_options options=$bool selected=$optional.forceglobals}
            </select>
        </div>
        <div class='step4_status'>
            {t}Infos in FAQ{/t}&nbsp;
            <img class='center'  alt='!' src='images/info_small.png' title='{t}Please read the FAQ for more informations{/t}'>
        </div>
    </div>

    <div class='step4_container'>
        <div class='step4_name'>
            {t}Force SSL{/t}
        </div>
        <div class='step4_value'>
	
            <select name="forcessl" size="1" title="">
            	{html_options options=$bool selected=$optional.forcessl}
            </select>
        </div>
        <div class='step4_status'>
            {t}Infos in FAQ{/t}&nbsp;
            <img class='center'  alt='!' src='images/info_small.png' title='{t}Please read the FAQ for more informations{/t}'>
        </div>
    </div>

    <div class='step4_container'>
        <div class='step4_name'>
            {t}Warn SSL{/t}
        </div>
        <div class='step4_value'>
	
            <select name="warnssl" size="1" title="">
            	{html_options options=$bool selected=$optional.warnssl}
            </select>
        </div>
        <div class='step4_status'>
            {t}Infos in FAQ{/t}&nbsp;
            <img class='center'  alt='!' src='images/info_small.png' title='{t}Please read the FAQ for more informations{/t}'>
        </div>
    </div>

    <div class='step4_container'>
        <div class='step4_name'>
            {t}Strict units{/t}
        </div>
        <div class='step4_value'>
	
            <select name="strict_units" size="1" title="">
            	{html_options options=$bool selected=$optional.strict_units}
            </select>
        </div>
        <div class='step4_status'>
            {t}Infos in FAQ{/t}&nbsp;
            <img class='center'  alt='!' src='images/info_small.png' title='{t}Please read the FAQ for more informations{/t}'>
        </div>
    </div>

    <div class='step4_container'>
        <div class='step4_name'>
            {t}PPd path{/t}
        </div>
        <div class='step4_value'>
            {if $optional.ppd_path_active == FALSE}
                <input type='checkbox' value='1' name='ppd_path_active'
                    onClick='changeState("ppd_path");'>
                <input style='width:90%' id='ppd_path' name='ppd_path' type='text' value='{$optional.ppd_path}' disabled>
            {else}
                <input type='checkbox' value='1' name='ppd_path_active' checked>
                <input style='width:90%' id='ppd_path' name='ppd_path' type='text' value='{$optional.ppd_path}'  >
            {/if}
        </div>
        <div class='step4_status'>
            {t}Infos in FAQ{/t}&nbsp;
            <img class='center'  alt='!' src='images/info_small.png' title='{t}Please read the FAQ for more informations{/t}'>
        </div>
    </div>

    <div class='step4_container'>
        <div class='step4_name'>
            {t}Network resolv hook{/t}
        </div>
        <div class='step4_value'>
            {if $optional.auto_network_hook_active == FALSE}
                <input type='checkbox' value='1' name='auto_network_hook_active'
                    onClick='changeState("auto_network_hook");'>
                <input style='width:90%' id='auto_network_hook' name='auto_network_hook' 
					type='text' value='{$optional.auto_network_hook}' disabled>
            {else}
                <input type='checkbox' value='1' name='auto_network_hook_active' checked>
                <input style='width:90%' id='auto_network_hook' name='auto_network_hook' type='text' 
					value='{$optional.auto_network_hook}'  >
            {/if}
        </div>
        <div class='step4_status'>
            {t}Infos in FAQ{/t}&nbsp;
            <img class='center'  alt='!' src='images/info_small.png' title='{t}Please read the FAQ for more informations{/t}'>
        </div>
    </div>

    <div class='step4_container'>
        <div class='step4_name'>
            {t}Mail queue script path{/t}
        </div>
        <div class='step4_value'>
            {if $optional.mailQueueScriptPath_active == FALSE}
                <input type='checkbox' value='1' name='mailQueueScriptPath_active'
                    onClick='changeState("mailQueueScriptPath");'>
                <input style='width:90%' id='mailQueueScriptPath' name='mailQueueScriptPath' 
					type='text' value='{$optional.mailQueueScriptPath}' disabled>
            {else}
                <input type='checkbox' value='1' name='mailQueueScriptPath_active' checked>
                <input style='width:90%' id='mailQueueScriptPath' name='mailQueueScriptPath' type='text' 
					value='{$optional.mailQueueScriptPath}'  >
            {/if}
        </div>
        <div class='step4_status'>
            {t}Infos in FAQ{/t}&nbsp;
            <img class='center'  alt='!' src='images/info_small.png' title='{t}Please read the FAQ for more informations{/t}'>
        </div>
    </div>

    <div class='step4_container'>
        <div class='step4_name'>
            {t}Notification path{/t}
        </div>
        <div class='step4_value'>
            {if $optional.notifydir_active == FALSE}
                <input type='checkbox' value='1' name='notifydir_active'
                    onClick='changeState("notifydir");'>
                <input style='width:90%' id='notifydir' name='notifydir' 
					type='text' value='{$optional.notifydir}' disabled>
            {else}
                <input type='checkbox' value='1' name='notifydir_active' checked>
                <input style='width:90%' id='notifydir' name='notifydir' 
					type='text' value='{$optional.notifydir}'  >
            {/if}
        </div>
        <div class='step4_status'>
            {t}Infos in FAQ{/t}&nbsp;
            <img class='center'  alt='!' src='images/info_small.png' title='{t}Please read the FAQ for more informations{/t}'>
        </div>
    </div>

    <div class='step4_container'>
        <div class='step4_name'>
            {t}Kiosk path{/t}
        </div>
        <div class='step4_value'>
            {if $optional.kioskpath_active == FALSE}
                <input type='checkbox' value='1' name='kioskpath_active'
                    onClick='changeState("kioskpath");'>
                <input style='width:90%' id='kioskpath' name='kioskpath' 
					type='text' value='{$optional.kioskpath}' disabled>
            {else}
                <input type='checkbox' value='1' name='kioskpath_active' checked>
                <input style='width:90%' id='kioskpath' name='kioskpath' 
					type='text' value='{$optional.kioskpath}'  >
            {/if}
        </div>
        <div class='step4_status'>
            {t}Infos in FAQ{/t}&nbsp;
            <img class='center'  alt='!' src='images/info_small.png' title='{t}Please read the FAQ for more informations{/t}'>
        </div>
    </div>

    <div class='step4_container'>
        <div class='step4_name'>
            {t}Smarty compile directory{/t}
        </div>
        <div class='step4_value'>
			<input style='width:90%' id='compile' name='compile' 
				type='text' value='{$optional.compile}'>
        </div>
        <div class='step4_status'>
            {t}Infos in FAQ{/t}&nbsp;
            <img class='center'  alt='!' src='images/info_small.png' title='{t}Please read the FAQ for more informations{/t}'>
        </div>
    </div>

    <div class='step4_container' style='height:5em;'>
        <div class='step4_name' style='height:5em;'>
            {t}SMB hash{/t}
        </div>
        <div class='step4_value' style='height:5em;'>
			<input {if $optional.smbhash == "/usr/bin/mkntpasswd"} checked {/if} 
				type='radio' name='smbhash' value='/usr/bin/mkntpasswd'>&nbsp;
				/usr/bin/mkntpasswd<br>
			<input {if $optional.smbhash == "mkntpwd"} checked {/if}
				type='radio' name='smbhash' value='mkntpwd'>&nbsp;
				mkntpwd<br>
			<input {if $optional.smbhash == 'perl -MCrypt::SmbHash -e "print join(q[:], ntlmgen \$ARGV[0]), $/;"'} checked {/if}
				type='radio' name='smbhash' value='perl -MCrypt::SmbHash -e "print join(q[:], ntlmgen \$ARGV[0]), $/;"'>&nbsp;
				perl -MCrypt::SmbHash ...
        </div>
        <div class='step4_status' style='height:5em;'>
            {t}Infos in FAQ{/t}&nbsp;
            <img class='center'  alt='!'  src='images/info_small.png' title='{t}Please read the FAQ for more informations{/t}'>
        </div>
    </div>

    <div class='step4_container'>
        <div class='step4_name'>
            {t}Session lifetime{/t}
        </div>
        <div class='step4_value'>
			<input style='width:90%' id='session_lifetime' name='session_lifetime' 
				type='text' value='{$optional.session_lifetime}'>
        </div>
        <div class='step4_status'>
            {t}Infos in FAQ{/t}&nbsp;
            <img class='center' alt='!'  src='images/info_small.png' title='{t}Please read the FAQ for more informations{/t}'>
        </div>
    </div>

    <div class='step4_container'>
        <div class='step4_name'>
            {t}Maximal ldap query time{/t}
        </div>
        <div class='step4_value'>
            {if $optional.max_ldap_query_time_active == FALSE}
                <input type='checkbox' value='1' name='max_ldap_query_time_active'
                    onClick='changeState("max_ldap_query_time");'>
                <input style='width:90%' id='max_ldap_query_time' name='max_ldap_query_time' 
					type='text' value='{$optional.max_ldap_query_time}' disabled>
            {else}
                <input type='checkbox' value='1' name='max_ldap_query_time_active' checked>
                <input style='width:90%' id='max_ldap_query_time' name='max_ldap_query_time' 
					type='text' value='{$optional.max_ldap_query_time}'  >
            {/if}
        </div>
        <div class='step4_status'>
            {t}Infos in FAQ{/t}&nbsp;
            <img class='center' alt='!'  src='images/info_small.png' title='{t}Please read the FAQ for more informations{/t}'>
        </div>
    </div>

    <div class='step4_container'>
        <div class='step4_name'>
            {t}Debuglevel{/t}
        </div>
        <div class='step4_value'>
			<select name='debuglevel'>
				{if $optional.debuglevel == 0}
					<option value='0' selected>0 {t}Disabled{/t}</option>
					<option value='1'>1 {t}Enabled{/t}</option>
				{else}
					<option value='0'>0 {t}Disabled{/t}</option>
					<option value='1' selected>1 {t}Enabled{/t}</option>
				{/if}
			</select>		
        </div>
        <div class='step4_status'>
            {t}Infos in FAQ{/t}&nbsp;
            <img class='center'  alt='!' src='images/info_small.png' title='{t}Please read the FAQ for more informations{/t}'>
        </div>
    </div>
</div>

<input type='hidden' value='1' name='step6a_posted'>
