
{if $warnings_cnt}
<b><font style='color:red' >Errors:</font></b>
{foreach from=$warnings item=val key=key}
	<br>
	<font style='color:red' >{$val}</font>
{/foreach}
{/if}
<div class='default' style='margin:12px; '>  
    <p><b>{t}Samba settings{/t}</b></p>
    <div class='step4_container'>
        <div class='step4_name'>
            {t}Samba SID{/t}
        </div>
        <div class='step4_value'>
            {if $samba_settings.samba_sid_active == FALSE}
                <input type='checkbox' value='1' name='samba_sid_active'
                    onClick='changeState("samba_sid");'>
                <input size=20 id='samba_sid' name='samba_sid' type='text' value='{$samba_settings.samba_sid}' disabled>
            {else}
                <input type='checkbox' value='1' name='samba_sid_active' checked>
                <input size=20 id='samba_sid' name='samba_sid' type='text' value='{$samba_settings.samba_sid}'  >
            {/if}
        </div>
    </div>

    <div class='step4_container'>
        <div class='step4_name'>
            {t}RID base{/t}
        </div>
        <div class='step4_value'>
            {if $samba_settings.samba_rid_active == FALSE}
                <input type='checkbox' value='1' name='samba_rid_active'
                    onClick='changeState("samba_rid");'>
                <input size=20 id='samba_rid' name='samba_rid' type='text' value='{$samba_settings.samba_rid}' disabled>
            {else}
                <input type='checkbox' value='1' name='samba_rid_active' checked>
                <input size=20 id='samba_rid' name='samba_rid' type='text' value='{$samba_settings.samba_rid}'  >
            {/if}
        </div>
    </div>
    <div class='step4_container'>
        <div class='step4_name'>
            {t}Workstation container{/t}
        </div>
        <div class='step4_value'>
            {if $generic_settings.wws_ou_active == FALSE}
                <input type='checkbox' value='1' name='wws_ou_active'
                    onClick='changeState("wws_ou");'>
                <input size=40 id='wws_ou' name='wws_ou' type='text' value='{$generic_settings.wws_ou}' disabled>
            {else}
                <input type='checkbox' value='1' name='wws_ou_active' checked>
                <input size=40 id='wws_ou' name='wws_ou' type='text' value='{$generic_settings.wws_ou}'  >
            {/if}
        </div>
    </div>
    <div class='step4_container'>
        <div class='step4_name'>
            {t}Samba SID mapping{/t}
        </div>
        <div class='step4_value'>
            <select name="sambaidmapping" size="1" title="">
			{html_options options=$bool selected=$sambaidmapping}
            </select>
        </div>
    </div>

    <p><b>{t}Additional GOsa settings{/t}</b></p>
	
    <div class='step4_container'>
        <div class='step4_name'>
            {t}GID / UID min id{/t}
        </div>
        <div class='step4_value'>
            {if $generic_settings.minid_active == FALSE}
                <input type='checkbox' value='1' name='minid_active'
                    onClick='changeState("minid");'>
                <input style='width:90%' id='minid' name='minid' type='text' value='{$generic_settings.minid}' disabled>
            {else}
                <input type='checkbox' value='1' name='minid_active' checked>
                <input style='width:90%' id='minid' name='minid' type='text' value='{$generic_settings.minid}'  >
            {/if}
        </div>
    </div>

    <div class='step4_container'>
        <div class='step4_name'>
            {t}Relaxed naming policies{/t}
        </div>
        <div class='step4_value'>
            <select name="strict" size="1" title="">
			{html_options options=$bool selected=$strict}
            </select>
        </div>
    </div>

    <div class='step4_container'>
        <div class='step4_name'>
            {t}Government mode{/t}
        </div>
        <div class='step4_value'>
            <select name="governmentmode" size="1" title="">
			{html_options options=$bool selected=$governmentmode}
			</select>
        </div>
    </div>

	
    <div class='step4_container'>
        <div class='step4_name'>
            {t}Automatic uids{/t}
        </div>
        <div class='step4_value'>
            {if $generic_settings.idgen_active == FALSE}
                <input type='checkbox' value='1' name='idgen_active'
                    onClick='changeState("idgen");'>
                <input style='width:90%' id='idgen' name='idgen' type='text' value='{$generic_settings.idgen}' disabled>
            {else}
                <input type='checkbox' value='1' name='idgen_active' checked>
                <input style='width:90%' id='idgen' name='idgen' type='text' value='{$generic_settings.idgen}'  >
            {/if}
        </div>
    </div>
    <div class='step4_container'>
        <div class='step4_name'>
            {t}Use account expiration{/t}
        </div>
        <div class='step4_value'>
            <select name="account_expiration" size="1" title="">
			{html_options options=$bool selected=$account_expiration}
            </select>
        </div>
    </div>
	<div class='step4_container' style='height:12em;'>
       <p><b>{t}Snapshots / Undo{/t}</b></p>
        <div class='step4_name'  style='height:12em;width:60%;background-color:red;'>
            {if $generic_settings.snapshot_active == FALSE}
                <input type='checkbox' value='1' name='snapshot_active'
                    onClick='	
								changeState("snapshot_base");
								changeState("snapshot_ldap_base");
								changeState("snapshot_server");							
								changeState("snapshot_user");							
								changeState("snapshot_password");'>{t}Enable{/t}<br>
		{t}Snapshot base{/t}&nbsp;
		<input style='width:220' id='snapshot_base' name='snapshot_base' 
					type='text' value='{$generic_settings.snapshot_base}' disabled><br>
            	    {t}Ldap base{/t}&nbsp;
				<input style='width:220' id='snapshot_ldap_base' name='snapshot_ldap_base' 
					type='text' value='{$generic_settings.snapshot_ldap_base}' disabled><br>
    	            {t}Server{/t}&nbsp;
				<input style='width:220' id='snapshot_server' name='snapshot_server' 
					type='text' value='{$generic_settings.snapshot_server}' disabled><br>
	                {t}User{/t}&nbsp;
				<input style='width:220' id='snapshot_user' name='snapshot_user' 
					type='text' value='{$generic_settings.snapshot_user}' disabled><br>
	                {t}Password{/t}&nbsp;
				<input style='width:220' id='snapshot_password' name='snapshot_password' 
					type='password' value='{$generic_settings.snapshot_password}' disabled>
            {else}
                <input type='checkbox' value='1' name='snapshot_active' checked>{t}Enable{/t}<br>
            	    {t}Snapshot base{/t}&nbsp;
				<input style='width:220' id='snapshot_base' name='snapshot_base' 
					type='text' value='{$generic_settings.snapshot_base}'><br>
            	    {t}Ldap base{/t}&nbsp;
				<input style='width:220' id='snapshot_ldap_base' name='snapshot_ldap_base' 
					type='text' value='{$generic_settings.snapshot_ldap_base}'><br>
        	        {t}Server{/t}&nbsp;
				<input style='width:220' id='snapshot_server' name='snapshot_server' 
					type='text' value='{$generic_settings.snapshot_server}'><br>
    	            {t}User{/t}&nbsp;
				<input style='width:220' id='snapshot_user' name='snapshot_user' 
					type='text' value='{$generic_settings.snapshot_user}'><br>
	                {t}Password{/t}&nbsp;
				<input style='width:220;' id='snapshot_password' name='snapshot_password' 
					type='password' value='{$generic_settings.snapshot_password}'>
            {/if}
        </div>
    </div>
</div>

<input type='hidden' value='1' name='step6_posted'>
<div style="clear:both"></div>
