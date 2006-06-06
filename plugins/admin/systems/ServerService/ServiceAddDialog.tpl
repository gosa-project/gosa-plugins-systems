<div style="font-size: 18px;">
	<div style="font-size: 18px;">
		{t}Add a new service to this server.{/t}.
	</div>
</div>
<br>
<p class="seperator"><b>
{t}This dialog allows you to add new services to the currenty edited server object. In the box below you can see all available services, services you have already in use are not shown.{/t} </b>
<br>
<br>
</p>
{if $Services} 

<br>

<b>{t}Please choose a service listed below.{/t}</b>
<br>
<br>
<br>{t}Available services{/t} &nbsp;
	<select name="ServiceName" >
		{html_options options=$Services }
	</select>
{else}
	<br>
	{t}There are no more services available, if you think that there is a service missing. Please check your gosa configuration file.{/t}
	<br>
{/if}
<br>
<br>
<p class="seperator">
<p>
<div style="width:100%; text-align:right;">
    <input type='submit' name='SaveServiceAdd' value='{t}Save{/t}' {if !$Services} disabled {/if}>
    &nbsp;
    <input type='submit' name='CancelServiceAdd' value='{t}Cancel{/t}'>
</div>
</p>

