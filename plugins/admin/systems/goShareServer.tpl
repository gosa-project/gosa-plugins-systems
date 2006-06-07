<b>{t}Shares{/t}</b>
   <table summary="" style="width:100%">
    <tr>
     <td>
        <select style="width:100%" id="goExportEntry" name="goExportEntryList[]" {$goExportEntryACL} size=12 multiple >
            {html_options values=$goExportEntry output=$goExportEntryKeys}
            <option disabled>&nbsp;</option>
        </select>
    <br>
        <input type="submit"    value="{t}Add{/t}"  name="NewNfsAdd"   {$goExportEntryACL}  id="NewNfsAddId">
        <input type="submit"    value="{t}Edit{/t}"     name="NewNfsEdit"  {$goExportEntryACL}  id="NewNfsEditId">
        <input type="submit"    value="{t}Delete{/t}"   name="DelNfsEnt"   {$goExportEntryACL}  id="DelNfsEntId">
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
