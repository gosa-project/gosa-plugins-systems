<body class='setup_body' >
<center>
{$php_errors}
<form action='setup.php' name='mainform' method='post' enctype='multipart/form-data'>
<input id='focus' name='focus' type='image' src='images/empty.png' style='width:0px; height:0px;' >
<div class='setup_dialog'>
	<div class='setup_header'>
		<img src='themes/default/images/go_logo.png' class='center' alt='GOsa'>
	</div>
	<div class='setup_menu'>
		<b>{t}GOsa setup wizard{/t}</b>
	</div>

	<div style="padding:10px;background-color:green;">

		<div class='setup_navigation'>
			{$navigation}		
		</div>
	
		<div class='setup_contents'>
				<div class='plugtop'>
					{$header}
				</div>
			<div style='padding-left:12px;'>
				{$contents}
			</div>
		</div>
	
	</div>
</div>

<div class='setup_bottom'>
	{$bottom}
</div>

<input type='hidden' name='setup_goto_step' value=''>
</form>
</center>
<div style='width:100%; text-align:right'>
	{$version}
</div>

<script type='javascript'>
	document.getElementById('focus').focus();
</script>
</body>
</html>
