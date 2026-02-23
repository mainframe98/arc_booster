--TEST--
arc_booster: utf8v
--EXTENSIONS--
arc_booster
--FILE--
<?php

var_dump(ArcBooster\utf8v('') === array());
var_dump(ArcBooster\utf8v('x') === array('x'));
var_dump(ArcBooster\utf8v('quack') === array('q', 'u', 'a', 'c', 'k'));
var_dump(ArcBooster\utf8v("x\xe6\x9d\xb1y") === array('x', "\xe6\x9d\xb1", 'y'));
var_dump(ArcBooster\utf8v("x\xCD\xA0y") === array('x', "\xCD\xA0", 'y'));

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
