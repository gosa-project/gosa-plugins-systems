<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding="0" border="0">
 <tr>
  <td style="width:50%; vertical-align:top;">
   <h2><img class="center" alt="" align="middle" src="images/rightarrow.png" />&nbsp;{t}Generic{/t}</h2>
   <table summary="">
    <tr>
     <td><label for="mail">{t}Primary address{/t}</label>{$must}</td>
     <td>
{render acl=$mailACL}
	<input id="mail" name="mail" size=35 maxlength=65 value="{$mail}">
{/render}
     </td>
    </tr>
    <tr>
     <td><label for="gosaMailServer">{t}Server{/t}</label></td>
     <td>
{render acl=$gosaMailServerACL}
      <select size="1" id="gosaMailServer" name="gosaMailServer" title="{t}Specify the mail server where the user will be hosted on{/t}">
	{html_options values=$mailServers output=$mailServers selected=$gosaMailServer}
	<option disabled>&nbsp;</option>
      </select>
{/render}
     </td>
    </tr>
    <tr>
     <td>
      <br>
      {t}Quota usage{/t}
     </td>
     <td>
      <br>
      {if $quotadefined eq "true"}
      	{$quotausage}
      {else}
      {t}not defined{/t}
      {/if}
     </td>
    </tr>
    <tr>
     <td><label for="gosaMailQuota">{t}Quota size{/t}</label></td>
     <td>
{render acl=$gosaMailQuotaACL}
      <input id="gosaMailQuota" name="gosaMailQuota" size="6" align="middle" maxlength="60" {$gosaMailQuotaACL}
		value="{$gosaMailQuota}"> MB
{/render}
     </td>
    </tr>
   </table>
  </td>

  <td style="border-left:1px solid #A0A0A0">
   &nbsp;
  </td>

  <td style="vertical-align:top;">
   <h2><img class="center" alt="" align="middle" src="images/alternatemail.png" /><label for="alternates_list"> {t}Alternative addresses{/t}</label></h2>
{render acl=$gosaMailAlternateAddressACL}
   <select id="alternates_list" style="width:100%;height:100px;" name="alternates_list[]" size="15" multiple 
	title="{t}List of alternative mail addresses{/t}">
	{html_options values=$gosaMailAlternateAddress output=$gosaMailAlternateAddress}
	<option disabled>&nbsp;</option>
{/render}
   </select>
   <br />
{render acl=$gosaMailAlternateAddressACL}
   <input name="alternate_address" size="30" align="middle" maxlength="65" value="">
{/render}
{render acl=$gosaMailAlternateAddressACL}
   <input type=submit value="{t}Add{/t}" name="add_alternate">
{/render}
{render acl=$gosaMailAlternateAddressACL}
   <input type=submit value="{t}Delete{/t}" name="delete_alternate">
{/render}
  </td>
 </tr>
</table>

<p class="seperator">&nbsp;</p>

<h2><img class="center" alt="" align="middle" src="images/envelope.png" />&nbsp;{t}Mail options{/t}</h2>
<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding=4 border=0>
 <tr style="padding-bottom:0px;">
  <td style="width:50%">
{render acl=$gosaMailDeliveryModeIACL}
   <input type=checkbox name="drop_own_mails" value="1" {$drop_own_mails}
	title="{t}Select if you want to forward mails without getting own copies of them{/t}"> {t}No delivery to own mailbox{/t}
{/render}
   <br>
{render acl=$gosaMailDeliveryModeVACL}
   <input type=checkbox name="use_vacation" value="1" {$use_vacation}
	title="{t}Select to automatically response with the vacation message defined below{/t}"> {t}Activate vacation message{/t}
{/render}
  </td>
   <td rowspan=2 style="border-left:1px solid #A0A0A0">
   &nbsp;
  </td>
  <td style="vertical-align:top;">
{render acl=gosaMailDeliveryModesACL}
   <input type=checkbox name="use_spam_filter" value="1" {$use_spam_filter} title="{t}Select if you want to filter this mails through spamassassin{/t}">
{/render}
 <label for="gosaSpamSortLevel">{t}Move mails tagged with spam level greater than{/t}</label>
	
