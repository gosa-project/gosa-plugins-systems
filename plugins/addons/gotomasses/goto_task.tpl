<table style='width:100%'>
	<tr>
		<td colspan="2">
			<h2><img alt="" src="images/head.png" class="center" align="middle"> {t}Generic{/t}</h2>
		</td>
	</tr>
	<tr>
		<td style='width:50%;border-right: 1px solid rgb(160, 160, 160);'>
			<table>
				<tr>
					<td>
						{t}Description{/t}
					</td>
					<td>
						{render acl=$CommentACL}
						<input type="text" name="Comment" value="{$Comment}">
						{/render}
					</td>
				</tr>
				<tr>
					<td>
						{t}Object group membership{/t}
					</td>
					<td>
						{render acl=$OGroupACL}
						<select name="OGroup">
							{html_options options=$OGroups selected=$OGroup}
						</select>
						{/render}
					</td>
				</tr>
			</table>
		</td>
		<td>
			<table>
				<tr>
					<td>
						{t}Action{/t}
					</td>
					<td>
						{render acl=$ActionACL}
						<select name="Action">
							{html_options options=$Actions selected=$Action}
						</select>
						{/render}
					</td>
				</tr>
			</table>

		</td>
	</tr>
</table>
<p class='seperator'>&nbsp;</p>
<table style='width:100%;'>
	<tr>
		<td colspan="2">
			<h2><img alt="" src="images/time.png" class="center" align="middle"> {t}Schedule{/t}</h2>
		</td>
	</tr>
	<tr>
		<td colspan="2">
			{t}Minute{/t}
			{render acl=$MinuteACL}
				<select name='Minute'>
				{html_options options=$Minutes selected=$Minute}
				</select>
			{/render}
			{t}Hour{/t}
			{render acl=$HourACL}
				<select name='Hour'>
				{html_options options=$Hours selected=$Hour}
				</select>
			{/render}
			{t}Day{/t}
			{render acl=$DayACL}
				<select name='Day'>
				{html_options options=$Days selected=$Day}
				</select>
			{/render}
			{t}Month{/t}
			{render acl=$MonthACL}
				<select name='Month'>
				{html_options options=$Months selected=$Month}
				</select>
			{/render}
			{t}Week day{/t}
			{render acl=$WeekdayACL}
				<select name='Weekday'>
				{html_options options=$Weekdays selected=$Weekday}
				</select>
			{/render}
		</td>
	</tr>
</table>
<p class='seperator'>&nbsp;</p>
<table style='width:100%;'>
	<tr>
		<td colspan="2" style='padding-top:5px;'>
			<h2><img alt="" src="images/select_workstation.png" class="center" align="middle"> {t}Target systems{/t}</h2>
		</td>
	</tr>
	<tr>
		<td colspan="2">
            <table  summary="" style="border:1px solid #B0B0B0; " cellspacing=0 cellpadding=0>
                <tr>
                    <td>
						{render acl=$TargetACL}
						{$Target_list}	
						{/render}
					</td>
				</tr>
			</table>
						{render acl=$TargetACL}
						<input type="text" name="target_text" value="">
						{/render}
						{render acl=$TargetACL}
						<input type="submit" name="add_target" value="{t}Add{/t}">
						{/render}
						{render acl=$TargetACL}
						<input type="submit" name="add_from_list" value="{t}Add from list{/t}">
						{/render}

		</td>
	</tr>
</table>
<input type='hidden' name='goto_task_posted' value='1'>
<p style="text-align:right">
	<input type='submit' name='save_goto_task' value='{t}Ok{/t}'>
{if !$new}
	<input type='submit' name='apply_goto_task' value='{t}Apply{/t}'>
{/if}
	<input type='submit' name='close_goto_task' value='{t}Abort{/t}'>
</p>
