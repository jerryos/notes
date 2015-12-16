<html>
<body>
<head>
	<title> 文件上传-VideoStitch  </title>
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

	$tt = $_POST["object"];
	$dir = "videostitch/projects/" . $tt;
//	echo $dir . "<br />";

	$newstr = substr($dir,0,strlen($dir)-1); 
//	echo $newstr . "<dir />";

//	$now=date("y-m-d-h:i:s", time());
	$now=date("y-m-d-h-i-s", time());
	$dir = $newstr . '-'  . $now . "/";
	echo $dir . "<br />";
	
	if (!file_exists($dir))
	{	 
		mkdir ($dir); 
		echo 'Creat folder ' . $tt . ' Success' . "<br />";
	}
	else 
	{
//		echo $tt . "exist, do nothing";
	}


	foreach($_FILES["file"]["error"] as $key => $err )
	{
		if ($err > 0)
	  	{
			switch($err){
				case "1":
					echo "文件超过单个文件大小upload_max_filesize的限定" . "<br />";
					continue;
				case "2":
					echo "上传文件的总大小超过post_max_size的限定" . "<br />";
					continue;
				case "3":
					echo "文件只有部分被上传" . "<br />";
					continue;
				case "4":
					echo "没有文件被上传" . "<br />";
					continue;
				case "6":
					echo "找不到临时文件夹 upload_tmp_dir" . "<br />";
					continue;
				case "7":
					echo "文件写入失败" . "<br />";
					continue;
			}	
	  	}
		else
	  	{
			$tmp_name = $_FILES["file"]["tmp_name"][$key];
            $name = $_FILES["file"]["name"][$key];
            $size = $_FILES["file"]["size"][$key];
            $type = $_FILES["file"]["type"][$key];

	  		echo "Type: " . $type . "<br />";
	  		echo "Size: " . $size . " kb<br />";
	  		echo "Temp file: " . $tmp_name . "<br />";

	  		if (file_exists($dir . $name))
	    	{
	    		echo $name . " already exists. Do nothing!!!";
	    	}
	  		else
	    	{
	    		move_uploaded_file($tmp_name, $dir . $name );
	    		echo "Stored in: " . $dir . $name . "<br />";
	    	}
	  	}
	}

?>
	<br />
	 <form method="post" action="upload.php" >
            <input type="button" style="width:90px;height:40px;"  onclick="window.location.href='/'" value="返回首页" />
            <input type="button" style="width:90px;height:40px;"  onclick="window.location.href='upload.php'" value="继续上传" />
	<form />

</body>
</html>
