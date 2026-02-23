--TEST--
arc_booster: nonempty
--EXTENSIONS--
arc_booster
--FILE--
<?php

$null = null;
var_dump(ArcBooster\nonempty($null, 'fallback'));
var_dump(ArcBooster\nonempty($null, 0, false));
var_dump(ArcBooster\nonempty());
var_dump(ArcBooster\nonempty($null, 'fallback', null, true));

?>
--EXPECT--

string(8) "fallback"
bool(false)
NULL
string(8) "fallback"
