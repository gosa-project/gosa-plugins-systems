{if $RestoreMode}

<h2>{t}Restoring an object snapshot{/t}</h2>
<br>
<p class="seperator">
{t}This procedure will create a working snapshot of the selected object.{/t}
<br>
</p>
<p class="seperator">
<br>

	{if $CountSnapShots!=0}
		<b>{t}You will be able to restore from{/t}</b>
	{else}
		<b>{t}There are no available snapshots.{/t}</b>
	{/if}
<br>
<br>
</p>
<br>
<table summary="">
	<tr>
		<td>
			{if $CountSnapShots==0}
				{t}There is no snapshot available that could be restored.{/t}
			{else}
				{t}Choose a snapshot and click continue, to restore the snapshot.{/t}
			{/if}
		</td>
	</tr>
	<tr>
		<td>
			<select name="SnapShot" {if $CountSnapShots==0} disabled {/if}>
				{if $CountSnapShots==0} disabled 
					<option value="">&nbsp;{t}none{/t}&nbsp;</option>
				{else}
					<option value="">&nbsp;</option>
					{html_options options=$SnapShots}
				{/if}
			</select>
		</td>
	</tr>
</table>

<p class="seperator">&nbsp;</p>
<p>
<div style="width:100%; text-align:right;">
    <input type='submit' name='RestoreSnapshot' value='{t}Save{/t}' {if $CountSnapShots==0} disabled {/if}>
    &nbsp;
    <input type='submit' name='CancelSnapshot' value='{t}Cancel{/t}'>
</div></p>
{else}

<h2>{t}Creating an object snapshot{/t}</h2>
<br>
<p class="seperator">
{t}This procedure will create a working snapshot of the selected object.{/t}
<br>
</p>
<p class="seperator">
<br>
<b>{t}You will be able to restore from{/t}</b>
<br>
<br>
</p>
<br>
<table summary="">
	<tr>
		<td style="width:120px;">
			{t}Object{/t} 
		</td>
		<td> 
		   {$CurrentDN}
		</td>
	</tr>
	<tr>
		<td>
			{t}Date{/t} 
		</td>
		<td> 
		   {$CurrentDate}
		</td>
	</tr>
	<tr>
		<td style="vertical-align:top;" colspan="2">
			{t}Description{/t}<br> 
			<textarea name="CurrentDescription" style="width:400px;height:120px;">{$CurrentDescription}</textarea>
		</td>
	</tr>
</table>

<p class="seperator">&nbsp;</p>
<p>
<div style="width:100%; text-align:right;">
    <input type='submit' name='CreateSnapshot' value='{t}Save{/t}'>
    &nbsp;
    <input type='submit' name='CancelSnapshot' value='{t}Cancel{/t}'>
</div></p>
{/if}
