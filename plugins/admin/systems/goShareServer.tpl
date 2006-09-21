<h2><img class="center" alt="" align="middle" src="images/rightarrow.png" /> {t}Shares{/t}</h2>
   <table summary="" style="width:100%">
    <tr>
     <td>
{render acl=$goExportEntryACL}
        <select style="width:100%" id="goExportEntry" name="goExportEntryList[]" size=12 multiple >
            {html_options values=$goExportEntry output=$goExportEntryKeys}
            <option disabled>&nbsp;</option>
        </select>
{/render}
    <br>
	
        <input type="submit"    value="{t}Add{/t}"  	name="NewNfsAdd"   id="NewNfsAddId" {if !$createable} disabled {/if}>
{render acl=$goExportEntryACL}
        <input type="submit"    value="{t}Edit{/t}"     name="NewNfsEdit"  id="NewNfsEditId">
{/render}
        <input type="submit"    value="{t}Delete{/t}"   name="DelNfsEnt"   id="DelNfsEntId" {if !$removeable} disabled {/if}>
    </td>
    </tr>
   </table>


<p class='seperator'>&nbsp;</p>
<p>
<div style="width:100%; text-align:right;">
    <input type='submit' name='SaveService' value='{t}Save{/t}'>
    &nbsp;
    <input type='submit' name='CancelService' value='{t}Cancel{/t}'>
</div>
</p>
<input type="hidden" name="goShareServerPosted" value="1">
