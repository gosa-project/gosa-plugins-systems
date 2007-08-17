<table style='width:100%;'>
	<tr>
		<td colspan="2">
			<h2>{t}Generic{/t}</h2>
		</td>
	</tr>
	<tr>
		<td style='width:50%;'>
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
	<tr>
		<td>
			<table>
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
	</tr>
	<tr>
		<td colspan="2">
			<h2>{t}Target systems{/t}</h2>
		</td>
	</tr>
	<tr>
		<td colspan="2">
            <table  summary="" style="border:1px solid #B0B0B0; " cellspacing=0 cellpadding=0>
                <tr>
                    <td>
						<div style='border-bottom: 1px solid #B0B0B0;'>
						{render acl=$TargetACL}
						{$Target_list}	
						{/render}
						</div>
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

		</td>
	</tr>
	<tr>
		<td colspan="2">
			<h2>{t}Schedule{/t}</h2>
		</td>
	</tr>
	<tr>
		<td colspan="2">
			{if $Expert}
			<input type='submit' name='ToggleExpertMode' value="{t}Normal mode{/t}">
			<table>
				<tr>
					<td style='width:20%;'>
						{t}Minute{/t}&nbsp;
						{render acl=$MinuteACL}
						<input type='text' name='Minute' value="{$Minute}" style='width:50px;'>
						{/render}
					</td>
					<td style='width:20%;'>
						{render acl=$HourACL}
						{t}Hour{/t}&nbsp;
						<input type='text' name='Hour' value="{$Hour}" style='width:50px;'>
						{/render}
					</td>
					<td style='width:20%;'>
						{t}Day{/t}&nbsp;
						{render acl=$DayACL}
						<input type='text' name='Day' value="{$Day}" style='width:50px;'>
						{/render}
					</td>
					<td style='width:20%;'>
						{t}Month{/t}&nbsp;
						{render acl=$MonthACL}
						<input type='text' name='Month' value="{$Month}" style='width:50px;'>
						{/render}
					</td>
					<td style='width:20%;'>
						{t}Week day{/t}&nbsp;
						{render acl=$WeekdayACL}
						<input type='text' name='Weekday' value="{$Weekday}" style='width:50px;'>
						{/render}
					</td>
				</tr>
			</table>
			{else}

			<input type='submit' name='ToggleExpertMode' value="{t}Expert mode{/t}">
			<table>
				<tr>
					<td>{t}Minute{/t}
					</td>
					<td>
						{render acl=$MinuteACL}
						<select name='Minute'>
							{html_options options=$Minutes selected=$Minute}
						</select>
						{/render}
					</td>
					<td>{t}Hour{/t}
					</td>
					<td>
						{render acl=$HourACL}
						<select name='Hour'>
							{html_options options=$Hours selected=$Hour}
						</select>
						{/render}
					</td>
					<td>{t}Day{/t}
					</td>
					<td>
						{render acl=$DayACL}
						<select name='Day'>
							{html_options options=$Days selected=$Day}
						</select>
						{/render}
					</td>
					<td>{t}Month{/t}
					</td>
					<td>
						{render acl=$MonthACL}
						<select name='Month'>
							{html_options options=$Months selected=$Month}
						</select>
						{/render}
					</td>
					<td>{t}Week day{/t}
					</td>
					<td>
						{render acl=$WeekdayACL}
						<select name='Weekday'>
							{html_options options=$Weekdays selected=$Weekday}
						</select>
						{/render}
					</td>
				</tr>
			</table>
			{/if}
	
		</td>
	</tr>
</table>
<input type='hidden' name='goto_task_posted' value='1'>
<p style="text-align:right">
	<input type='submit' name='close_goto_task' value='{t}Cancel{/t}'>
	<input type='submit' name='save_goto_task' value='{t}Apply{/t}'>
</p>
