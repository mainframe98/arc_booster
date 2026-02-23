--TEST--
arc_booster: mbstring interoperability
--EXTENSIONS--
arc_booster
mbstring
--FILE--
<?php

var_dump(phutil_utf8_strlen(''));

?>
--EXPECT--
int(0)
