<html>
<body>
<head>
	<title> 普通文件上传-VideoStitch  </title>
</head>

<br /> 
<?php

	if (ini_get('magic_quotes_gpc')) {
    	function stripslashesRecursive(array $array)
     	{
        	foreach ($array as $k => $v) {
				if (is_string($v)) {
					$array[$k] = stripslashes($v);
				} 
				else if (is_array($v)) {
					$array[$k] = stripslashesRecursive($v);
				}
			}
			return $array;
     	}
		$_GET = stripslashesRecursive($_GET);
		$_POST = stripslashesRecursive($_POST);
	}

	$select = $_REQUEST["select"];
	$dir = "videostitch/" . $select . "/";
//	echo $dir . "<br />";

	if ($_FILES["file"]["error"] > 0)
  	{
  		echo "Error: " . $_FILES["file"]["error"] . "<br />";
  	}
	else
	{
		$tmp_name = $_FILES["file"]["tmp_name"];
        $name = $_FILES["file"]["name"];
        $size = $_FILES["file"]["size"];
        $type = $_FILES["file"]["type"];
	
	  	echo "Name: " . $name . "<br />";
	  	echo "Type: " . $type . "<br />";
	  	echo "Size: " . $size . " kb<br />";

		if (file_exists($dir . $name))
	   	{
	   		echo $name . "  already exists. Do nothing!!!";
	   	}
	  	else
	   	{
	   		move_uploaded_file($tmp_name, $dir . $name );
	   		echo "Stored in: " . $dir . $name . "<br />";
	   	}

	}	

?>
	<br />
	 <form method="post" action="upload.php" >
            <input type="button" style="width:90px;height:40px;"  onclick="window.location.href='/'" value="返回首页" />
	<form />

</body>
</html>
