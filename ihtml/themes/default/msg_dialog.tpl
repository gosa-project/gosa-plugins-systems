{if $IE}
	<iframe id='e_layer3{$i_ID}'
		style="
			position:absolute;
			width:100%;
			height:100%;
			top:0px;
			left:0px;
			border:none;
			border-style:none;
			border-width:0pt;
			display:block;
			allowtransparency='true';
			background-color: #FFFFFF;
			filter:chroma(color=#FFFFFF);
			z-index:0; ">
	</iframe>
	<div  id='e_layer2{$i_ID}'
		style="
			position: absolute;
			left: 0px;
			top: 0px;
			right:0px;
			bottom:0px;
			z-index:0;
			width:100%;
			height:100%;
			filter:progid:DXImageTransform.Microsoft.AlphaImageLoader(enabled=true, sizingMethod=scale, src='images/opacity_black.png'); ">
{else}
	<div  id='e_layer2{$i_ID}'>
<!--		style="
			position: absolute;
			left: 0px;
			top: 0px;
			right:0px;
			bottom:0px;
			z-index:0;
			background-image: url(images/opacity_black.png);">
-->
{/if}

{if $i_Type == ERROR_DIALOG}
	<div id='e_layer{$i_ID}'
			style='left:20%;
				right:20%;
				top:30%;
				background-color:white;
				padding:5px;
				border:5px solid red;
				z-index:150;
				position:absolute;'>
		<table style='width:100%' summary='' border=0>
			<tr>
				<td style='vertical-align:top;padding:10px'>
					<img src='images/error.png' alt='{t}Error{/t}'>
				<img alt='' src=''>
				</td>
				<td style='width:100%'>
					<h1>{$s_Title}</h1>
					<b>{$s_Message}</b>
					<br>
					<br>
				</td>
			</tr>
			<tr>
				<td colspan='2' align='center'>
					<button type='button' name='MSG_OK{$i_ID}' onClick='hide("e_layer{$i_ID}");hide("e_layer2{$i_ID}");'>{t}Ok{/t}</button>
				</td>
			</tr>
		</table>
	</div>
</div>
{elseif $i_Type == WARNING_DIALOG}
	<div id='e_layer{$i_ID}'
			style='left:20%;
				right:20%;
				top:30%;
				background-color:white;
				padding:5px;
				border:5px solid yellow;
				z-index:150;
				position:absolute;'>
		<table style='width:100%' summary='' border=0>
			<tr>
				<td style='vertical-align:top;padding:10px'>
					<img src='images/warning.png'  alt='{t}Warning{/t}'>
				</td>
				<td style='width:100%'>
					<h1>{$s_Title}</h1>
					<b>{$s_Message}</b>
					<br>
					<br>
				</td>
			</tr>
			<tr>
				<td colspan='2' align='center'>
					<button type='button' name='MSG_OK{$i_ID}' onClick='hide("e_layer{$i_ID}");hide("e_layer2{$i_ID}");'>{t}Ok{/t}</button>
				</td>
			</tr>
		</table>
	</div>
</div>
{elseif $i_Type == INFO_DIALOG}
	<div id='e_layer{$i_ID}'
			style='left:20%;
				right:20%;
				top:30%;
				background-color:white;
				padding:5px;
				border:5px solid green;
				z-index:150;
				position:absolute;'>
		<table style='width:100%' summary='' border=0>
			<tr>
				<td style='vertical-align:top;padding:10px'>
					<img src='images/info.png' alt='{t}Information{/t}'>
				</td>
				<td style='width:100%'>
					<h1>{$s_Title}</h1>
					<b>{$s_Message}</b>
				</td>
			</tr>
			<tr>
				<td colspan='2' align='center'>
						<button type='button' name='MSG_OK{$i_ID}' onClick='hide("e_layer{$i_ID}");hide("e_layer2{$i_ID}");'>{t}Ok{/t}</button>
				</td>
			</tr>
		</table>
	</div>
</div>
{elseif $i_Type == CONFIRM_DIALOG}
	<div id='e_layer{$i_ID}'
			style='left:20%;
				right:20%;
				top:30%;
				background-color:white;
				padding:5px;
				border:5px solid green;
				z-index:150;
				position:absolute;'>
		<table style='width:100%' summary='' border=0>
			<tr>
				<td style='vertical-align:top;padding:10px'>
					<img src='images/info.png' alt='{t}Information{/t}'>
				</td>
				<td style='width:100%'>
					<h1>{$s_Title}</h1>
					<b>{$s_Message}</b>
				</td>
			</tr>
			<tr>
				<td colspan='2' align='center'>
					<button type='submit' name='MSG_OK{$i_ID}'  	onClick='hide("e_layer{$i_ID}");hide("e_layer2{$i_ID}");'>{t}Ok{/t}</button>
					<button type='button' name='MSG_CANCEL{$i_ID}' onClick='hide("e_layer{$i_ID}");hide("e_layer2{$i_ID}");'>{t}Cancel{/t}</button>
				</td>
			</tr>
		</table>
	</div>
</div>
{/if}
