--TEST--
arc_booster: coalesce
--EXTENSIONS--
arc_booster
--FILE--
<?php

$null = null;
var_dump(ArcBooster\coalesce($null, 'fallback'));
var_dump(ArcBooster\coalesce($null, $null, $null));
var_dump(ArcBooster\coalesce());
var_dump(ArcBooster\coalesce($null, 'fallback', null, true));

?>
--EXPECT--

string(8) "fallback"
NULL
NULL
string(8) "fallback"
