<body class='setup_body'>
{$php_errors}
<form action='setup.php' name='mainform' method='post' enctype='multipart/form-data'>

<!-- All over div -->
<div class='setup_dialog' style='float:left' >

	<!-- Basic header -->
	<div class='setup_header' style='float:bottom'>
		{t}GOsa - GOnicus System Administrator Setup{/t}
	</div>

	<!-- Navigation div -->
	<div class='setup_navigation' style='float:left;'>
		<div class='setup_navigation_title'>
			{t}Setup Steps{/t}
		</div>
	{$navigation}			
	</div>
	<div  class='setup_plug_header'>
		{$header}
	</div>

	<!-- Step content -->
	<div class='setup_content' >
		{$contents}
	</div>
</div>
</form>
</body>
</html>
