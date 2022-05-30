<?php
//$output = shell_exec('libcamera-still --datetime');
//echo "<pre>$output</pre>";
$output = shell_exec('libcamera-still --datetime');
$output = shell_exec('mv *.jpg /var/www/html/photo/');

?>

<meta http-equiv="Refresh" content="0; url='../index.php'" /> 