{render acl=$gosaSpamSortLevelACL}
   <select id="gosaSpamSortLevel" size="1" name="gosaSpamSortLevel" title="{t}Choose spam level - smaller values are more sensitive{/t}">
        {html_options values=$spamlevel output=$spamlevel selected=$gosaSpamSortLevel}
   </select>
{/render}
   <label for="gosaSpamMailbox">{t}to folder{/t}</label>
{render acl=$gosaSpamMailboxACL}
   <select size="1" id="gosaSpamMailbox" name="gosaSpamMailbox">
        	{html_options values=$spambox output=$spambox selected=$gosaSpamMailbox}
			<option disabled>&nbsp;</option>
   </select>
{/render}
   <br>
{render acl=$gosaMailDeliveryModeRACL}
   <input type=checkbox name="use_mailsize_limit" value="1" {$use_mailsize_limit}
	<label for="gosaMailMaxSize">{t}Reject mails bigger than{/t}</label> 
{/render}
{render acl=$gosaMailMaxSizeACL}
   <input id="gosaMailMaxSize" name="gosaMailMaxSize" size="6" align="middle" maxlength="30" {$gosaMailMaxSizeACL}
	value="{$gosaMailMaxSize}"> {t}MB{/t}
{/render}
  </td>
 </tr>
 <tr>
  <td style="vertical-align:top; width:45%">
   <p style="margin-bottom:0px;">
    <b><label for="gosaVacationMessage">{t}Vacation message{/t}</label></b>
   </p>
{render acl=$gosaVacationMessageACL}
   <textarea id="gosaVacationMessage" style="width:99%; height:100px;" name="gosaVacationMessage" rows="4" cols="512"
	>{$gosaVacationMessage}</textarea>
{/render}
   <br>

   {if $show_templates eq "true"}
{render acl=$gosaVacationMessageACL}
   <select name="vacation_template">
        	{html_options options=$vacationtemplates selected=$template}
		<option disabled>&nbsp;</option>
   </select>
{/render}
{render acl=$gosaVacationMessageACL}
   <input type="submit" value="{t}Import{/t}" name="import_vacation">
{/render}
   {/if}
  </td>
  <td>
   <p style="margin-bottom:0px;">
    <b><label for="forwarder_list">{t}Forward messages to{/t}</label></b>
   </p>
{render acl=$gosaMailForwardingAddressACL}
   <select id="gosaMailForwardingAddress" style="width:100%; height:100px;" name="forwarder_list[]" size=15 multiple >
			{html_options values=$gosaMailForwardingAddress output=$gosaMailForwardingAddress selected=$template}        
			<option disabled>&nbsp;</option>
   </select>
{/render}
   <br>
{render acl=$gosaMailForwardingAddressACL}
   <input name="forward_address" size=20 align="middle" maxlength=65 value="">
{/render}
{render acl=$gosaMailForwardingAddressACL}
   <input type="submit" value="{t}Add{/t}" name="add_forwarder" >&nbsp;
{/render}
{render acl=$gosaMailForwardingAddressACL}
   <input type="submit" value="{t}Add local{/t}" name="add_local_forwarder" >&nbsp;
{/render}
{render acl=$gosaMailForwardingAddressACL}
   <input type="submit" value="{t}Delete{/t}" name="delete_forwarder">
{/render}
  </td>
 </tr>
</table>

<p class="seperator">&nbsp;</p>

<h2><img class="center" alt="" align="middle" src="images/false.png" />&nbsp;{t}Advanced mail options{/t}</h2>

<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding="4" border="0">
 <tr>
  <td>
{render acl=$gosaMailDeliveryModeLACL}
   <input type=checkbox name="only_local" value="1" {$only_local} title="{t}Select if user can only send and receive inside his own domain{/t}">
{/render}
   {t}User is only allowed to send and receive local mails{/t}
  </td>
  <td style="border-left:1px solid #A0A0A0">
   &nbsp;
  </td>
  <td>

{render acl=$gosaMailDeliveryModeCACL}
   <input type=checkbox name="own_script" value="1" {$own_script}>
   {t}Use custom sieve script{/t} <b>({t}disables all Mail options!{/t})</b>
{/render}
  </td>
 </tr>
</table>

<input type="hidden" name="mailTab" value="mailTab">

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
  document.mainform.mail.focus();
  -->
</script>
