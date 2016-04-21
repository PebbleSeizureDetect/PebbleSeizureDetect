<?php
 $dir = 'location';

 // create new directory with 744 permissions if it does not exist yet
 // owner will be the user/group the PHP script is run under
 if ( !file_exists($dir) ) {
  mkdir ($dir, 0777);
 }

 date_default_timezone_set('America/Los_Angeles');
 file_put_contents ($dir.'/index.html', '<a href="http://google.com/maps?' . $_SERVER["QUERY_STRING"] . '">Click for last known location</a> (recorded at '. date('l jS \of F Y h:i:s A') . ' Pacific Time)');

