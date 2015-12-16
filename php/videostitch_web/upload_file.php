<html>
<body>
	<head>
		<title> 文件上传-VideoStitch  </title>
	</head>

	<br />

	<?php
		$obj=$_REQUEST["object"];
		$number=$_REQUEST["number"];
		echo "Object Name: " . $obj . "<br />" . "Channels: " . $number . "<br /> <br />";
	?>


		<?php
		echo '<form action="upload_impl.php" method="post" enctype="multipart/form-data">';

		for($i=0; $i<$number; $i++)
		{
			$j=$i+1;
			echo '<label for="file" style="line-height:185%;">File' . $j . ':</label>';
			echo '<input type="file" name="file[]" id="file"  />' . "<br />"; 
		}

		echo '<input type="hidden" name="object" value=' . $obj . '/>';
		echo "<br />";
		echo '<input type="submit" name="submit" value="Submit" />' . '</form>';

		?>

</body>
</html>
