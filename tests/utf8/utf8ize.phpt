--TEST--
arc_booster: utf8ize
--EXTENSIONS--
arc_booster
--FILE--
<?php

var_dump(ArcBooster\utf8ize(null));
var_dump(ArcBooster\utf8ize(''));
var_dump(ArcBooster\utf8ize('Some ASCII text.'));

$s = "this\x01 is a \x7f test string";
var_dump(ArcBooster\utf8ize($s) === $s);

$s = "\xc3\x9c \xc3\xbc \xe6\x9d\xb1!";
var_dump(ArcBooster\utf8ize($s) === $s);

$s = "\xc3 this has \xe6\x9d some invalid utf8 \xe6";
var_dump(ArcBooster\utf8ize($s) === "\xEF\xBF\xBD this has \xEF\xBF\xBD\xEF\xBF\xBD some invalid utf8 \xEF\xBF\xBD");

$s = "hon\xE7k";
var_dump(ArcBooster\utf8ize($s) === "hon\xEF\xBF\xBDk");

$s = "test\xE0\xB0\xB1\xE0";
var_dump(ArcBooster\utf8ize($s) === "test\xE0\xB0\xB1\xEF\xBF\xBD");

$s = "test\xE0\xB0\xB1\xE0\xE0";
var_dump(ArcBooster\utf8ize($s) === "test\xE0\xB0\xB1\xEF\xBF\xBD\xEF\xBF\xBD");

$s = "\xfc";
var_dump(ArcBooster\utf8ize($s) === "\xEF\xBF\xBD");

$s = "\xfc\xfc";
var_dump(ArcBooster\utf8ize($s) === "\xEF\xBF\xBD\xEF\xBF\xBD");

$s = "\xE2\x82\x28";
var_dump(ArcBooster\utf8ize($s) === "\xEF\xBF\xBD\xEF\xBF\xBD\x28");

var_dump(ArcBooster\utf8ize("A \xED\xA9\x98") === "A \xEF\xBF\xBD\xEF\xBF\xBD\xEF\xBF\xBD");

?>
--EXPECT--
string(0) ""
string(0) ""
string(16) "Some ASCII text."
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
