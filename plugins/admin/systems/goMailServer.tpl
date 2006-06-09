<h2><img class="center" alt="" align="middle" src="images/rightarrow.png" /> {t}Generic{/t}</h2>
<table  style="width:100%;">
	<tr>
		<td width="50%" style="vertical-align:top">
			<table >
				<tr>
					<td>{t}Visible full qualified hostname{/t}
					</td>
					<td>
						<input type="text" name='postfixMyhostname' value='{$postfixMyhostname}' title='{t}The full qualified host name.{/t}'>
					</td>
				</tr>
				<tr>
					<td>{t}Max mail header size{/t}
					</td>
					<td>
						<input type="text" name='postfixHeaderSizeLimit' value='{$postfixHeaderSizeLimit}' 
									title='{t}This value specifies the maximal header size.{/t}'>&nbsp;{t}KB{/t}
					</td>
				</tr>
				<tr>
					<td>{t}Max mailbox size{/t}
					</td>
					<td>
						<input type="text" name='postfixMailboxSizeLimit' value='{$postfixMailboxSizeLimit}' 
									title='{t}Defines the maximal size of mail box.{/t}'>&nbsp;{t}KB{/t}
					</td>
				</tr>
				<tr>
					<td>{t}Max message size{/t}
					</td>
					<td>
						<input type="text" name='postfixMessageSizeLimit' value='{$postfixMessageSizeLimit}' 
									title='{t}Specify the maximal size of a message.{/t}'>&nbsp;{t}KB{/t}
					</td>
				</tr>
				<tr>
					<td>{t}Relay host{/t}
					</td>
					<td>
						<input type="text" name='postfixRelayhost' value='{$postfixRelayhost}' 
									title='{t}Relay messages to following host.{/t}'>
					</td>
				</tr>
			</table>
		</td>
		<td style="border-left:1px solid #A0A0A0; ">
			<table style="width:100%;">
				<tr>
					<td>
						{t}Local networks{/t}<br>
						<select name='Select_postfixMyNetworks[]' multiple size=6 style='width:100%;' title='{t}Postfix networks{/t}'>
							{html_options options=$postfixMyNetworks}
						</select>
						<input type="text" name="NewString_postfixMyNetworks" value="">
						<input type="submit" name="AddpostfixMyNetworks" value="{t}Add{/t}">
						<input type="submit" name="DelpostfixMyNetworks" value="{t}Remove{/t}">
					</td>
				</tr>
			</table>
		</td>
	</tr>
	<tr>
		<td colspan="2">
			<p class='seperator'>&nbsp;</p>
			<h2>{t}Domains and routing{/t}</h2>
		</td>
	</tr>	
	<tr>
		<td>
			  <table style="width:100%;">
                <tr>
                    <td>
                        {t}Domains to accept mail for{/t}<br>
                        <select name='Select_postfixMyDestinations[]' multiple size=6 style='width:100%;' title='{t}Postfix is responsilbe for the follwing domains.{/t}'>
                            {html_options options=$postfixMyDestinations}
                        </select>
                        <input type="text" name="NewString_postfixMyDestinations" value="">
                        <input type="submit" name="AddpostfixMyDestinations" value="{t}Add{/t}">
                        <input type="submit" name="DelpostfixMyDestinations" value="{t}Remove{/t}">
                    </td>
                </tr>
            </table>
		</td>
		<td style="border-left:1px solid #A0A0A0; ">
			  <table style="width:100%;">
                <tr>
                    <td>
                        {t}Transports{/t}<br>
						{$Div_postfixTransportTable}
                        <input type="text" name="Source_postfixTransportTable" value="">
                        <select name='TransportProtocol' title='{t}Select a transport protocoll.{/t}'>
                            {html_options options=$TransportProtocols}
                        </select>
                        <input type="text" name="Destination_postfixTransportTable" value="">
                        <input type="submit" name="AddpostfixTransportTable" value="{t}Add{/t}">
                    </td>
                </tr>
            </table>
		</td>
	</tr>
	<tr>
		<td colspan="2">
			<p class='seperator'>&nbsp;</p>
			<h2>{t}Restrictions{/t}</h2>
		</td>
	</tr>	
	<tr>
		<td>
            <table style="width:100%;">
                <tr>
                    <td>
                        {t}Restrictions for sender{/t}<br>
						{$Div_postfixSenderRestrictions}
                        <input type="text" name="Source_postfixSenderRestrictions" value="">
                        <select name='SenderRestrictionFilter' title='{t}Restriction filter{/t}'>
                            {html_options options=$RestrictionFilters}
                        </select>
                        <input type="text" name="Destination_postfixSenderRestrictions" value="">
                        <input type="submit" name="AddpostfixSenderRestrictions" value="{t}Add{/t}">
                    </td>
                </tr>
            </table>
		</td>
		<td style="border-left:1px solid #A0A0A0;">
            <table style="width:100%;">
                <tr>
                    <td>
                        {t}Restrictions for recipient{/t}<br>
						{$Div_postfixRecipientRestrictions}
                        <input type="text" name="Source_postfixRecipientRestrictions" value="">
                        <select name='RecipientRestrictionFilter' title='{t}Restriction filter{/t}'>
                            {html_options options=$RestrictionFilters}
                        </select>
                        <input type="text" name="Destination_postfixRecipientRestrictions" value="">
                        <input type="submit" name="AddpostfixRecipientRestrictions" value="{t}Add{/t}">
						
                    </td>
                </tr>
            </table>
		</td>
	</tr>	
</table>

<p class='seperator'>&nbsp;</p>
<br>
<h2><img class="center" alt="" src="images/rocket.png" align="middle"> Action</h2>
{if $is_new == "new"}
	{t}The server must be saved before you can use the status flag.{/t}
{elseif !$is_acc}
	{t}The service must be saved before you can use the status flag.{/t}
{/if}
<br>
<select name="action" title='{t}Set new status{/t}' 
	{if $is_new == "new" || $is_acc == false} disabled {/if}
>
	<option value="none">&nbsp;</option>
	{html_options options=$Actions}	
</select>
<input type='submit' name='ExecAction' title='{t}Set status{/t}' value='{t}Execute{/t}' 
	{if $is_new == "new" || $is_acc == false} disabled {/if}
>

<p class="seperator">&nbsp;</p>
<p>
<div style="width:100%; text-align:right;">
    <input type='submit' name='SaveService' value='{t}Save{/t}'>
    &nbsp;
    <input type='submit' name='CancelService' value='{t}Cancel{/t}'>
</div>
</p>

