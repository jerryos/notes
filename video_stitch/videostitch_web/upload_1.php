<html>
<body>
	<head>
		<title> 普通文件上传-VideoStitch  </title>
	</head>

	<br />

	<?php
		function getDir($dir) 
		{
		    $dirArray[]=NULL;
		    
			if (false != ($handle = opendir ( $dir ))) 
			{
       			 $i=0;
       			 while ( false !== ($file = readdir ( $handle )) ) 
				{
            		if (filetype($dir.$file) =='dir' && $file != "." && $file != "..") 
					{
                		$dirArray[$i]=$file;
               		 	$i++;
           		 	}
        		}
       			closedir ( $handle );
    		}
    		
			return $dirArray;
		}	

		$dirlist=getDir("./videostitch/");

/*
		foreach ($dirlist as $dir)
		{
    		echo $dir;
    	//	echo "<br />";
    		echo "\n";
		}
*/

//		echo '<form method="post" action="upload_1_impl.php" >';
		
		echo '<form action="upload_1_impl.php" method="post" enctype="multipart/form-data">';
	
		echo '<font face="arial" size="2"  color="black"> 选择上传目标目录:  </font>';
		echo "<select name='select'>";
		foreach ($dirlist as $dir)
		{
			if($dir == "other")
			{
  				echo '<option value =' . $dir . ' selected="selected">' . $dir . '</option>';
			}
			else
			{
  				echo '<option value =' . $dir . '>' . $dir . '</option>';
			}
		}
		echo "</select>" . "<br /> <br />";

	
		echo '<label for="file"> FileName : </label>';
    	echo '<input type="file" name="file" id ="file"/>' . "<br />";

		echo ' <input type="submit" style="width:70px;height:25px;">';
		echo " </form>";



	?>
	


</body>
</html>
