<body style='background-image:none;background-color: #F4F4F4; padding:10px;'>

{$php_errors}
<form action='setup.php' name='mainform' method='post' enctype='multipart/form-data'>
<table style='width:100%;'>
	<tr>
		<td colspan=2 style='border:1px solid #CCC; height: 40px; vertical-align:top;' >
			GOsa Setup blabla
		</td>
	</tr>
	<tr>
		<td style='border:1px solid #CCC; height: 450px; width:200px; vertical-align:top;' rowspan=2>
			{$navigation}			
		</td>
		<td colspan=2 style='border:1px solid #CCC; height: 40px; vertical-align:top;' >
			{$header}
		</td>
	</tr>
	<tr>
		<td style='border:1px solid #CCC; vertical-align:top;'>
			{$contents}
		</td>
	</tr>
</table>
</form>
</body>
</html>
