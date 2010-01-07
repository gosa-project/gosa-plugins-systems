<div class="contentboxh">
 <p class="contentboxh">
  <img src="images/launch.png" align="right" alt="[F]">{t}Filter{/t}
 </p>
</div>

<div class="contentboxb">
  <div style="border-top:1px solid #AAAAAA"></div>

  {if $USE_server}
  {$SERVER}&nbsp;{t}Show servers{/t}<br>
  {/if}
  {if $USE_workstation}
  {$WORKSTATION}&nbsp;{t}Show workstations{/t}<br>
  {/if}
  {if $USE_terminal}
  {$TERMINAL}&nbsp;{t}Show terminals{/t}<br>
  {/if}
  {if $USE_printer}
  {$PRINTER}&nbsp;{t}Show network printer{/t}<br>
  {/if}
  {if $USE_phone}
  {$PHONE}&nbsp;{t}Show phones{/t}<br>
  {/if}
  {if $USE_winstation}
  {$WINSTATION}&nbsp;{t}Show windows based workstations{/t}<br>
  {/if}
  {if $USE_component}
  {$COMPONENT}&nbsp;{t}Show network devices{/t}<br>
  {/if}
  {if $USE_incoming}
  {$INCOMING}&nbsp;{t}Show incoming devices{/t}<br>
  {/if}
  {if $USE_opsi_client}
  {$OPSI}&nbsp;{t}Show OPSI based clients{/t}<br>
  {/if}

  <div style="border-top:1px solid #AAAAAA"></div>

 {$SCOPE}

 <table summary="" style="width:100%;border-top:1px solid #B0B0B0;">
  <tr>
   <td>
    <label for="NAME">
     <img src="images/lists/search.png" align=middle>&nbsp;Name
    </label>
   </td>
   <td>
    {$NAME}
   </td>
  </tr>
 </table>

 <table summary=""  width="100%"  style="background:#EEEEEE;border-top:1px solid #B0B0B0;">
  <tr>
   <td width="100%" align="right">
    {$APPLY}
   </td>
  </tr>
 </table>
</div>
