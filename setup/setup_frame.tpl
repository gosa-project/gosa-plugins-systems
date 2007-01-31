<body class='setup_body'>
{$php_errors}
<form action='setup.php' name='mainform' method='post' enctype='multipart/form-data'>

<!-- All over div -->
<div class='setup_dialog'>

	<!-- Basic header  -->
	<div class='setup_header'>
		{t}GOsa - GOnicus System Administrator Setup{/t}
	</div>

	<!-- Navigation div   -->
	<div class='setup_navigation'>
		<div class='setup_navigation_border' >
			<div class='setup_navigation_title'>
				{t}Setup Steps{/t}
			</div>
			{$navigation}			
		</div>
	</div>

	<!-- Setup step div -->
	<div  class='setup_header_and_content_container_1'>

		<!-- Div to skip horizontal scrolling -->	
		<div class='setup_header_and_content_container_2'> 

			<!-- Step header -->	
			<div class='setup_plug_header' >
				{$header}
			</div>
	
			<!-- Step content -->
			<div class='setup_content'>
				{$contents}
			</div>
		</div>
	</div>
</div>
</form>
</body>
</html>
