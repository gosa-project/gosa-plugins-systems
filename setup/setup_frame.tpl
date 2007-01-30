<body class='setup_body'>
{$php_errors}
<form action='setup.php' name='mainform' method='post' enctype='multipart/form-data'>

<!-- All over div -->
<div class='setup_dialog'>

	<!-- Basic header  -->
	<div class='setup_header'>
		GOsa Setup blabla
	</div>

	<!-- Navigation div   -->
	<div class='setup_navigation'> 
		{$navigation}			
	</div>

	<!-- Setup step div -->
	<div  style='clear: right'>i

		<!-- Div to skip horizontal scrolling -->	
		<div style='float: top; clear: top ;padding:0px;width:100%' >

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
