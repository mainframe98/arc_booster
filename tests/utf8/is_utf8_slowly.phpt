--TEST--
arc_booster: is_utf8_slowly
--EXTENSIONS--
arc_booster
--FILE--
<?php

var_dump(ArcBooster\is_utf8_slowly(null));
var_dump(ArcBooster\is_utf8_slowly(''));
var_dump(ArcBooster\is_utf8_slowly('a'));
var_dump(ArcBooster\is_utf8_slowly(str_repeat('abc', 3)));

var_dump(ArcBooster\is_utf8_slowly("\xc3\x9c \xc3\xbc \xe6\x9d\xb1!"));
var_dump(ArcBooster\is_utf8_slowly("\xc3 this has \xe6\x9d some invalid utf8 \xe6"));

// This is a character in the basic multilingual plane (sequence of more than 3 bytes).
var_dump(ArcBooster\is_utf8_slowly("\xf1\x81\x81\x81", false));
var_dump(ArcBooster\is_utf8_slowly("\xf1\x81\x81\x81", true));

// Valid, but not minimal representations. In both cases, this is not considered valid UTF-8.
var_dump(ArcBooster\is_utf8_slowly("\xf0\x81\x81\x81", false));
var_dump(ArcBooster\is_utf8_slowly("\xf0\x81\x81\x81", true));

var_dump(ArcBooster\is_utf8_slowly("\xE0\x80\xA0"));

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
