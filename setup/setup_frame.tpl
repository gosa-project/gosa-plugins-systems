<body class='setup_body' >
<center>
{$php_errors}
<form action='setup.php' name='mainform' method='post' enctype='multipart/form-data'>
<input id='focus' name='focus' type='image' src='images/empty.png' style='width:1px; height:1px;' >
<div class='setup_dialog'>
	<div class='setup_header'>
		{t}GOsa - GOnicus System Administrator Setup{/t}
	</div>
	<div style="width:100%; ">

		<div class='setup_navigation'>
			<div class='setup_navigation_title'>	
				{t}Setup steps{/t}
			</div>
			{$navigation}		
		</div>

		<div class="setup_plug_header_container">
			<div class='setup_plug_header'>
				{$header}
			</div>

		</div>
		<div class="setup_contents_container">
			<div class='setup_contents'>
				{$contents}
			</div>
		</div>

		<div class='setup_bottom'>
			Bottom	
		</div>
	</div>
</div>
<input type='hidden' name='setup_goto_step' value=''>
</form>
</center>

<script type='javascript'>
	document.getElementById('focus').focus();
</script>
</body>
</html>
